// Winter 2019

#include "Final.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

#include <string>
#include <string.h>
#include "loadTexture.h"

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

const int roomDimensions = 60;

float playerStartWidth = 5.0f;
float playerStartHeight = 20.0f;

//----------------------------------------------------------------------------------------
// Constructor
Final::Final(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  playerWidth(playerStartWidth),
	  playerHeight(playerStartHeight),
	  moveUp(false),
	  moveDown(false),
	  moveLeft(false),
	  moveRight(false),
	  adjustMovementSpeedZ(0.2f),
	  adjustMovementSpeedX(0.4f),
	  maxJumpHeight(playerHeight * 0.4),
	  lastJumpHeight(0),
	  jump(false),
	  jumpStart(false),
	  jumpTime(0),
	  currJumpHeight(0),
	  xMouseStart(0.0), yMouseStart(0.0),
	  xCurr(0.0), yCurr(0.0),
	  leftPressed(false),
	  rightPressed(false),
	  middlePressed(false),
	  GameplayWindow(true),
	  InventoryWindow(false),
	  OptionWindow(false),
	  message(""),
	  shadowMap(true), 
	  textureMap(true), 
	  collisionDetection(true),
	  transparency(true),
	  m_fbo(0),
	  interactions(eyeVec, adjustMovementSpeedX, 
	  	adjustMovementSpeedZ, maxJumpHeight, popup,
	  	playerWidth, playerHeight, inventory, nodeVector, 
	  	message),
	  popup(false),
	  acceleration(-9.8),
	  zSlipTime(0.0), 
	  xSlipTime(0.0),
	  lastUp(false), 
	  lastLeft(false),
	  lastXSlip(0),
	  lastZSlip(0),
	  timerZ(0), timerX(0),
	  V0z(0), V0x(0)		
{

}

//----------------------------------------------------------------------------------------
// Destructor
Final::~Final()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void Final::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	renderTexturefiles();

	glGenVertexArrays(1, &m_vao_meshData);

	initializeFBO();

	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);


	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("room.obj"),
			getAssetFilePath("table.obj"),
			getAssetFilePath("sphere.obj")
			,getAssetFilePath("test.obj")
			,getAssetFilePath("doorKnob.obj")
			,getAssetFilePath("key.obj")
			,getAssetFilePath("cup.obj")
			,getAssetFilePath("painting.obj")

	});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// uploads max and min coordinate for each geometry node to create bounding box (static collisions)
	uploadBoundingBoxInfo(*meshConsolidator);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	initLightMatrices();

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void Final::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = import_lua(filename);
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}

	rootRotation = new SceneNode("rootRotation");			// add a node to handle rotations
	rootRotation->add_child(m_rootNode);
	rootRotation->set_transform(m_rootNode->get_transform());
	m_rootNode->set_transform(mat4(1.0f));

	rootTranslation = new SceneNode("rootTranslation");		// add node to handle translations
	rootTranslation->add_child(rootRotation);
	rootTranslation->set_transform(mat4(1.0f));

	int index = 0;
	createNodeVector(m_rootNode, index);

	m_rootNode->parent = rootRotation;						// set parent relations
	rootRotation->parent = rootTranslation;
	rootTranslation->parent = NULL;
}

//----------------------------------------------------------------------------------------
void Final::createNodeVector(SceneNode *node, int &index) {

	nodeVector.push_back(node);							// add node to vector
	node->m_nodeId = index;
	++index;											// increment index

	for (SceneNode *n : node->children) {
		n->parent = node;
		createNodeVector(n, index);
	}

	if ( node->m_name == "room" ) {						// set object we're standing on
		lastYCollisionNode = static_cast<GeometryNode *>(node);
		collisionNode = static_cast<GeometryNode *>(node);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Texture mapping - Reads in a texture file and applies it onto an object
 *
 */

void Final::renderTexturefiles() {

	textureQueenOfHeartsPortrait = loadBMP_custom("textures/queenPortrait6.bmp");
	textureRoom = loadBMP_custom("textures/room.bmp");
	textureDrinkMe = loadBMP_custom("textures/drinkMe.bmp");
	textureDoor = loadBMP_custom("textures/door.bmp");
	texturePainting1 = loadBMP_custom("textures/cupcakePainting.bmp");
	texturePainting2 = loadBMP_custom("textures/teaCupsPainting.bmp");
	texturePainting3 = loadBMP_custom("textures/cheshireCatPainting.bmp");

}

//----------------------------------------------------------------------------------------
void Final::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_depthShader.generateProgramObject();
	m_depthShader.attachVertexShader( getAssetFilePath("depth.vs").c_str() );
	m_depthShader.attachFragmentShader( getAssetFilePath("depth.fs").c_str() );
	m_depthShader.link();

}

//----------------------------------------------------------------------------------------
void Final::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		// Enable the vertex shader attribute location for "texture" when rendering
		m_uvAttribLocation = m_shader.getAttribLocation("inTextureCoord");
		glEnableVertexAttribArray(m_uvAttribLocation);

		// Enable the vertex shader attribute location for "depthPosition" when rendering
		m_depthPositionAttribLocation = m_depthShader.getAttribLocation("position");
		glEnableVertexAttribArray(m_depthPositionAttribLocation);


		CHECK_GL_ERRORS;

	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
// assigns bounding box coordinates to geometry nodes
void Final::uploadBoundingBoxInfo(MeshConsolidator & meshConsolidator) 
{
	for ( auto &it : nodeVector ) {
		if ( it->m_nodeType == NodeType::GeometryNode ) {
			GeometryNode * gNode = static_cast<GeometryNode *>(it);
			gNode->maxCoordinate = meshConsolidator.getMaxCoordInfo(gNode->meshId);
			gNode->minCoordinate = meshConsolidator.getMinCoordInfo(gNode->meshId);
		}
	}

}

//----------------------------------------------------------------------------------------
void Final::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex texture data
	{
		glGenBuffers(1, &m_vbo_vertexUV);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUV);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUVBytes(),
			meshConsolidator.getVertexUVDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

}

//----------------------------------------------------------------------------------------
    
void Final::initializeFBO() {

	glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);			// bind fbo for reading depth

	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_windowWidth, 
		m_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	float borderColour[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
    	m_depthTexture, 0);

    glDrawBuffer(GL_NONE);				// only need depth info, indicate no colour will be recorded
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    	cout << "framebuffer could not be created" << endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);			// bind to remove fbo

}


//----------------------------------------------------------------------------------------
void Final::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUV);
	glVertexAttribPointer(m_uvAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 
		nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_depthPositionAttribLocation, 
		3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}


//----------------------------------------------------------------------------------------
void Final::initPerspectiveMatrix()
{
	// for center of trackball circle
	xCenterCoord = m_windowWidth / 2;
	yCenterCoord = m_windowHeight / 2;

	// set radius of trackball circle
	double diameter = (m_windowWidth < m_windowHeight) ? m_windowWidth * 0.5 : m_windowHeight * 0.5;
	radius = diameter / 2;

	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perspective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void Final::initViewMatrix() {
	eyeVec = vec3(25.0, playerHeight-(roomDimensions/2), 25.0);
	viewVec = vec3(0.0, -0.5, -1.0);
	upVec = vec3(0.0, 1.0, 0.0);

	m_view = glm::lookAt(eyeVec, viewVec, upVec);
}

//----------------------------------------------------------------------------------------
void Final::initLightSources() {
	// World-space position
	m_light.position = vec3(29.9f, 29.9f, 29.9f);
	m_light.rgbIntensity = vec3(0.8f); // light
}

//
void Final::initLightMatrices() {
	
	// determine view from light POV
	m_lightViewMatrix = lookAt(m_light.position, vec3(0, -30.0, 0), upVec);
	m_lightModelMatrix = glm::mat4(1.0);
}

//----------------------------------------------------------------------------------------
void Final::uploadCommonSceneUniforms() {
	m_shader.enable();
	{

		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, do_picking ? 1 : 0 );
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("View");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_view));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("depthView");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_lightViewMatrix));
		CHECK_GL_ERRORS;

		if ( !do_picking ) {
			//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.05f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
	}
	m_shader.disable();

	m_depthShader.enable();
	{
		GLint location = m_depthShader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perspective));
		CHECK_GL_ERRORS;
		
		location = m_depthShader.getUniformLocation("View");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_lightViewMatrix));
		CHECK_GL_ERRORS;
	}
	m_depthShader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void Final::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();

}



//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void Final::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(200, 100), opacity,
			windowFlags);

    	if (ImGui::BeginMenuBar()) {

    		if ( ImGui::Button("Gameplay") ) {
    			GameplayWindow = true;
    			InventoryWindow = false;
    			OptionWindow = false;
    		} 
    		ImGui::SameLine();
    		if ( ImGui::Button("Inventory") ) {
    			GameplayWindow = false;
    			InventoryWindow = true;
    			OptionWindow = false;
    		} 
    		ImGui::SameLine();
    		if ( ImGui::Button("Option") ) {
    			GameplayWindow = false;
    			InventoryWindow = false;
    			OptionWindow = true;
    		}
    		ImGui::EndMenuBar();
    	}

		if (GameplayWindow) {								// displays game messages

			ImGui::Text( "%s\n", message.c_str() );

			if ( ImGui::Button("Quit (Q)") ) {
				cleanup();
				glfwSetWindowShouldClose(m_window, GL_TRUE);
			}
		} else if (InventoryWindow) {					// list items in inventory
			for (auto item : inventory) {
				ImGui::Text( "%s\n", item.c_str() );
			}
		} else if (OptionWindow) {					// toggle options

			ImGui::Checkbox("Transparency", &transparency);
			ImGui::Checkbox("Collision Detection", &collisionDetection);
			ImGui::Checkbox("Texture Mapping", &textureMap);
			ImGui::Checkbox("Shadow Mapping", &shadowMap);
		}


		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
mat4 Final::updateShaderView(
		const ShaderProgram & shader,
		const SceneNode & node,
		const mat4 & modelMatrix
) {
	mat4 newModelMatrix;
	GLint location;
	shader.enable();
	{

		//-- Set ModelView matrix:
		location = shader.getUniformLocation("Model");
		newModelMatrix = modelMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(newModelMatrix));
		CHECK_GL_ERRORS;

	}
	shader.disable();
	return newModelMatrix;
}

mat4 Final::updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const mat4 & modelMatrix
) {

	mat4 newModelMatrix = updateShaderView(shader, node, modelMatrix);
	shader.enable();
	{
		if (shader.getProgramObject() == m_shader.getProgramObject()) {

			GLint location;
			location = m_shader.getUniformLocation("shadowMap");	// display/hide shadows
			glUniform1i( location, shadowMap ? 1 : 0 );

			location = m_shader.getUniformLocation("textureMap");	// display/hide textures
			glUniform1i( location, textureMap ? 1 : 0 );

			location = m_shader.getUniformLocation("transparency"); // node transparency
			glUniform1f( location, node.transparency );

			location = m_shader.getUniformLocation("allowTransparency"); // node transparency
			glUniform1i( location, transparency ? 1 : 0 );

			CHECK_GL_ERRORS;

			if (!do_picking) 
			{
				bool isTextureSet = true;
				GLuint chosenTexture;

				glActiveTexture(GL_TEXTURE0);				// bind texture to GL_TEXTURE0

				if ( node.getName() == "queenOfHeartsPainting" ) {
					chosenTexture = textureQueenOfHeartsPortrait;
				} else if ( node.getName() == "room" ) {
					chosenTexture = textureRoom;
				} else if ( node.getName() == "drinkMe" ) {
					chosenTexture = textureDrinkMe;
				} else if ( node.getName() == "door" ) {
					chosenTexture = textureDoor;
				} else if ( node.getName() == "painting1" ) {
					chosenTexture = texturePainting1;
				} else if ( node.getName() == "painting2" ) {
					chosenTexture = texturePainting2;
				} else if ( node.getName() == "painting3" ) {
					chosenTexture = texturePainting3;
				} else {
					isTextureSet = false;
				}

				// identifier to frragment shader for if object has a texture
				textureBufferID[0] = m_shader.getUniformLocation("isTextureSet");
				glUniform1i( textureBufferID[0], isTextureSet ? 1 : 0 );

				if (isTextureSet) {			// tell fs to render texture, else base colour
					glBindTexture(GL_TEXTURE_2D, chosenTexture);
					textureBufferID[0] = m_shader.getUniformLocation("textureSampler");
					glUniform1i(textureBufferID[0], 0);
					CHECK_GL_ERRORS;
				}

				glActiveTexture(GL_TEXTURE1); 				// bind shadows to GL_TEXTURE1
				glBindTexture(GL_TEXTURE_2D, m_depthTexture);
				textureBufferID[0] = m_shader.getUniformLocation("shadowMapSampler");
				glUniform1i(textureBufferID[0], 1);
			} // if not picking

			vec3 col_kd, col_ks;
			if (node.isSelected) { 
				col_kd = vec3(1.0, 1.0, 0.0 );
				col_ks = vec3(1.0, 1.0, 0.0);
			} else {
				col_ks = node.material.ks;
				col_kd = node.material.kd;
			}
			//-- Set Material values:

			if( do_picking ) {							// set false colours
				float r = float(node.m_nodeId&0xff) / 255.0f;
				float g = float((node.m_nodeId>>8)&0xff) / 255.0f;
				float b = float((node.m_nodeId>>16)&0xff) / 255.0f;

				location = shader.getUniformLocation("material.kd");
				glUniform3f( location, r, g, b );
				CHECK_GL_ERRORS;
			} else {
				location = shader.getUniformLocation("material.kd");
				glUniform3fv(location, 1, value_ptr(col_kd));
				CHECK_GL_ERRORS;
				location = shader.getUniformLocation("material.ks");
				glUniform3fv(location, 1, value_ptr(col_ks));
				CHECK_GL_ERRORS;
				location = shader.getUniformLocation("material.shininess");
				glUniform1f(location, node.material.shininess);
				CHECK_GL_ERRORS;
			}

		} // if m_shader

		else if (shader.getProgramObject() == m_depthShader.getProgramObject())
		{
			glEnableVertexAttribArray(m_depthPositionAttribLocation);	// access z coordinates
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

			glVertexAttribPointer(m_depthPositionAttribLocation, 
				3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glDisableVertexAttribArray(m_depthPositionAttribLocation);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	shader.disable();

	return newModelMatrix;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void Final::draw() {

	glEnable( GL_DEPTH_TEST );

	glDepthFunc( GL_LESS );

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	movement();										// adjust m_view for movement

	renderSceneGraph(*rootTranslation);

	glDisable( GL_DEPTH_TEST );

}

//----------------------------------------------------------------------------------------
void Final::movement() {

	// --------------- Jumps and Gravity ------------------
	vec3 newPosition;
	bool result(true);

	if ( jumpStart && (currJumpHeight == 0) ) jumpReset();	// reset jump parameters, disable multiple jumps

	eyeVec.y -= lastJumpHeight;						// decrement previous jump 

	currJumpHeight = (jumpV0 * jumpTime) + (0.5 * acceleration * (pow(jumpTime, 2)));

	if ( !jump ) {									// not performing a jump
		if (currJumpHeight > 0) currJumpHeight = 0;	// don't jump randomly
	}												// else it's a fall - let it happen

	jumpTime += 0.02;								// increment time

	newPosition = vec3(eyeVec.x, eyeVec.y + currJumpHeight, eyeVec.z);
	result = checkForPlayerCollision(newPosition);	// check for collisions
	if ( !result || !collisionDetection ) {		// no collisions, jump and update eyeVec
		eyeVec = newPosition;
		lastJumpHeight = currJumpHeight;
	} else {							// collision detected, no change in eyeVec
		eyeVec.y += lastJumpHeight;		// restore last jump height
		jump = false;
		lastJumpHeight = 0;
		currJumpHeight = 0;
		jumpTime = 0;
		if ((collisionDetection) &&				// update node we're standing on
			(collisionNode->m_name != lastYCollisionNode->m_name)) {
			lastYCollisionNode = collisionNode;
		}
	}

	// --------------- Directional Movement  ------------------

	vec3 changeInPosition(viewVec.x * adjustMovementSpeedZ, 0, 
		viewVec.z * adjustMovementSpeedZ);
	newPosition = eyeVec; 								// reset newPosition;
	// float currSlip;

	if ( moveUp ) {										// move eye vector forward
		newPosition = eyeVec + changeInPosition;
		result = checkForPlayerCollision(newPosition);
		// initializeVelocities();
		// currZSlipTime = 0;								// reset

	} else if ( moveDown ) {									// move eye vector back
		newPosition = eyeVec - changeInPosition;
		result = checkForPlayerCollision(newPosition);
		// initializeVelocities();
	// 	currZSlipTime = 0;								// reset

	// } else if (V0z != 0) {
	// 	zSlipTime = -V0z / lastYCollisionNode->frictionDeceleration;
	// 	if ( lastUp ) {
	// 		eyeVec.z -= lastZSlip;
	// 		currSlip = (V0z * currZSlipTime) + 
	// 			(0.5 * lastYCollisionNode->frictionDeceleration * (pow(zSlipTime, 2)));
	// 		newPosition = eyeVec + currSlip;
	// 	} else {
	// 		eyeVec.z += lastZSlip;
	// 		currSlip = (V0z * currZSlipTime) + 
	// 			(0.5 * lastYCollisionNode->frictionDeceleration * (pow(zSlipTime, 2)));
	// 		newPosition = eyeVec - currSlip;
	// 	}
	// 	result = checkForPlayerCollision(newPosition);
	// 	++currZSlipTime;
	}

	if ( !result || !collisionDetection ) {			// if there is no collision, update eyeVec
		eyeVec = newPosition;
	}
	result = true; // reset result;

	changeInPosition = vec3(viewVec.x * adjustMovementSpeedX, 0, 
		viewVec.z * adjustMovementSpeedX);

	changeInPosition = cross(changeInPosition, viewVec);

	newPosition = eyeVec;								// reset newPosition;
	
	if ( moveLeft ) {									// move eye vector left
		newPosition = eyeVec + changeInPosition;
		result = checkForPlayerCollision(newPosition);
		// initializeVelocities();

	} else if ( moveRight ) {									// move eye vector right
		newPosition = eyeVec - changeInPosition;
		result = checkForPlayerCollision(newPosition);
		// intializeVelocities();
	}

	if ( !result || !collisionDetection ) {
		eyeVec = newPosition;
	} 

	result = true; // reset result

	m_view = glm::lookAt(eyeVec, viewVec + eyeVec,		// update with new eye position
			vec3(0.0f, 1.0f, 0.0f) );
}

//----------------------------------------------------------------------------------------
void Final::jumpReset() {
	jumpTime = 0;
	jumpVf = 0;
	currMaxJumpHeight = maxJumpHeight;
	lastJumpHeight = 0;
	jumpV0 = sqrt( (pow(jumpVf, 2)) - (2 * acceleration * currMaxJumpHeight ));
	jumpStart = false;
	jump = true;
}

void Final::initializeVelocities() {

	if (timerZ == 0) {								// initialize V0z
		startPositionZ = eyeVec.z;
		++timerZ;
	} else if (timerZ < 60) {
		++timerZ;
	} else if (timerZ == 60) {
		endPositionZ = eyeVec.z;
		V0z = (endPositionZ - startPositionZ)/100;
		++timerZ;
	}

	if (timerX == 0) {								// initialize V0x
		startPositionX = eyeVec.x;
		++timerX;
	} else if (timerX < 60) {
		++timerX;
	} else if (timerX == 60) {
		endPositionX = eyeVec.x;
		V0x = (endPositionX - startPositionX)/100;
		++timerX;
	}
}

//----------------------------------------------------------------------------------------
// player's bounding box points
bool Final::checkForPlayerCollision(vec3 eye) {
	vec3 maxPoint(eye.x + (playerWidth/2), eye.y + 2.0, eye.z + (playerWidth/2));
	vec3 minPoint(eye.x - (playerWidth/2), eye.y - playerHeight+1.0, eye.z - (playerWidth/2));
	return checkForPointCollision(maxPoint, minPoint);
}

// check if neighbouring object's bounding boxes collide with player's
// evaluates for player's max and minimum point
bool Final::checkForPointCollision(vec3 maxCoord, vec3 minCoord) {
	
	for (auto &node : nodeVector) {

		if (node->m_nodeType != NodeType::GeometryNode) continue;  // skip non geometry nodes
		if (!node->visibility) continue; 			// visibility has been disabled
		GeometryNode *n = static_cast<GeometryNode *>(node);

		mat4 startTransform(1.0);	// matrix to transform player points from world to model view
		mat4 transformationInverse = glm::inverse(obtainTransformationMatrix(*n, startTransform));
		
		vec3 maxCoordTransformed = vec3(transformationInverse * vec4(maxCoord, 1.0));
		vec3 minCoordTransformed = vec3(transformationInverse * vec4(minCoord, 1.0));
		vec3 nodeMaxCoord = n->maxCoordinate;
		vec3 nodeMinCoord = n->minCoordinate;

		if (n->m_name == "room") {			// want to stay within room bounds
			if ( (
				(maxCoordTransformed.x > nodeMaxCoord.x) ||  
				(maxCoordTransformed.y > nodeMaxCoord.y) || 
				(maxCoordTransformed.z > nodeMaxCoord.z)
				)
				|| (
				(minCoordTransformed.x < nodeMinCoord.x ) || 
				(minCoordTransformed.y < nodeMinCoord.y ) || 
				(minCoordTransformed.z < nodeMinCoord.z )
				)  

			) {
				collisionNode = n;	// track which node collision occured with
				return true;
			}
		} else {					// don't want to enter objects
			if (checkForCollision(minCoordTransformed, maxCoordTransformed, 
					nodeMinCoord, nodeMaxCoord))
			{
				collisionNode = n; 	// track which node collision occured with
				return true; 
			}
		}
	}
	return false;	// no collision detected
}

// obtain transformation matrix to move from world to model coordinates for node n
mat4 Final::obtainTransformationMatrix(SceneNode &n, mat4 &m) {
	if (n.getName() == m_rootNode->getName()) {
		return n.get_transform() * m;
	} else {
		return n.get_transform() * obtainTransformationMatrix(*n.parent, m);
	}
}

// helper to check for collisions
bool Final::checkForCollision(
	vec3 minCoord, vec3 maxCoord, vec3 nodeMin, vec3 nodeMax) 
{
	if ( 
		(((minCoord.x <= nodeMax.x ) && (maxCoord.x >= nodeMin.x)) && 
			((minCoord.y <= nodeMax.y ) && (maxCoord.y >= nodeMin.y)) && 
			((minCoord.z <= nodeMax.z ) && (maxCoord.z >= nodeMin.z))) 
		|| 
		(((nodeMin.x <= maxCoord.x ) && (nodeMax.x >= minCoord.x)) && 
			((nodeMin.y <= maxCoord.y ) && (nodeMax.y >= minCoord.y)) && 
			((nodeMin.z <= maxCoord.z ) && (nodeMax.z >= minCoord.z)))

	) return true;
	return false;
}

// void Final::updateFriction(GeometryNode *n) {
// 	coefficientOfFriction = n->friction;
// }

//----------------------------------------------------------------------------------------
void Final::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);	// render to depth buffer
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 modelMatrixDepth(1.0f);
	renderNode(m_depthShader, root, modelMatrixDepth);	// first pass using depthShader - shadows

	glBindFramebuffer(GL_FRAMEBUFFER, 0);		// render to screen
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 modelMatrix(1.0f);
	renderNode(m_shader, root, modelMatrix);			// second pass with m_shader

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}
//----------------------------------------------------------------------------------------
// render different nodes
void Final::renderNode(
	const ShaderProgram & shader,
	const SceneNode & node,
	const glm::mat4 & modelMatrix) 
{

	// apply transformations to scene
	mat4 newModelMatrix = updateShaderView(shader, node, modelMatrix);
	
	// render children nodes based on newMatrixView
	for (const SceneNode *n : node.children) {
		if (n->m_nodeType == NodeType::GeometryNode) {
			const GeometryNode * geometryNode = static_cast<const GeometryNode *>(n);
			renderGeometryNode(shader, *geometryNode, newModelMatrix);
		} else {
			renderNode(shader, *n, newModelMatrix);
		}
	}
}
void Final::renderGeometryNode(
	const ShaderProgram & shader,
	const GeometryNode & node,
	const glm::mat4 & modelMatrix) 
{

	mat4 newModelMatrix = updateShaderUniforms(shader, node, modelMatrix);

	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = m_batchInfoMap[node.meshId];

	//-- Now render the mesh:
	shader.enable();

	if (shader.getProgramObject() == m_depthShader.getProgramObject()) 
	{
		glEnableVertexAttribArray(m_depthPositionAttribLocation);
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		glDisableVertexAttribArray(m_depthPositionAttribLocation);
	} else if (shader.getProgramObject() == m_shader.getProgramObject())
	{

		if (node.visibility) {		// don't render nodes that aren't visable
			glEnableVertexAttribArray(m_positionAttribLocation);
			glEnableVertexAttribArray(m_uvAttribLocation);
			glEnableVertexAttribArray(m_normalAttribLocation);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
			glDisableVertexAttribArray(m_positionAttribLocation);
			glDisableVertexAttribArray(m_uvAttribLocation);
			glDisableVertexAttribArray(m_normalAttribLocation);
		}
	}
	shader.disable();

	// render children nodes
	for (const SceneNode *n : node.children) {
		if (n->m_nodeType == NodeType::GeometryNode) {
			const GeometryNode * geometryNode = static_cast<const GeometryNode *>(n);
			renderGeometryNode(shader, *geometryNode, newModelMatrix);
		} else {
			renderNode(shader, *n, newModelMatrix);
		}
	}

}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void Final::cleanup()
{
	delete rootTranslation;
	delete rootRotation;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool Final::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------

/*
 * Helper for rotation - TAKEN FROM SAMPLE CODE ON PIAZZA
 * 
 * void vCalcRotVec(float fNewX, float fNewY, 
 *                  float fOldX, float fOldY,
 *                  float fDiameter,
 *                  float *fVecX, float *fVecY, float *fVecZ);
 *
 *    Calculates a rotation vector based on mouse motion over
 *    a virtual trackball.
 *
 *    The fNew and fOld mouse positions
 *    should be in 'trackball' space. That is, they have been
 *    transformed into a coordinate system centered at the middle
 *    of the trackball. fNew, fOld, and fDiameter must all be specified
 *    in the same units (pixels for example).
 *
 * Parameters: fNewX, fNewY - New mouse position in trackball space.
 *                            This is the second point along direction
 *                            of rotation.
 *             fOldX, fOldY - Old mouse position in trackball space.
 *                            This is the first point along direction
 *                            of rotation.
 *             fDiameter - Diameter of the trackball. This should
 *                         be specified in the same units as fNew and fOld.
 *                         (ie, usually pixels if fNew and fOld are transformed
 *                         mouse positions)
 *             fVec - The output rotation vector. The length of the vector
 *                    is proportional to the angle of rotation.
 *
 *******************************************************/
void vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ) {
   long  nXOrigin, nYOrigin;
   float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
         fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
         fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVecX    = fNewX * 2.0 / fDiameter;
   fNewVecY    = fNewY * 2.0 / fDiameter;
   fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVecZ < 0.0) {
      fLength = sqrt(1.0 - fNewVecZ);
      fNewVecZ  = 0.0;
      fNewVecX /= fLength;
      fNewVecY /= fLength;
   } else {
      fNewVecZ = sqrt(fNewVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVecX    = fOldX * 2.0 / fDiameter;
   fOldVecY    = fOldY * 2.0 / fDiameter;
   fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVecZ < 0.0) {
      fLength = sqrt(1.0 - fOldVecZ);
      fOldVecZ  = 0.0;
      fOldVecX /= fLength;
      fOldVecY /= fLength;
   } else {
      fOldVecZ = sqrt(fOldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    * 
    * fOldVec x fNewVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
   *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
   *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}

/*******************************************************
 * HELPER CODE TAKEN FROM PIAZA
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat)
 *    
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *    
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, mat4 &mNewMat) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        return;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    mNewMat[0][0] = (float)(dCosAlpha + fNewVecX*fNewVecX*dT);
    mNewMat[0][1] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    mNewMat[0][2] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    mNewMat[0][3] = 0;

    mNewMat[1][0] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    mNewMat[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
    mNewMat[1][2] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    mNewMat[1][3] = 0;

    mNewMat[2][0] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    mNewMat[2][1] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    mNewMat[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    mNewMat[2][3] = 0;

    mNewMat[3][0] = 0;
    mNewMat[3][1] = 0;
    mNewMat[3][2] = 0;
    mNewMat[3][3] = 1;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool Final::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	xPrev = xCurr;							// copy over mouse positions
	yPrev = yCurr;
	xCurr = xPos;
	yCurr = yPos;

	double xChange, yChange, alpha;
	alpha = 0.2f;
	xChange = (xCurr - xPrev) * 0.02;			// determine how much to transform
	yChange = -(yCurr - yPrev) * 0.02;

	if ( rightPressed ) {				// change camera view

		vec3 b = cross(viewVec, upVec);					// obtain coss of view and up
		viewVec += vec3(b.x * xChange, 0, b.z * xChange); 	// update viewVec in x direction
		viewVec = normalize(viewVec);

		viewVec.y += yChange;						// update viewVec in y direction
	}

	xPrev = xCurr;						// stops transformation if mouse stops moving
	yPrev = yCurr;

	eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool Final::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	if (actions == GLFW_PRESS) {						// record which mouse button pressed
		xMouseStart = xCurr;
		yMouseStart = yCurr;
		eventHandled = true;

		if ( button == GLFW_MOUSE_BUTTON_LEFT ) {		// interact with environment (picking)
			leftPressed = true;
			performPicking();
		} 
		if ( button == GLFW_MOUSE_BUTTON_RIGHT ) {		// rotation
			rightPressed = true;
		} 
		if ( button == GLFW_MOUSE_BUTTON_MIDDLE ) {		// translation(z) or joint rotation
			middlePressed = true;
		}

	} else if (actions == GLFW_RELEASE) {			// detect mouse release
		xMouseStart = xCurr;
		yMouseStart = yCurr;
		if ( button == GLFW_MOUSE_BUTTON_LEFT ) {
			leftPressed = false;						// reset mouse depressions
		}
		if ( button == GLFW_MOUSE_BUTTON_MIDDLE ) {
			middlePressed = false;
		}
		if ( button == GLFW_MOUSE_BUTTON_RIGHT ) {
			rightPressed = false;
		}

		eventHandled = true;
		return eventHandled;

	}
	return eventHandled;
}
//----------------------------------------------------------------------------------------
/*
 * Handles picking - CODE TAKEN AND MODIFIED FROM PROVIDED SAMPLE PICKING CODE 
 */
void Final::performPicking() {
	double xpos, ypos;
	xpos = xCurr;
	ypos = yCurr;

	do_picking = true;

	uploadCommonSceneUniforms();
	glClearColor(1.0, 1.0, 1.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor(0.35, 0.35, 0.35, 1.0);

	draw();
	
	CHECK_GL_ERRORS;

	// Ugly -- FB coordinates might be different than Window coordinates
	// (e.g., on a retina display).  Must compensate.
	xpos *= double(m_framebufferWidth) / double(m_windowWidth);
	// WTF, don't know why I have to measure y relative to the bottom of
	// the window in this case.
	ypos = m_windowHeight - ypos;
	ypos *= double(m_framebufferHeight) / double(m_windowHeight);

	GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
	// A bit ugly -- don't want to swap the just-drawn false colours
	// to the screen, so read from the back buffer.
	glReadBuffer( GL_BACK );
	// Actually read the pixel at the mouse location.
	glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	CHECK_GL_ERRORS;

	// Reassemble the object ID.
	unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
	if ( what < nodeVector.size() && 
		(nodeVector[what]->interactive) ) {				// valid node and is interactive

		interactions.invokeAction(nodeVector[what]);
	}

	do_picking = false;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool Final::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool Final::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool Final::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {

		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		} else if ( key == GLFW_KEY_Q ) {					// close program
			cleanup();
			glfwSetWindowShouldClose( m_window, GL_TRUE );
			eventHandled = true;
		} else if ( key == GLFW_KEY_W ) {					// move up
			moveUp = true;
			eventHandled = true;
		} else if ( key == GLFW_KEY_S ) {					// move down
			moveDown = true;
			eventHandled = true;
		} else if ( key == GLFW_KEY_A ) {					// move left
			moveLeft = true;
			eventHandled = true;
		} else if ( key == GLFW_KEY_D ) {					// move right
			moveRight = true;
			eventHandled = true;
		} else if ( key == GLFW_KEY_SPACE) {				// jump
			jumpStart = true;
			eventHandled = true;
		}
	} else if( action == GLFW_RELEASE ) {

		if ( key == GLFW_KEY_W ) {					// move up
			moveUp = false;
			eventHandled = true;
		} else if ( key == GLFW_KEY_S ) {					// move down
			moveDown = false;
			eventHandled = true;
		} else if ( key == GLFW_KEY_A ) {					// move left
			moveLeft = false;
			eventHandled = true;
		} else if ( key == GLFW_KEY_D ) {					// move right
			moveRight = false;
			eventHandled = true;
		}
	}

	return eventHandled;
}

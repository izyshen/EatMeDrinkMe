// Winter 2019

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include "interactions.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

typedef glm::vec3 colour3; // red, green, blue


class Final : public CS488Window {
public:
	Final(const std::string & luaSceneFile);
	virtual ~Final();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadBoundingBoxInfo(MeshConsolidator & meshConsolidator);
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();
	void initLightMatrices();
	void initializeFBO();

	void initPerspectiveMatrix();
	void initTransformationMatrices();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);

	glm::mat4 m_perspective;
	glm::mat4 m_view;
	glm::mat4 m_lightViewMatrix;
	glm::mat4 m_lightModelMatrix;
	glm::mat4 m_depthMVP;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_vertexUV;
	GLuint m_vbo_vertexisTextured;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	GLint m_uvAttribLocation;
	GLint m_transparentAttribLocation;
	ShaderProgram m_shader;


	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	SceneNode *m_rootNode, *rootRotation, *rootTranslation;

	// assign indices and create vector of Nodes
	std::vector<SceneNode *> nodeVector;
	void createNodeVector(SceneNode *node, int &index);			// gives an index(ID) to nodes

	// render different note types
	void renderNode(
		const ShaderProgram & shader,
		const SceneNode & node,
		const glm::mat4 & modelMatrix);
	void renderGeometryNode(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & modelMatrix);
	glm::mat4 updateShaderView(
		const ShaderProgram & shader,
		const SceneNode & node,
		const mat4 & modelMatrix);
	glm::mat4 updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const mat4 & modelMatrix);

	// camera vectors
	glm::vec3 eyeVec;
	glm::vec3 viewVec;
	glm::vec3 upVec;

	// character interaction
	float playerWidth;
	float playerHeight;
	bool moveUp, moveDown, 
		moveLeft, moveRight, 
		jumpStart, jump, jumpDown;
	float adjustMovementSpeedZ, adjustMovementSpeedX;
	float maxJumpHeight, currMaxJumpHeight, currJumpHeight,
		lastJumpHeight, jumpV0, jumpVf,
		jumpTime;
	void movement();
	void jumpReset();
	std::vector<std::string> inventory;

	// mouse details
	double xMouseStart, yMouseStart, xPrev, xCurr, yPrev, yCurr;
	bool leftPressed, rightPressed, middlePressed;

	// arc circle radius
	double radius, xCenterCoord, yCenterCoord;

	// Menu options
	bool GameplayWindow, InventoryWindow, OptionWindow;
	std::string message;
	bool shadowMap, textureMap, collisionDetection, transparency;

	// handle picking
	bool do_picking;
	int numNodes;
	vec3 colourPicked;
	bool toggleSelected(int desiredIndex, int &currIndex, SceneNode &node);
	void performPicking();

	// textures
	void renderTexturefiles();
	GLuint textureBufferID[1];
	GLuint textureQueenOfHeartsPortrait;
	GLuint texturePainting1;
	GLuint texturePainting2;
	GLuint texturePainting3;
	GLuint textureRoom;
	GLuint textureDrinkMe;
	GLuint textureDoor;

	// shadow mapping
	GLuint m_fbo;
	GLuint m_depthTexture;
	GLuint m_lightAttribLocation;
	GLuint m_depthMVPLocation;
	ShaderProgram m_depthShader;
	GLint m_depthPositionAttribLocation;

	// Class to help handle specific character-scene interactions
	friend class Interactions;
	Interactions interactions;
	bool popup;

	// gravity, friction
	float acceleration;						// gravity
	GeometryNode *lastYCollisionNode;		// detects which object we are standing on
	float zSlipTime, xSlipTime;				// how much extra distance to add
	float currZSlipTime, currXSlipTime;
	bool lastUp, lastLeft;					// whether last moved direction is L/R, U/D
	float lastXSlip, lastZSlip;				// previous distance slipped

	float timerZ, startPositionZ, endPositionZ; // used to obtain initial velocities
	float timerX, startPositionX, endPositionX; 	// for slipping
	float V0z, V0x; 
	void initializeVelocities();

	// collision detection
	bool checkForCollision(
		glm::vec3 minCoord, 
		glm::vec3 maxCoord, 
		glm::vec3 nodeMin, 
		glm::vec3 nodeMax);
	bool checkForPlayerCollision(glm::vec3 eye);
	bool checkForPointCollision(
		glm::vec3 minCoord, 
		glm::vec3 maxCoord);
	glm::mat4 obtainTransformationMatrix(SceneNode &n, glm::mat4 &m);
	GeometryNode *collisionNode;


};

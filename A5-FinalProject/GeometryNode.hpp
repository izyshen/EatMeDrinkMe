// Winter 2019

#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;

	glm::vec3 maxCoordinate;
	glm::vec3 minCoordinate;

	float friction;
	float frictionDeceleration;

	float transparency;

	void setFriction(std::string name);
	void setTransparency(std::string name);

};

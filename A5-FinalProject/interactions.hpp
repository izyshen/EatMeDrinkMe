#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "SceneNode.hpp"

class Interactions {

	glm::vec3 &eyeVec;

	float &adjustMovementSpeedX,
		&adjustMovementSpeedZ;

	float &maxJumpHeight;

	bool &popup;

	float &playerWidth, &playerHeight;

	std::vector<std::string> &inventory;

	std::vector<SceneNode *> &nodeVector;

	std::string &message;


public:
	Interactions(glm::vec3 &eyeVec, 
		float &adjustMovementSpeedX, 
		float &adjustMovementSpeedZ,
		float &maxJumpHeight,
		bool &popup,
		float &playerWidth,
		float &playerHeight,
		std::vector<std::string> &inventory,
		std::vector<SceneNode *> &nodeVector,
		std::string &message
		);

	void invokeAction( SceneNode *n );

	void enableStairVisibility();

	bool checkForItem(std::string item);

};
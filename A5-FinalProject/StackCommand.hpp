#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "JointNode.hpp"
#include <iostream>
using namespace std;


enum class RotationType {
	x, y, z
};

class StackCommand {
  public:
  	StackCommand(JointNode *node) : node(node) {
  		prevTransformation = node->get_transform();
  	};
	~StackCommand() { };

	void undo() { node->set_transform(prevTransformation); };
	void redo() { node->set_transform(currTransformation); };

	void saveTransform() { currTransformation = node->get_transform(); };

	float angle;
	RotationType axis;

	// store transforms to prevent recalculation
	glm::mat4 prevTransformation, currTransformation;	

	// store joint for which to apply transformations
	JointNode *node;

};

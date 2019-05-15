// Winter 2019

#include "GeometryNode.hpp"
#include <string>

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;

	// apply visibility values
	if ( name.substr(0, 5) == "stair" ) {
		visibility = false;
	}

	setTransparency(name);

	setFriction(name);
}

//---------------------------------------------------------------------------------------
// apply friction values
void GeometryNode::setFriction(std::string name) {

	float gravity = 9.8;
	if ( name == "room" ) {
		friction = 0.3;
		frictionDeceleration = -friction * gravity;
	} else {
		friction = 1.0;
		frictionDeceleration = -friction * gravity;
	}
}

//---------------------------------------------------------------------------------------
// apply transparency values
void GeometryNode::setTransparency(std::string name) {
	if ( name.substr(0, 4) == "case" ) {
		transparency = 0.3;
	} else {
		transparency = 1.0;
	}
}

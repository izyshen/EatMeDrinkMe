// Winter 2019

#include "JointNode.hpp"
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
	m_joint_x.curr = init;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	m_joint_y.curr = init;
}

//---------------------------------------------------------------------------------------
void JointNode::adjustAngleTransformY(double angle) {

	double *min = &m_joint_y.min;
	double *cur = &m_joint_y.curr;
	double *max = &m_joint_y.max;
	double angleChange;

	if (*cur == *min && angle < 0) { return; } 		// angle is already at min
	if (*cur == *max && angle > 0) { return; }		// angle already at max
	if ( (*cur + angle) > *max ) {		// new angle greater than max
		angleChange = *max - *cur;
		*cur = *max;
	} else if ( (*cur + angle) < *min ) {	// new angle less than min
		angleChange = *min - *cur;
		*cur = *min;
	} else {
		angleChange = angle;
		*cur += angle;
	}

	rotate('x', (angleChange / 0.1f));

}

void JointNode::adjustAngleTransformX(double angle) {

	double *min = &m_joint_x.min;
	double *cur = &m_joint_x.curr;
	double *max = &m_joint_x.max;
	double angleChange;

	if (*cur == *min && angle < 0) { return; } 		// angle is already at min
	if (*cur == *max && angle > 0) { return; }		// angle already at max
	if ( (*cur + angle) > *max ) {		// new angle greater than max
		angleChange = *max - *cur;
		*cur = *max;
	} else if ( (*cur + angle) < *min ) {	// new angle less than min
		angleChange = *min - *cur;
		*cur = *min;
	} else {
		angleChange = angle;
		*cur += angle;
	}

	rotate('y', (angleChange / 0.1f));

}
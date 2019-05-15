// Winter 2019

#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, curr, max;
	};

	void adjustAngleTransformX(double angle);		// boundary check angles
	void adjustAngleTransformY(double angle);		// boundary check angles

	JointRange m_joint_x, m_joint_y;
};

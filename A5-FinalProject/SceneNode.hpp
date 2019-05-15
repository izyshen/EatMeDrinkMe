// Winter 2019

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cs488-framework/ShaderProgram.hpp"

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
}; 

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // interactions
	bool isSelected;
    bool interactive;
    bool visibility;
    
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    // relations
    std::list<SceneNode*> children;
    SceneNode *parent;

    // identifiers
	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    std::string getName() const;




};

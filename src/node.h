#pragma once

#include <GLM/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <vector>
#include <string>

#include "JointTypes.h"
#include "shader.h"


class Node
{

protected:

	float m_pScale;
	float m_pRotX, m_pRotY, m_pRotZ; // local rotation angles

	glm::mat4 m_pLocalRotation;
	glm::mat4 m_pLocalTransformMatrix;
	glm::mat4 m_pLocalToWorldMatrix;
	glm::mat4 baseTranslation;

	// Forward Kinematics
	// HTR animation
	unsigned int m_pCurrentFrame;

	int frameDelay = 1;
	int frame = 0;

	//Loader sphere; debug sphere
	GLuint VBO;
	GLuint VAO;
private:
	//recursive function
	virtual void update(float dt);
	//the update function our updates call
	void updating(float dt, float overRide = 0);

public:
	glm::vec3 m_pLocalPosition;

	void setFrame(int);
	int getFrame();

	//Node(Node);
	Node();
	Node(std::string _name, glm::vec3 position);
	~Node();
	void init();

	void setPosition(glm::vec3 newPosition);
	void setRotationAngleX(float newAngle);
	void setRotationAngleY(float newAngle);
	void setRotationAngleZ(float newAngle);
	void setScale(float newScale);

	glm::mat4 getLocalToWorldMatrix();

	
	void updateTop(float dt);
	void updateBot(float dt, float overRide, bool finalAni = false);

	virtual void update();
	void Node::createBase(std::vector<glm::mat4> &temp, std::vector<glm::mat4> &, int &count);


	// gets the top half of the matricies joints
	void Node::getMatrixStackT(std::vector<glm::mat4>&, std::vector<glm::mat4>&, int&);

	// gets the bottom half of the matricies joints
	void Node::getMatrixStackB(std::vector<glm::mat4>&, std::vector<glm::mat4>&, int&);

	//used to get the rest of the matrix after the special half and bot function
	void Node::getMatrixStack(std::vector<glm::mat4>&, std::vector<glm::mat4>&, int&);


	virtual void drawSkeleton(glm::mat4, Shader);
	//virtual void draw();

	// Animation from HTR
	JointDescriptor* jointAnimation;

	Node* m_pParent;
	std::vector<Node*> m_pChildren;


	// Forward Kinematics
	// Pass in null to make game object a root node
	void setParent(Node* newParent);
	void addChild(Node* newChild);
	void removeChild(Node* rip);
	glm::vec3 getWorldPosition();
	glm::mat4 getWorldRotation();
	bool isRoot();

	// Other Properties
	std::string name;
	glm::vec4 colour;
	// Mesh* ...
	// Material* ...
};


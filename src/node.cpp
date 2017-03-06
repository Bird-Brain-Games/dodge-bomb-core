#include "Node.h"
#include <iostream>

const float degToRad = 3.14159f / 180.0f;

Node::Node()
	: m_pScale(1.0f),
	colour(glm::vec4(1.0)),
	m_pCurrentFrame(0),
	jointAnimation(nullptr)
{


}

Node::~Node() {}

void Node::setFrame(int _pos)
{
	if (_pos < jointAnimation->numFrames - 1 && _pos >= 0)
		m_pCurrentFrame = _pos;
	else
		std::cout << "Error: frame limit is  " << frame << " given value is greaten then this limit";
}

int Node::getFrame()
{
	return m_pCurrentFrame;
}

void Node::init()
{

}

void Node::setPosition(glm::vec3 newPosition)
{
	m_pLocalPosition = newPosition;
}

void Node::setRotationAngleX(float newAngle)
{
	m_pRotX = newAngle;
}

void Node::setRotationAngleY(float newAngle)
{
	m_pRotY = newAngle;
}

void Node::setRotationAngleZ(float newAngle)
{
	m_pRotZ = newAngle;
}

void Node::setScale(float newScale)
{
	m_pScale = newScale;
}

glm::mat4 Node::getLocalToWorldMatrix()
{
	return m_pLocalToWorldMatrix;
}

void Node::updateTop(float dt)
{
	//effeciancy only call this update in top or bot. need to change a couple of things first though
	updating(dt);
	m_pChildren[0]->update(dt);
}

void Node::updateBot(float dt, float overRide)
{
	updating(dt, overRide);
	m_pChildren[1]->update(dt);
}

void Node::update(float dt)
{
	updating(dt);
	for (int i = 0; i < m_pChildren.size(); i++)
		m_pChildren[i]->update(dt);
}

void Node::updating(float dt, float overRide)
{
	frame++;


	// If there is no animation for this joint, create the transform matrix as usual
	if (jointAnimation == nullptr)
	{
		// Create rotation matrix
		glm::mat4 rx = glm::rotate(glm::radians(m_pRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 ry = glm::rotate(glm::radians(m_pRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rz = glm::rotate(glm::radians(m_pRotZ), glm::vec3(0.0f, 0.0f, 1.0f));

		// Note: pay attention to rotation order, ZYX is not the same as XYZ
		// Order is specified by an HTR file when you export it
		m_pLocalRotation = rz * ry * rx;

	}
	else // Transform comes from HTR
	{

		m_pLocalRotation =
			glm::mat4_cast(jointAnimation->jointBaseRotation * jointAnimation->jointRotations[m_pCurrentFrame]);
		if (overRide != 0)
		{
			glm::mat4 rx = glm::rotate(overRide, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 ry = glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 rz = glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			m_pLocalRotation *= (rz * ry * rx);
		}

		m_pLocalPosition = jointAnimation->jointBasePosition + jointAnimation->jointPositions[m_pCurrentFrame];
		jointAnimation->jointScales[m_pCurrentFrame];

		// Increment frame (note: you could do this based on dt)
		m_pCurrentFrame++;

		if (m_pCurrentFrame >= jointAnimation->numFrames - 1)
			m_pCurrentFrame = 0;

	}

	// Create translation matrix
	glm::mat4 tran = glm::translate(m_pLocalPosition);

	// Create scale matrix
	glm::mat4 scal = glm::scale(glm::vec3(m_pScale, m_pScale, m_pScale));

	// Combine all above transforms into a single matrix
	// This is the local transformation matrix, ie. where is this game object relative to it's parent
	// If a game object has no parent (it is a root node) then its local transform is also it's global transform
	// If a game object has a transform, then we must apply the parent's transform
	m_pLocalTransformMatrix = tran * m_pLocalRotation * scal;

	if (m_pParent)
		m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix() * m_pLocalTransformMatrix;
	else
		m_pLocalToWorldMatrix = m_pLocalTransformMatrix;
}

void Node::update()
{

	// Create translation matrix
	glm::mat4 tran = glm::translate(m_pLocalPosition);

	// Create scale matrix
	glm::mat4 scal = glm::scale(glm::vec3(m_pScale, m_pScale, m_pScale));

	// Combine all above transforms into a single matrix
	// This is the local transformation matrix, ie. where is this game object relative to it's parent
	// If a game object has no parent (it is a root node) then its local transform is also it's global transform
	// If a game object has a transform, then we must apply the parent's transform
	m_pLocalTransformMatrix = tran * m_pLocalRotation * scal;

	if (m_pParent != this)
		m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix() * m_pLocalTransformMatrix;
	else
		m_pLocalToWorldMatrix = m_pLocalTransformMatrix;

	// Update children
	for (int i = 0; i < m_pChildren.size(); i++)
		m_pChildren[i]->update();

}

void Node::createBase(std::vector<glm::mat4> &baseCase, std::vector<glm::mat4> &matrix, int &count)
{
	//if (frameDelay == frame)
	//{
	frame = 0;
	// Create 4x4 transformation matrix

	// If there is no animation for this joint, create the transform matrix as usual
	if (jointAnimation == nullptr)
	{
		// Create rotation matrix
		glm::mat4 rx = glm::rotate(glm::radians(m_pRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 ry = glm::rotate(glm::radians(m_pRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rz = glm::rotate(glm::radians(m_pRotZ), glm::vec3(0.0f, 0.0f, 1.0f));

		// Note: pay attention to rotation order, ZYX is not the same as XYZ
		// Order is specified by an HTR file when you export it
		m_pLocalRotation = rz * ry * rx;

	}
	else // Transform comes from HTR
	{
		int size = jointAnimation->jointRotations.size();
		m_pLocalRotation =
			glm::mat4_cast(jointAnimation->jointBaseRotation * jointAnimation->jointRotations[size - 1]);

		m_pLocalPosition = jointAnimation->jointBasePosition + jointAnimation->jointPositions[size - 1];

		jointAnimation->jointScales[size - 1];

		count++;
	}

	// Create translation matrix
	glm::mat4 tran = glm::translate(m_pLocalPosition);

	// Create scale matrix
	glm::mat4 scal = glm::scale(glm::vec3(m_pScale, m_pScale, m_pScale));

	// Combine all above transforms into a single matrix
	// This is the local transformation matrix, ie. where is this game object relative to it's parent
	// If a game object has no parent (it is a root node) then its local transform is also it's global transform
	// If a game object has a transform, then we must apply the parent's transform
	m_pLocalTransformMatrix = tran * m_pLocalRotation * scal;

	if (m_pParent)
		m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix() * m_pLocalTransformMatrix;
	else
		m_pLocalToWorldMatrix = m_pLocalTransformMatrix;

	if (count >= 0)
	{
		matrix.push_back(glm::mat4(0));
		baseCase.push_back(glm::inverse(m_pLocalToWorldMatrix));
	}

	// Update children
	for (int i = 0; i < m_pChildren.size(); i++)
		m_pChildren[i]->createBase(baseCase, matrix, count);

}



//updates from count to max
void Node::getMatrixStack(std::vector<glm::mat4> &temp, std::vector<glm::mat4> &origin, int &count)
{
	if (jointAnimation != NULL)
	{
		temp[count] = m_pLocalToWorldMatrix * origin[count];
		count++;
	}
	// Update children
	for (int i = 0; i < m_pChildren.size(); i++)
	{
		m_pChildren[i]->getMatrixStack(temp, origin, count);
	}
}

void Node::getMatrixStackT(std::vector<glm::mat4> &temp, std::vector<glm::mat4> &origin, int &count)
{
	if (jointAnimation != NULL)
	{
		temp[count] = m_pLocalToWorldMatrix * origin[count];
		count++;
	}
	// Update children
	m_pChildren[0]->getMatrixStack(temp, origin, count);
}

void Node::getMatrixStackB(std::vector<glm::mat4> &temp, std::vector<glm::mat4> &origin, int &count)
{
	// Update children
	m_pChildren[1]->getMatrixStack(temp, origin, count);
}

void Node::drawSkeleton(glm::mat4 mvp, Shader shader)
{
	//glBindVertexArray(VAO);
	//glm::mat4 mvp2 = mvp * m_pLocalToWorldMatrix;
	//int imvp2 = glGetUniformLocation(shader.getProgramID(), "mvp");
	//glUniformMatrix4fv(imvp2, 1, GL_FALSE, glm::value_ptr(mvp2));
	//glDrawArrays(GL_TRIANGLES, 0, sphere.getVertices().size());
	//// Draw children
	//for (int i = 0; i < m_pChildren.size(); ++i)
	//	m_pChildren[i]->drawSkeleton(mvp, shader);
}

void Node::setParent(Node* newParent)
{
	m_pParent = newParent;
}

void Node::addChild(Node* newChild)
{
	if (newChild)
	{
		m_pChildren.push_back(newChild);
		newChild->setParent(this); // tell new child that this game object is its parent
	}
}

void Node::removeChild(Node* rip)
{
	for (int i = 0; i < m_pChildren.size(); ++i)
	{
		if (m_pChildren[i] == rip) // compare memory locations (pointers)
		{
			std::cout << "Removing child: " + rip->name << " from object: " << this->name;
			m_pChildren.erase(m_pChildren.begin() + i);
		}
	}
}

glm::vec3 Node::getWorldPosition()
{
	if (m_pParent)
		return m_pParent->getLocalToWorldMatrix() * glm::vec4(m_pLocalPosition, 1.0f);
	else
		return m_pLocalPosition;
}

glm::mat4 Node::getWorldRotation()
{
	if (m_pParent)
		return m_pParent->getWorldRotation() * m_pLocalRotation;
	else
		return m_pLocalRotation;
}

bool Node::isRoot()
{
	if (m_pParent)
		return false;
	else
		return true;
}


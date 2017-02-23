#include "GameObject.h"
#include <iostream>

GameObject::GameObject(
	glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	std::unique_ptr<RigidBody> _rb)

	:m_pLocalPosition(position),
	m_pParent(nullptr),
	m_pRotX(0.0f), m_pRotY(0.0f), m_pRotZ(0.0f),
	mesh(_mesh),
	material(_material),
	texture(_texture)
{
	if (_rb != nullptr)
		rigidBody = std::move(_rb);
}

GameObject::~GameObject() {}

void GameObject::setPosition(glm::vec3 newPosition)
{
	m_pLocalPosition = newPosition;
}

void GameObject::setRotationAngleX(float newAngle)
{
	m_pRotX = newAngle;
}

void GameObject::setRotationAngleY(float newAngle)
{
	m_pRotY = newAngle;
}

void GameObject::setRotationAngleZ(float newAngle)
{
	m_pRotZ = newAngle;
}

glm::mat4 GameObject::getLocalToWorldMatrix()
{
	return m_pLocalToWorldMatrix;
}

void GameObject::update(float dt)
{
	// Create 4x4 transformation matrix

	// Create rotation matrix
	glm::mat4 rx = glm::rotate(glm::radians(m_pRotX), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ry = glm::rotate(glm::radians(m_pRotY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rz = glm::rotate(glm::radians(m_pRotZ), glm::vec3(0.0f, 0.0f, 1.0f));

	// Note: pay attention to rotation order, ZYX is not the same as XYZ
	m_pLocalRotation = rz * ry * rx;

	// Create translation matrix
	glm::mat4 tran = glm::translate(m_pLocalPosition);
	
	// Combine all above transforms into a single matrix
	// This is the local transformation matrix, ie. where is this game object relative to it's parent
	// If a game object has no parent (it is a root node) then its local transform is also it's global transform
	// If a game object has a parent, then we must apply the parent's transform
	m_pLocalTransformMatrix = tran * m_pLocalRotation;

	if (m_pParent)
		m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix() * m_pLocalTransformMatrix;
	else
		m_pLocalToWorldMatrix = m_pLocalTransformMatrix;

	// Update children
	for (int i = 0; i < m_pChildren.size(); i++)
		m_pChildren[i]->update(dt);
}

void GameObject::draw(Camera &camera)
{
	material->shader->bind();
	material->mat4Uniforms["u_mvp"] = camera.getViewProj() * m_pLocalToWorldMatrix;
	material->mat4Uniforms["u_mv"] = camera.getView() * m_pLocalToWorldMatrix;

	// Bind the texture
	if (texture != nullptr)
	{
		texture->bind(GL_TEXTURE31, GL_TEXTURE30);
	}

	material->sendUniforms();

	mesh->draw(material->shader);

	// Unbind the texture
	if (texture != nullptr)
	{
		texture->unbind(GL_TEXTURE31, GL_TEXTURE30);
	}

	// Draw children
	for (int i = 0; i < m_pChildren.size(); ++i)
		m_pChildren[i]->draw(camera);
}

void GameObject::setParent(GameObject* newParent)
{
	m_pParent = newParent;
}

void GameObject::addChild(GameObject* newChild)
{
	if (newChild)
	{
		m_pChildren.push_back(newChild);
		newChild->setParent(this); // tell new child that this game object is its parent
	}
}

void GameObject::removeChild(GameObject* rip)
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

glm::vec3 GameObject::getWorldPosition()
{
	if (m_pParent)
		return m_pParent->getLocalToWorldMatrix() * glm::vec4(m_pLocalPosition, 1.0f);
	else
		return m_pLocalPosition;
}

glm::mat4 GameObject::getWorldRotation()
{
	if (m_pParent)
		return m_pParent->getWorldRotation() * m_pLocalRotation;
	else
		return m_pLocalRotation;
}

bool GameObject::isRoot()
{
	if (m_pParent)
		return false;
	else
		return true;
}


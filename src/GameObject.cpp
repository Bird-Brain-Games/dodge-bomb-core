#include "GameObject.h"
#include <iostream>

GameObject::GameObject()
	: m_pLocalPosition(glm::vec3(0.0f)),
	m_pParent(nullptr),
	m_pRotX(0.0f), m_pRotY(0.0f), m_pRotZ(0.0f),
	mesh(nullptr),
	material(nullptr),
	texture(nullptr),
	rigidBody(nullptr)
{

}

GameObject::GameObject(
	glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture)

	:m_pLocalPosition(position),
	m_pParent(nullptr),
	m_pRotX(0.0f), m_pRotY(0.0f), m_pRotZ(0.0f),
	mesh(_mesh),
	material(_material),
	texture(_texture),
	rigidBody(nullptr)
{

}

GameObject::GameObject(GameObject& other)
	: m_pLocalPosition(other.m_pLocalPosition),
	m_pParent(nullptr),
	m_pRotX(other.m_pRotX), m_pRotY(other.m_pRotY), m_pRotZ(other.m_pRotZ),
	mesh(other.mesh),
	material(other.material),
	texture(other.texture),
	rigidBody(nullptr)
{
	rigidBody = std::make_unique<RigidBody>(*other.rigidBody);
	rigidBody->load(other.rigidBody->getFileName(), 
		(btCollisionObject::CollisionFlags)other.rigidBody->getBody()->getCollisionFlags());
}

GameObject::~GameObject() 
{

}

void GameObject::attachRigidBody(std::unique_ptr<RigidBody> &_rb)
{
	if (_rb != nullptr)
	{
		rigidBody = std::move(_rb);
		updateLocalTransform();
		m_pLocalToWorldMatrix = m_pLocalTransformMatrix;
		rigidBody->setWorldTransform(m_pLocalToWorldMatrix);
	}
}

void GameObject::setLocalTransformToBody()
{
	m_pLocalPosition = m_pLocalToWorldMatrix[3];
	// ROTATION NEEDED TO BE ADDED
	//m_pRotX = m_pLocalToWorldMatrix[]
}

void GameObject::updateLocalTransform()
{
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
}

void GameObject::setPosition(glm::vec3 newPosition)
{
	m_pLocalPosition = newPosition;
	needsUpdating = true;
}

void GameObject::setRotationAngleX(float newAngle)
{
	m_pRotX = newAngle;
	needsUpdating = true;
}

void GameObject::setRotationAngleY(float newAngle)
{
	m_pRotY = newAngle;
	needsUpdating = true;
}

void GameObject::setRotationAngleZ(float newAngle)
{
	m_pRotZ = newAngle;
	needsUpdating = true;
}

glm::mat4 GameObject::getLocalToWorldMatrix()
{
	return m_pLocalToWorldMatrix;
}

void GameObject::update(float dt)
{
	// Create 4x4 transformation matrix
	if (!hasRigidBody() || needsUpdating)
	{
		updateLocalTransform();
	}
	
	if (m_pParent)
		m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix() * m_pLocalTransformMatrix;
	else
	{
		// Root node
		m_pLocalToWorldMatrix = m_pLocalTransformMatrix;

		if (hasRigidBody())
		{
			// If the gameobject has updated
			// set the world transform of the body to the gameobject.
			if (needsUpdating)
			{
				rigidBody->setWorldTransform(m_pLocalToWorldMatrix);
			}

			// If the gameobject has updated, set the world transform
			if (!rigidBody->getBody()->isStaticOrKinematicObject())
			{
				m_pLocalToWorldMatrix = rigidBody->getWorldTransform();
				// update local variables to have the properties
				setLocalTransformToBody();
			}
		}
	}

	// Update children
	for (int i = 0; i < m_pChildren.size(); i++)
		m_pChildren[i]->update(dt);

	needsUpdating = false;
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

void GameObject::checkCollisionWith(std::shared_ptr<GameObject> other)
{

}


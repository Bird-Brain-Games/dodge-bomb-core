#pragma once

#include <GLM/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <vector>
#include <string>
#include <utility>
#include <memory>
#include <map>

#include "camera.h"
#include "Texture.h"
#include "Material.h"
#include "loadObject.h"
#include "RigidBody.h"

enum COLLIDER_TYPE
{
	COLLIDER_DEFAULT,
	PLAYER,
	BOMB_BASE,
	BOMB_EXPLOSION
};

class GameObject
{
public:
	GameObject();
	GameObject(glm::vec3 position, 
		std::shared_ptr<Loader> _mesh, 
		std::shared_ptr<Material> _material, 
		std::shared_ptr<Texture> _texture);	

	GameObject(GameObject&);
	~GameObject();

	void setTexture(std::shared_ptr<Texture> _texture) { texture = _texture; }

	void setPosition(glm::vec3 newPosition);
	void setRotationAngleX(float newAngle);
	void setRotationAngleY(float newAngle);
	void setRotationAngleZ(float newAngle);
	void setScale(glm::vec3 newScale);
	void setOutlineColour(glm::vec4 colour);

	virtual void update(float dt);	
	virtual void draw(Camera &camera);

	// Forward Kinematics
	// Pass in null to make game object a root node
	void setParent(GameObject* newParent);
	void addChild(GameObject* newChild);
	void removeChild(GameObject* rip);

	glm::mat4 getLocalToWorldMatrix();
	glm::vec3 getWorldPosition();
	glm::mat4 getWorldRotation();
	glm::vec3 getScale() { return m_pScale; }
	COLLIDER_TYPE getColliderType() { return colliderType; }

	bool isRoot();
	bool hasRigidBody() { return (rigidBody != nullptr); }
	virtual void attachRigidBody(std::unique_ptr<RigidBody> &_rb);
	virtual void checkCollisionWith(GameObject* other);

	void setMaterial(std::shared_ptr<Material> _material) { material = _material; }

	// Other Properties
	std::string name;
	float emissiveLight = 0.0f;

private:
	// Updates the local transform based on the updated variables
	void updateLocalTransform();
	// Sets the local variables based on the rigidbody (dynamic bodies)
	void setLocalTransformToBody();

protected:
	float m_pRotX, m_pRotY, m_pRotZ; // local rotation angles
	glm::vec3 m_pScale;

	glm::vec3 m_pLocalPosition;
	glm::mat4 m_pLocalRotation;

	glm::mat4 m_pLocalTransformMatrix;
	glm::mat4 m_pLocalToWorldMatrix;

	// Forward Kinematics
	GameObject* m_pParent;
	std::vector<GameObject*> m_pChildren;

	// Graphics components
	glm::vec4 outlineColour;
	float transparency;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Loader> mesh;
	std::shared_ptr<Material> material;

	// Rigid body for rigidbody dynamics
	// Using unique_ptr as objects shouldn't share rigidBodies
	std::unique_ptr<RigidBody> rigidBody;

	bool posNeedsUpdating, rotNeedsUpdating = false;
	COLLIDER_TYPE colliderType;
};
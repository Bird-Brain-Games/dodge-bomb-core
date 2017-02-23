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

class GameObject
{
protected:
	float m_pScale;

	float m_pRotX, m_pRotY, m_pRotZ; // local rotation angles

	glm::vec3 m_pLocalPosition;
	glm::mat4 m_pLocalRotation;

	glm::mat4 m_pLocalTransformMatrix;
	glm::mat4 m_pLocalToWorldMatrix;

	// Forward Kinematics
	GameObject* m_pParent;
	std::vector<GameObject*> m_pChildren;

public:
	GameObject();
	GameObject(glm::vec3 position, 
		std::shared_ptr<Loader> _mesh, 
		std::shared_ptr<Material> _material);
	~GameObject();

	void setTexture(std::shared_ptr<Texture> _texture) { texture = _texture; }

	void setPosition(glm::vec3 newPosition);
	void setRotationAngleX(float newAngle);
	void setRotationAngleY(float newAngle);
	void setRotationAngleZ(float newAngle);
	void setScale(float newScale);

	glm::mat4 getLocalToWorldMatrix();

	virtual void update(float dt);	
	virtual void draw(Camera &camera);

	// Forward Kinematics
	// Pass in null to make game object a root node
	void setParent(GameObject* newParent);
	void addChild(GameObject* newChild);
	void removeChild(GameObject* rip);
	glm::vec3 getWorldPosition();
	glm::mat4 getWorldRotation();
	bool isRoot();

	// Other Properties
	std::string name;
	glm::vec4 colour; 
	std::shared_ptr<Texture> texture;

	std::shared_ptr<Loader> mesh;
	std::shared_ptr<Material> material;
};
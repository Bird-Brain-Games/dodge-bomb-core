#pragma once

#include <map>
#include <string>
#include <vector>

#include <GLM\mat4x4.hpp>
#include "BulletDebug.h"
#include <btBulletDynamicsCommon.h>


// Class to manage physics operations and memory
class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();

	bool init();
	void update(float deltaTasSeconds, int maxStep);
	void drawDebug(glm::mat4x4 const& modelViewMatrix, glm::mat4x4 const& projectionMatrix);
	bool isDrawingDebug() { return useDebug; }

	btRigidBody::btRigidBodyConstructionInfo* getRigidBodyCI(std::string fileName);
	void addRigidBody(btRigidBody* rb);
	void removeRigidBody(btRigidBody* rb);

	void setDebugDraw(bool isDrawing);

	btDiscreteDynamicsWorld* dynamicsWorld;

private:
	// Create the rigid body construction information
	bool createRigidBodyCI(std::string fileName);
private:
	// Bullet core variables
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;

	// Bullet debug
	BulletDebugger *debugger;
	bool useDebug;

private:
	bool systemInit;
	std::map<std::string, btRigidBody::btRigidBodyConstructionInfo> CIMap;
	std::vector<btCollisionShape*> collisionShapes;
};


// Physics wrapper class for bullet physics rigid bodies
class RigidBody
{
public:
	RigidBody();
	~RigidBody();

	bool load(std::string fileName);
	glm::mat4x4 getWorldTransform();
	void setWorldTransform(glm::vec3 pos);
	void setWorldTransform(glm::vec3 pos, glm::vec4 quat);

public:
	static void systemUpdate(float deltaTasSeconds, int maxStep);
	static void drawDebug(glm::mat4x4 const& modelViewMatrix, glm::mat4x4 const& projectionMatrix);
	static void setDebugDraw(bool isDrawing) { Sys.setDebugDraw(isDrawing); }
	static bool isDrawingDebug() { return Sys.isDrawingDebug(); }
protected:
	static PhysicsEngine Sys;

private:
	btRigidBody *body;
};
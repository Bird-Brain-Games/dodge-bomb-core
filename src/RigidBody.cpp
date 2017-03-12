#include "RigidBody.h"
#include "GameObject.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "GLM\gtc\type_ptr.hpp"

PhysicsEngine RigidBody::Sys;

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////	PhysicsEngine
PhysicsEngine::PhysicsEngine()
{
	useDebug = false;
	systemInit = false;
}

PhysicsEngine::~PhysicsEngine()
{
	// Destroy collision shapes
	for (unsigned i = 0; i < collisionShapes.size(); i++)
	{
		delete collisionShapes.at(i);
		collisionShapes.at(i) = nullptr;
	}

	// Destroy construction information
	/*std::map<std::string, btRigidBody::btRigidBodyConstructionInfo*>::iterator it;
	for (it = CIMap.begin(); it != CIMap.end(); it++)
	{
		delete it->second;
	}*/

	// Destroy debugger
	delete debugger; debugger = nullptr;

	// Destroy Bullet core variables
	delete dynamicsWorld;	dynamicsWorld = nullptr;
	delete solver;	solver = nullptr;
	delete collisionConfiguration;	collisionConfiguration = nullptr;
	delete dispatcher;	dispatcher = nullptr;
	delete broadphase;	broadphase = nullptr;
}

bool PhysicsEngine::init()
{
	if (systemInit) return true;

	// Build the broadphase
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver;

	// The world
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -65, 0));

	// The debugger
	debugger = new BulletDebugger();
	dynamicsWorld->setDebugDrawer(debugger);
	systemInit = true;
	return true;
}

void PhysicsEngine::update(float deltaTasSeconds, int maxStep)
{
	dynamicsWorld->stepSimulation(deltaTasSeconds, maxStep);
}

void PhysicsEngine::drawDebug(glm::mat4x4 const& modelViewMatrix, glm::mat4x4 const& projectionMatrix)
{
	debugger->SetMatrices(modelViewMatrix, projectionMatrix);
	dynamicsWorld->debugDrawWorld();
	//debugger->drawVAO();
}

btRigidBody::btRigidBodyConstructionInfo* PhysicsEngine::getRigidBodyCI(std::string fileName)
{
	std::map<std::string, btRigidBody::btRigidBodyConstructionInfo>::iterator it;
	it = CIMap.find(fileName);
	if (it != CIMap.end())
	{
		return &(it->second);
	}
	else
	{
		bool result = createRigidBodyCI(fileName);
		if (!result)
		{
			return nullptr;
		}

		return &CIMap.at(fileName);
	}
}

void PhysicsEngine::addRigidBody(btRigidBody* rb)
{
	dynamicsWorld->addRigidBody(rb);
}

void PhysicsEngine::addRigidBody(btRigidBody* rb, short group, short mask)
{
	dynamicsWorld->addRigidBody(rb, group, mask);
}

void PhysicsEngine::removeRigidBody(btRigidBody* rb)
{
	dynamicsWorld->removeRigidBody(rb);
}

void PhysicsEngine::setDebugDraw(bool isDrawing)
{
	useDebug = isDrawing;

	if (useDebug)
		debugger->setDebugMode(1);
	else
		debugger->setDebugMode(0);
}

bool PhysicsEngine::createRigidBodyCI(std::string fileName)
{
	std::ifstream stream(fileName);
	if (!stream)
	{
		std::cerr << "Error: file " << fileName << " does not exist." << std::endl;
		return false;
	}

	// Collider properties
	float length, radius;
	btCollisionShape* shape = nullptr;
	float extents[3];

	// Rigidbody properties
	int	bodyType;		// Static, dynamic, or kinematic
	float friction;
	float restitution;	// bounciness
	float mass;
	std::string tag;


	// Parse through the file, gathering the necessary information
	std::string line;
	std::string key;
	std::size_t pos;
	
	while (std::getline(stream, line))
	{
		key = "";
		pos = line.find(':');
		if (pos != std::string::npos)
		{
			key = line.substr(0, pos);
			line = line.substr(pos + 1);
		}

		if (key == "tag")
		{
			tag = line;
		}
		else if (key == "extents")
		{
			std::stringstream sstream(line);
			for (unsigned int i = 0; i < 3; i++)
			{
				sstream >> extents[i];
			}
		}
		else if (key == "length")
			length = std::stof(line);
		else if (key == "radius")
			radius = std::stof(line);
		else if (key == "collisionShape")
		{
			int shapeType = std::stoi(line);
			btVector3 shapeExtents;
			switch (shapeType)
			{
			case 1:
				// Box model
				shapeExtents = btVector3(extents[0], extents[1], extents[2]);
				shape = new btBoxShape((shapeExtents / 2.0f));
				break;
			case 2:
				// Sphere model
				shape = new btSphereShape(radius);
				break;
			case 3:
				// Capsule model
				shape = new btCapsuleShape(radius, extents[1]);
				break;
			default:
				shape = nullptr;
				break;
			}
		}
		else if (key == "bodyType")
			bodyType = std::stoi(line);
		else if (key == "friction")
			friction = std::stof(line);
		else if (key == "restitution")
			restitution = std::stof(line);
		else if (key == "mass")
			mass = std::stof(line);
	}

	////////////////////////////////////////////////	Form the rigid body CI
	// Calculate local inertia if dynamic
	btVector3 inertia(0, 0, 0);
	if (bodyType == 2)
		shape->calculateLocalInertia(mass, inertia);
	else
		mass = 0.0f;

	// construct the CI
	btRigidBody::btRigidBodyConstructionInfo CI(mass, nullptr, shape, inertia);
	CI.m_restitution = restitution;
	CI.m_friction = friction;

	// Add the CI to the map
	CIMap.insert({ fileName, CI });
	collisionShapes.push_back(shape);

	stream.close();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////	RigidBody
RigidBody::RigidBody()
{
	body = nullptr;
	group = -1;
	mask = -1;
}

RigidBody::RigidBody(short _group, short _mask)
{
	body = nullptr;
	group = _group;
	mask = _mask;
}

RigidBody::RigidBody(RigidBody& other)
	:body(nullptr),
	group(other.group),
	mask(other.mask)
{
	
}

RigidBody::~RigidBody()
{
	Sys.removeRigidBody(body);
	delete body->getMotionState();
	body = nullptr;
}

bool RigidBody::load(std::string fileName)
{
	if (!Sys.init()) return false;

	try
	{
		btRigidBody::btRigidBodyConstructionInfo* rigidCI = Sys.getRigidBodyCI(fileName);
		body = new btRigidBody(*rigidCI);

		if (rigidCI->m_mass <= 0.0)
		{
			body->setCollisionFlags(btCollisionObject::CollisionFlags::CF_STATIC_OBJECT);
		}

		body->setMotionState(new btDefaultMotionState());

		if (group >= 0)
			Sys.addRigidBody(body, group, mask);
		else
			Sys.addRigidBody(body);

		u_fileName = fileName;
		return true;

	}
	catch (char const* e)
	{
		std::cerr << e << std::endl;
		return false;
	}


}

bool RigidBody::load(std::string fileName, btCollisionObject::CollisionFlags flags)
{
	if (!Sys.init()) return false;

	try
	{
		btRigidBody::btRigidBodyConstructionInfo* rigidCI = Sys.getRigidBodyCI(fileName);
		body = new btRigidBody(*rigidCI);

		body->setCollisionFlags(flags);
		body->setMotionState(new btDefaultMotionState());

		if (group >= 0)
			Sys.addRigidBody(body, group, mask);
		else
			Sys.addRigidBody(body);

		u_fileName = fileName;
		return true;

	}
	catch (char const* e)
	{
		std::cerr << e << std::endl;
		return false;
	}


}

void RigidBody::systemUpdate(float deltaTasSeconds, int maxStep)
{
	Sys.update(deltaTasSeconds, maxStep);
}

void RigidBody::setKinematic()
{
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	setDeactivationMode(DISABLE_DEACTIVATION);
	std::cout << body->isKinematicObject() << std::endl;
}

void RigidBody::setDeactivationMode(int mode)
{
	body->setActivationState(mode);
}

void RigidBody::drawDebug(glm::mat4x4 const& modelViewMatrix, glm::mat4x4 const& projectionMatrix)
{
	Sys.drawDebug(modelViewMatrix, projectionMatrix);
}

glm::mat4x4 RigidBody::getWorldTransform()
{
	btTransform t;
	btScalar m[16];
	body->getMotionState()->getWorldTransform(t);
	t.getOpenGLMatrix(m);
	return glm::make_mat4x4((float*)m);
}

void RigidBody::setWorldTransform(glm::vec3 pos, glm::vec3 rot)
{
	glm::mat3 x = glm::rotate(rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat3 y = glm::rotate(rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat3 z = glm::rotate(rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat3 rotations = z * y * x;
	btMatrix3x3 bullet(rotations[0].x, rotations[0].y, rotations[0].z,
		rotations[1].x, rotations[1].y, rotations[1].z,
		rotations[2].x, rotations[2].y, rotations[2].z);
	btTransform newTran(bullet, btVector3(pos.x, pos.y, pos.z));
	body->setWorldTransform(newTran);
}

void RigidBody::setWorldTransform(glm::vec3 pos)
{
	btTransform newTran = btTransform(body->getWorldTransform().getRotation(), btVector3(pos.x, pos.y, pos.z));
	if (body->isKinematicObject())
	{
		body->getMotionState()->setWorldTransform(newTran);
		return;
	}
	body->setWorldTransform(newTran);
}

void RigidBody::setWorldTransform(glm::mat4x4 transform)
{
	setWorldTransform(transform[3]);
}

void RigidBody::setScale(glm::vec3 newScale)
{
	body->getCollisionShape()->setLocalScaling(btVector3(
		newScale.x,
		newScale.y,
		newScale.z
	));
}

glm::vec3 RigidBody::getScale()
{
	btVector3 ls = body->getCollisionShape()->getLocalScaling();
	return glm::vec3(ls.getX(), ls.getY(), ls.getZ());
}

void RigidBody::setUserPointer(GameObject* gameObject)
{
	// WARNING: VERY UNSAFE
	userPointer = gameObject;
	body->getCollisionShape()->setUserPointer(userPointer);
	body->setUserPointer(userPointer);
}

void RigidBody::setLinearVelocity(glm::vec3 newVelocity)
{
	body->setLinearVelocity(
		btVector3(newVelocity.x, newVelocity.y, newVelocity.z));
}
#pragma once
// Classes used to debug the Bullet physics engine

#include <LinearMath\btIDebugDraw.h>
#include <GLM\glm.hpp>

class BulletDebugger : public btIDebugDraw
{
public:
	BulletDebugger();
	void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix);

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void   drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

	virtual void   reportErrorWarning(const char* warningString);

	virtual void   draw3dText(const btVector3& location, const char* textString);

	virtual void   setDebugMode(int _debugMode);

	virtual int      getDebugMode() const { return (int)debugMode; }

private:
	DebugDrawModes debugMode;
};


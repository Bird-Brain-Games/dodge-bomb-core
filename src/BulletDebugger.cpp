#include "BulletDebug.h"
#include "gl\glew.h"

BulletDebugger::BulletDebugger()
	: debugMode(DebugDrawModes::DBG_NoDebug)
{

}

void BulletDebugger::SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix)
{
	glUseProgram(0); // Use Fixed-function pipeline (no shaders)
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&pViewMatrix[0][0]);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&pProjectionMatrix[0][0]);
}

void BulletDebugger::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	glDisable(GL_DEPTH_TEST);
	//glPolygonOffset(1, 1);
	glColor3f(color.x(), color.y(), color.z());
	glBegin(GL_LINES);
		glVertex3f(from.x(), from.y(), from.z());
		glVertex3f(to.x(), to.y(), to.z());
	glEnd();
	glEnable(GL_DEPTH_TEST);
	//glPolygonOffset(0, 1);
}

void BulletDebugger::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{

}
	 
void BulletDebugger::reportErrorWarning(const char* warningString)
{

}
	 
void BulletDebugger::draw3dText(const btVector3& location, const char* textString)
{

}

void BulletDebugger::setDebugMode(int _debugMode)
{
	debugMode = (DebugDrawModes)_debugMode;
}
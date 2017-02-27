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
	//glDisable(GL_DEPTH_TEST);
	////glPolygonOffset(1, 1);
	//glColor3f(color.x(), color.y(), color.z());
	//glBegin(GL_LINES);
	//	glVertex3f(from.x(), from.y(), from.z());
	//	glVertex3f(to.x(), to.y(), to.z());
	//glEnd();
	//glEnable(GL_DEPTH_TEST);
	////glPolygonOffset(0, 1);

	float tmp[6] = { from.getX(), from.getY(), from.getZ(),
					to.getX(), to.getY(), to.getZ() };

	glPushMatrix();
	{
		glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);

		glVertexPointer(3, GL_FLOAT, 0, &tmp);

		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, 3);
		glDrawArrays(GL_LINES, 0, 3);
	}
	glPopMatrix();
}

void BulletDebugger::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{

}
	 
void BulletDebugger::reportErrorWarning(const char* warningString)
{
	printf(warningString);
}
	 
void BulletDebugger::draw3dText(const btVector3& location, const char* textString)
{

}

void BulletDebugger::setDebugMode(int _debugMode)
{
	debugMode = (DebugDrawModes)_debugMode;
}
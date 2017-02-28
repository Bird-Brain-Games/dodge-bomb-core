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
	temp_vertices.push_back(glm::vec3(from.getX(), from.getY(), from.getZ()));
	temp_vertices.push_back(glm::vec3(to.getX(), to.getY(), to.getZ()));
	temp_colours.push_back(glm::vec3(color.getX(), color.getY(), color.getZ()));
	temp_colours.push_back(glm::vec3(color.getX(), color.getY(), color.getZ()));
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

void BulletDebugger::drawVAO()
{
	vao.destroy();

	// how many pieces of data do we have.
	int size = temp_vertices.size();
	// times by 3 because each vertex data has 3 floats worth of data.

	if (temp_vertices.size() > 0)
	{
		Attribute positionAttrib(AttributeLocations::VERTEX, GL_FLOAT, sizeof(glm::vec3), 3, size, "vPos", temp_vertices.data());
		vao.addAttribute(positionAttrib);
	}

	// set up other attributes...
	if (temp_colours.size() > 0)
	{
		Attribute positionAttrib(AttributeLocations::COLOUR, GL_FLOAT, sizeof(glm::vec3), 3, size, "colour", temp_colours.data());
		vao.addAttribute(positionAttrib);
	}

	
	vao.createVAO();
	vao.draw();

	//if (vaoHandle)
	//{
	//	glBindVertexArray(vaoHandle);

	//	glDrawArrays(GL_TRIANGLES, 0, attributes[0].getNumElements());

	//	glBindVertexArray(0);
	//}

	temp_vertices.clear();
	temp_colours.clear();
}
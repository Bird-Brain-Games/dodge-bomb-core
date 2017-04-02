/*Campbell Hamilton 100582048
january 2017
*/

#include "VAO.h"

Attribute::Attribute(AttributeLocations _attribLocation, GLenum _element, unsigned int _dataSize, unsigned int _EPA, unsigned int _numElements, std::string _name, void* _data)
{
	attribLocation = _attribLocation;
	element = _element;
	dataSize = _dataSize;
	elementsPerAttrib = _EPA;
	numElements = _numElements;
	attributeName = _name;
	data = _data;
}

VAO::VAO()
{
	vaoHandle = 0;
	primitiveType = GL_TRIANGLES;
}
VAO::~VAO()
{
	destroy();
}

int  VAO::addAttribute(Attribute attrib)
{
	attributes.push_back(attrib);
	return 1;
}

void VAO::createVAO(GLenum vboUsage)
{
	if (vaoHandle) destroy();

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	unsigned int buffers = attributes.size();
	vboHandles.resize(buffers);

	glGenBuffers(buffers, &vboHandles[0]);

	for (int i = 0; i < buffers; i++)
	{
		Attribute* attrib = &attributes[i];
		glEnableVertexAttribArray(attrib->getAttribLocation());
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[i]);
		glBufferData(GL_ARRAY_BUFFER, attrib->getNumElements() * attrib->getDataSize(), attrib->data, vboUsage);
		glVertexAttribPointer(attrib->getAttribLocation(), attrib->getElementsPerAttrib(), attrib->getElement(), GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void VAO::draw()
{
	if (vaoHandle)
	{
		glBindVertexArray(vaoHandle);

		glDrawArrays(primitiveType, 0, attributes[0].getNumElements());

		glBindVertexArray(0);
	}
}

void VAO::drawLines()
{
	if (vaoHandle)
	{
		glBindVertexArray(vaoHandle);

		glDrawArrays(GL_LINES, 0, attributes[0].getNumElements());

		glBindVertexArray(0);
	}
}

void VAO::destroy()
{
	if (vaoHandle)
	{
		glDeleteVertexArrays(1, &vaoHandle);
		glDeleteBuffers(vboHandles.size(), &vboHandles[0]);
		vaoHandle = 0;
	}

	vboHandles.clear();
	attributes.clear();
}

void VAO::setPrimitive(GLenum type)
{
	primitiveType = type;
}

Attribute* VAO::getAttribute(AttributeLocations location)
{
	for (int i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getAttribLocation() == location)
			return &attributes[i];
	}
}

unsigned int VAO::getVBO(AttributeLocations location)
{
	for (int i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getAttribLocation() == location)
			return vboHandles[i];
	}
	return 0;
}

unsigned int VAO::getVAO()
{
	return vaoHandle;
}

AttributeLocations		Attribute::getAttribLocation() { return attribLocation; }
GLenum			Attribute::getElement() { return element; }
unsigned int	Attribute::getDataSize() { return dataSize; }
unsigned int	Attribute::getElementsPerAttrib() { return elementsPerAttrib; }
unsigned int	Attribute::getNumElements() { return numElements; }
std::string		Attribute::getAttributeName() { return attributeName; }

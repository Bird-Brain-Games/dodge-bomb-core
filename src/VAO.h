/*Campbell Hamilton 100582048
january 2017
*/

#pragma once
#include "gl\glew.h"
#include <string>
#include <vector>

//attribute locations;
enum AttributeLocations
{
	VERTEX = 0,
	NORMAL,
	TEX_COORD,
	COLOUR,
	BONES = 8,
	WEIGHTS
};

// Contatins all the data needed to send our attributes to the shader
class Attribute
{
public:
	Attribute(AttributeLocations _attribLocation, GLenum _element, 
		unsigned int _dataSize, unsigned int _EPA, 
		unsigned int numElements, std::string _name, void* _data);

	AttributeLocations  getAttribLocation();
	GLenum getElement();
	unsigned int getDataSize();
	unsigned int getElementsPerAttrib();
	unsigned int getNumElements();
	std::string getAttributeName();
	void* data;						// pointer to data


private:
	AttributeLocations  attribLocation;

	GLenum element;					// type of data
	unsigned int dataSize;			// size of the data
	unsigned int elementsPerAttrib; //number of elements for one attribute (ie 3 floats for a vector)
	unsigned int numElements;		// number of elements in entire array
	std::string attributeName;		//name of the attribute in the shader

};

//contains all the attributes for an object
class VAO
{
public:
	VAO();
	~VAO();

	int addAttribute(Attribute attrib);
	void createVAO();
	void draw();
	void destroy();

private:

	//a handle to our VAO. which we use to tell opengl what VBO's to use
	unsigned int vaoHandle;

	//handles to our VBO. VBO's being our data we have saved to our GPU.
	std::vector<unsigned int> vboHandles;
	//Pointer to our arrays of data.
	std::vector<Attribute> attributes;
};
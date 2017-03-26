// loadObject.cpp
// - Wavefront OBJ loader
// - Only takes in faces with 3 points (triangles)
// - Only the most basic functionality

#include "loadObject.h"

#include <iostream>
#include <fstream>

#include <GL\freeglut.h>
#include <IL\ilut.h>

#include "Shader.h"

void LoadObject::draw(std::shared_ptr<ShaderProgram> s)
{
	vao.draw();
}

bool LoadObject::load(std::string filename)
{
	return load(filename.c_str());
}

bool LoadObject::load(char const* filename)
{
	std::ifstream input(filename);
	if (!input)
	{
		// error handling
		std::cerr << "Error: could not load " << filename << std::endl;
		return false;
	}
	std::vector<glm::vec3>	temporary_vertices;
	std::vector<glm::vec2>	temporary_uvs;
	std::vector<glm::vec3>	temporary_normals;
	char buf[256];
	while (input.getline(buf, 256)) //read every line
	{
		switch (buf[0])
		{
		case 'v': // v, vn, vt
			switch (buf[1])
			{
			case ' ': // v or vertex
				float x, y, z;
				sscanf_s(buf, "v %f %f %f", &x, &y, &z);
				temporary_vertices.push_back(glm::vec3(x, y, z));
				break;
			case 'n': // vn or normal
				float nx, ny, nz;
				sscanf_s(buf, "vn %f %f %f", &nx, &ny, &nz);
				temporary_normals.push_back(glm::vec3(nx, ny, nz));
				break;
			case 't': // vt or texture coordinate
				float u, v;
				sscanf_s(buf, "vt %f %f", &u, &v);
				temporary_uvs.push_back(glm::vec2(u, v));
				break;
			}
			break;
		case 'f': // f or face
			unsigned int vertexIndices[3], uvIndices[3], normalIndices[3];

			// %u for unsigned ints, search for "scanf format string" if unsure about this
			sscanf_s(buf, "f %u/%u/%u %u/%u/%u %u/%u/%u",
				&vertexIndices[0], &uvIndices[0], &normalIndices[0],
				&vertexIndices[1], &uvIndices[1], &normalIndices[1],
				&vertexIndices[2], &uvIndices[2], &normalIndices[2]
			);
			vertices.push_back(temporary_vertices[vertexIndices[0] - 1]);
			vertices.push_back(temporary_vertices[vertexIndices[1] - 1]);
			vertices.push_back(temporary_vertices[vertexIndices[2] - 1]);

			texcoords.push_back(temporary_uvs[uvIndices[0] - 1]);
			texcoords.push_back(temporary_uvs[uvIndices[1] - 1]);
			texcoords.push_back(temporary_uvs[uvIndices[2] - 1]);

			normals.push_back(temporary_normals[normalIndices[0] - 1]);
			normals.push_back(temporary_normals[normalIndices[1] - 1]);
			normals.push_back(temporary_normals[normalIndices[2] - 1]);

			break;
		}
	}
	input.close();
	createVAO();
	return true;
}

void LoadObject::createVAO()
{
	// how many pieces of data do we have.
	int size = vertices.size();
	// times by 3 because each vertex data has 3 floats worth of data.

	if (vertices.size() > 0)
	{
		Attribute positionAttrib(AttributeLocations::VERTEX, GL_FLOAT, sizeof(glm::vec3), 3, size, "vPos", vertices.data());
		vao.addAttribute(positionAttrib);

	}

	// Set up UV attribute
	if (texcoords.size() > 0)
	{
		Attribute UVAttrib(AttributeLocations::TEX_COORD, GL_FLOAT, sizeof(glm::vec2), 2, size, "texture", texcoords.data());
		vao.addAttribute(UVAttrib);
	}

	// Set up normal attribute
	if (normals.size() > 0)
	{
		Attribute normalAttrib(AttributeLocations::NORMAL, GL_FLOAT, sizeof(glm::vec3), 3, size, "normal", normals.data());
		vao.addAttribute(normalAttrib);
	}

	// set up other attributes...

	vao.createVAO();
}
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

LoadObject::~LoadObject()
{
	glDeleteBuffers(1, &vertbo);
	glDeleteBuffers(1, &normbo);
	glDeleteBuffers(1, &texbo);
	glDeleteBuffers(1, &colorbo);
	glDeleteBuffers(1, &vao);
}

void LoadObject::draw()
{
	glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, numtris);
	glBindVertexArray(0);
}

bool LoadObject::loadFromObject(char const* filename)
{
	std::ifstream input(filename);
	if (!input)
	{
		// error handling
		std::cerr << "Error: could not load " << filename << std::endl;
		return false;
	}

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords; //note: vec2
	std::vector<Face> faces;

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
				vertices.push_back(glm::vec3(x, y, z));
				break;
			case 'n': // vn or normal
				float nx, ny, nz;
				sscanf_s(buf, "vn %f %f %f", &nx, &ny, &nz);
				normals.push_back(glm::vec3(nx, ny, nz));
				break;
			case 't': // vt or texture coordinate
				float u, v;
				sscanf_s(buf, "vt %f %f", &u, &v);
				texcoords.push_back(glm::vec2(u, v));
				break;
			}
			break;
		case 'f': // f or face
			unsigned int vertexIndices[3], uvIndices[3], normalIndices[3];
			unsigned int numMatches;

			// %u for unsigned ints, search for "scanf format string" if unsure about this
			numMatches = sscanf_s(buf, "f %u/%u/%u %u/%u/%u %u/%u/%u",
				&vertexIndices[0], &uvIndices[0], &normalIndices[0],
				&vertexIndices[1], &uvIndices[1], &normalIndices[1],
				&vertexIndices[2], &uvIndices[2], &normalIndices[2]
				);
			if (numMatches != 9) // OBJ could theoretically have less than 9 results, but no
			{
				std::cerr << "Error in file: " 
					<< filename << " numMatches wasn't 9" << std::endl;
				return false;
			}

			Face temp;

			temp.vertices[0] = vertexIndices[0] - 1;
			temp.vertices[1] = vertexIndices[1] - 1;
			temp.vertices[2] = vertexIndices[2] - 1;

			temp.textures[0] = uvIndices[0] - 1;
			temp.textures[1] = uvIndices[1] - 1;
			temp.textures[2] = uvIndices[2] - 1;

			temp.normals[0] = normalIndices[0] - 1;
			temp.normals[1] = normalIndices[1] - 1;
			temp.normals[2] = normalIndices[2] - 1;

			faces.push_back(temp);
			break;
		}
	}
	input.close();


	//convert obj to interleaved vertex array
	numtris = faces.size() * 3;

	this->verts = new float[numtris * 3];
	this->norms = new float[numtris * 3];
	this->texs = new float[numtris * 2];

	for (unsigned long i = 0; i < faces.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			this->verts[i * 9 + j * 3 + 0] = vertices[faces[i].vertices[j]].x;
			this->verts[i * 9 + j * 3 + 1] = vertices[faces[i].vertices[j]].y;
			this->verts[i * 9 + j * 3 + 2] = vertices[faces[i].vertices[j]].z;

			this->texs[i * 6 + j * 2 + 0] = texcoords[faces[i].textures[j]].x;
			this->texs[i * 6 + j * 2 + 1] = texcoords[faces[i].textures[j]].y;

			this->norms[i * 9 + j * 3 + 0] = normals[faces[i].normals[j]].x;
			this->norms[i * 9 + j * 3 + 1] = normals[faces[i].normals[j]].y;
			this->norms[i * 9 + j * 3 + 2] = normals[faces[i].normals[j]].z;
		}
	}

	this->colors = new float[numtris * 3];
	for (unsigned int i = 0; i < numtris * 3; i++)
	{
		colors[i] = norms[i] + 1.0f / 2.0f;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		// verts tho
		glGenBuffers(1, &vertbo);
		glBindBuffer(GL_ARRAY_BUFFER, vertbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris * 3, this->verts, GL_STATIC_DRAW);
			glEnableVertexAttribArray(4); // position/vertices
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// textures tho(poopybutt)
		glGenBuffers(1, &texbo);
		glBindBuffer(GL_ARRAY_BUFFER, texbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris*2, this->texs, GL_STATIC_DRAW);
			glEnableVertexAttribArray(5); // texcoords/uv
			glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// normals tho
		glGenBuffers(1, &normbo);
		glBindBuffer(GL_ARRAY_BUFFER, normbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris*3, this->norms, GL_STATIC_DRAW);
			glEnableVertexAttribArray(6); //  normals
			glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// normals tho
		glGenBuffers(1, &colorbo);
		glBindBuffer(GL_ARRAY_BUFFER, colorbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numtris*3, this->colors, GL_STATIC_DRAW);
			glEnableVertexAttribArray(7); //  colors
			glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	delete[] this->texs;
	delete[] this->norms;
	delete[] this->verts;
	delete[] this->colors;

	return true;
}

Texture::Texture(char* _diffuseTex, char* _specularTex, float _shininess)
{
	diffuseTex = ilutGLLoadImage(_diffuseTex);
	specularTex = ilutGLLoadImage(_specularTex);
	shininess = _shininess;
}

Texture::Texture(GLuint _diffuseTex, GLuint _specularTex, float _shininess)
{
	diffuseTex = _diffuseTex;
	specularTex = _specularTex;
	shininess = _shininess;
}

void Texture::bind(Shader* s)
{
	s->uniformTex("diffuseTex", diffuseTex, 0);
	s->uniformTex("specularTex", specularTex, 1);
	s->uniformFloat("shininess", 25.0f);
}

void Texture::setShininess(float shine)
{
	shininess = shine;
}
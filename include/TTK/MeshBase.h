//////////////////////////////////////////////////////////////////////////
//
// This header is a part of the Tutorial Tool Kit (TTK) library. 
// You may not use this header in your GDW games.
//
// This is the base class for all meshes.
//
// Michael Gharbharan 2015
//
//////////////////////////////////////////////////////////////////////////

#ifndef MESH_BASE_H
#define MESH_BASE_H

#include <vector>
#include "GLM/glm.hpp"
#include "VertexBufferObject.h"

namespace TTK
{
	enum PrimitiveType
	{
		Triangles = 1,
		Quads
	};

	class MeshBase
	{
	public:
		// Description:
		// Very simple draw function which binds all three buffers
		// Yes, it uses OpenGL 1.0 draw calls... for now.
		void draw_1_0();

		// The modern draw function which uses vertex buffer objects!
		void draw();

		// Description:
		// Sets all per-vertex colours to the specified colour
		void setAllColours(glm::vec4 colour);
		
		void createVBO();

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<glm::vec4> colours;

		PrimitiveType primitiveType;

		VertexBufferObject vbo;
	};
}

#endif
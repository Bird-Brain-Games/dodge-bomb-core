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
		void draw();

		// Description:
		// Sets all per-vertex colours to the specified colour
		void setAllColours(glm::vec4 colour);

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<glm::vec4> colours;

		PrimitiveType primitiveType;
	};
}

#endif
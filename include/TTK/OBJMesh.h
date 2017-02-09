//////////////////////////////////////////////////////////////////////////
//
// This header is a part of the Tutorial Tool Kit (TTK) library. 
// You may not use this header in your GDW games.
//
//
// Michael Gharbharan 2015
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "TTK/MeshBase.h"
#include <string>

namespace TTK
{
	class OBJMesh : public MeshBase
	{
	public:
		void loadMesh(std::string filename);
	};
}
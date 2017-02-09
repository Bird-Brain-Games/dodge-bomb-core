//////////////////////////////////////////////////////////////////////////
//
// This header is a part of the Tutorial Tool Kit (TTK) library. 
// You may not use this header in your GDW games.
//
//
// Michael Gharbharan 2016
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>

namespace TTK
{
	namespace IO
	{
		// Loads the specified text file from disk and returns a copy of it in a std::string
		std::string loadFile(std::string fileName);
	}
}
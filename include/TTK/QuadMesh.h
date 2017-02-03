#pragma once

#include "TTK/MeshBase.h"

class QuadMesh : public TTK::MeshBase
{
public:
	QuadMesh();
	~QuadMesh();

	void setUVbottomLeft(float u, float v);
	void setUVtopLeft(float u, float v);
	void setUVbottomRight(float u, float v);
	void setUVtopRight(float u, float v);
};
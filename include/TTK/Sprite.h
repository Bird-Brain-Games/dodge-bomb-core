#pragma once

#include <vector>


#include <TTK\GraphicsUtils.h>
#include <TTK\Texture2D.h>

#include "QuadMesh.h"
#include "GameObject.h"

struct SpriteCoordinates
{
public:
	float xMin, xMax, yMin, yMax; // in pixels
	float uMin, uMax, vMin, vMax; // normalized texture coordinates
};

class Sprite : public GameObject
{
private:
	std::vector<glm::vec3> m_pPoints; // the points which make up the quad

	glm::vec3 m_pColour;

	QuadMesh* m_pMesh;
	TTK::Texture2D* m_pTexture;

	std::vector<SpriteCoordinates> m_pSpriteCoordinates;
	glm::vec2 m_pCurrentFrame;

	// x is the number of sprites in a single row
	// y is the number of rows
	glm::vec2 m_pSpritesDim;

	bool m_pLoops;

	const float m_pTimePerFrame = (1000.0f / 2.0f) / 1000.0f;
	float m_pCurrentFrameTime;

public:
	Sprite(QuadMesh* quad, TTK::Texture2D* tex);
	~Sprite();

	// Description:
	// Calculates the coordinates for each sprite in the sheet
	// Arguments:
	// spriteSizeX is the width of each sprite in the sheet
	// spriteSizeY is the height of each sprite in the sheet
	// numSpritesPerRow is the number of sprites in a single row
	// numRows is the number of rows that make up the sheet
	void generateSpriteCoordinates(float spriteSizeX, float spriteSizeY, int numSpritesPerRow, int numRows);

	void setColour(glm::vec3 newColour);

	virtual void update(float dt);

	void draw();

	void setLooping(bool loop);
};
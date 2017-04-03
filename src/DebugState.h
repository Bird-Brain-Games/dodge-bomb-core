#pragma once

#include "GameState.h"
#include "camera.h"

class Debug : public GameState
{
public:
	Debug(Camera*);
	void update(float dt);
	void draw();
private:
	Camera * camera;
};
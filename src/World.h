#pragma once

#include <vector>
#include <map>

#include "loadObject.h"
#include "ANILoader.h"
#include <memory>

// Handles memory management of textures and models
// look into shared pointer and deconstruction
class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	LoadObject* getModel(std::string fileName);
	ANILoader* getAniModel(std::string fileName);
	//Texture* getTexture(std::string tag);

	//void loadModel(std::string fileName);
	//void loadAniModel(std::string fileName);

	//// Uses a tag system to identify textures (SHOULD BE CHANGED)
	//void loadTexture(char* diffuseTex, char* specularTex, float shininess, std::string tag);

private:
	std::map<std::string, LoadObject*> modelMap;
	std::map<std::string, ANILoader*> aniModelMap;
	//std::map<std::string, Texture*> textureMap;
};

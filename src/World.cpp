#include "World.h"
#include <iostream>
GameWorld::GameWorld()
{
	std::cout << "World created" << std::endl;
}

GameWorld::~GameWorld()
{
	//Look into share pointer deconstruction 
}

LoadObject* GameWorld::getModel(std::string fileName)
{
	// Check if the model is in the map.
	auto& it = modelMap.find(fileName);

	// If model isn't in map, create it.
	if (it == modelMap.end())
	{
		LoadObject* obj = new LoadObject();

		bool result = obj->load(fileName.c_str());

		// If loading fails, delete and return nullptr.
		if (!result)
		{
			return nullptr;
		}

		// Add the model to the map, and return it.
		modelMap.insert({ fileName, obj });
		return obj;
	}

	// If model exists, return it.
	return it->second;
}

ANILoader* GameWorld::getAniModel(std::string fileName)
{
	// Check if the animated model is in the map.
	auto& it = aniModelMap.find(fileName);

	// If animated model isn't in map, create it.
	if (it != aniModelMap.end() || aniModelMap.size() == 0)
	{
		ANILoader* ani = new ANILoader();
		bool result = ani->loadHTR(fileName);
		
		// If loading fails, delete and return nullptr.
		if (!result)
		{
			return nullptr;
		}

		// Add the animated model to the map, and return it.
		ani->createNodes();
		aniModelMap.insert({ fileName, ani });
		return ani;
	}

	// If animated model exists, return it.
	return it->second;
}

//Texture* GameWorld::getTexture(std::string fileName)
//{
//	// Check if the texture is in the map.
//	auto& it = textureMap.find(fileName);
//
//	// If texture isn't in map, create it.
//	if (it != textureMap.end())
//	{
//		Texture* tex = new Texture()
//
//		// If loading fails, delete and return nullptr.
//		if (!result)
//		{
//			delete ani;
//			return nullptr;
//		}
//
//		// Add the texture to the map, and return it.
//		ani->createNodes();
//		textureMap.insert({ fileName, ani });
//		return ani;
//	}
//
//	// If texture exists, return it.
//	return it->second;
//}

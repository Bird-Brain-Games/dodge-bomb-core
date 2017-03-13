#pragma once
#include "GameState.h"
#include "GameObject.h"
#include "menu.h"
#include "controller.h"
#include "Player.h"
#include "InputManager.h"
#include "FrameBufferObject.h"
#include <GLEW\glew.h>
#include "gl\freeglut.h"
#include "IL\ilut.h"
// Framebuffer objects


// Outline Controls


//////////////////////////////////////////////////////////////////////
///////////////////////	Lighting Controls	//////////////////////////
enum LightingMode
{
	NOLIGHT,
	TOON
};



class MainMenu : public GameState
{
public:
	MainMenu(std::shared_ptr<Menu>);

	void update(float dt);
	void draw();
	void setPaused(bool);

private:
	int position, frame;
	std::shared_ptr<Menu> atlas;
};

class Pause : public GameState
{
public:
	Pause(std::shared_ptr<Menu>);
	void setPaused(bool);
	void update(float dt);
	void draw();
	void setActive(Controller*);
	Controller *active;
	//used so we cant pause then immediately unpause.
	float pauseTimer;
private:
	//when we pause it passes the controller used so only that player can unpause
	std::shared_ptr<Menu> atlas;
};

class Score : public GameState
{
public:
	Score(std::shared_ptr<Menu>);
	void setPaused(bool);
	void update(float dt);
	void draw();
	Controller *active;
	std::shared_ptr<Menu> atlas;
private:
	float pauseTimer;
};


class Debug : public GameState
{
public:
	Debug(Camera*);
	void update(float dt);
	void draw();
private:
	Camera * camera;
};

class Game : public GameState
{
public:
	Game(std::map<std::string, std::shared_ptr<GameObject>>*,
		std::map<std::string, std::shared_ptr<Player>>*,
		std::map<std::string, std::shared_ptr<Material>>*,
		std::shared_ptr<BombManager>,
		Pause*, Score*, Camera*);

	void update(float dt);//where we do all the updates and controls
	void draw();// where we do all 
	void setPaused(int a_paused);

private:


	//secondary functions (called in update and draw)
	bool lifeCheck();
	void updateScene(float dt);
	void drawScene();
	void setMaterialForAllGameObjects(std::string materialName);
	void setMaterialForAllPlayerObjects(std::string materialName);
	void brightPass();
	void blurPass(FrameBufferObject fboIn, FrameBufferObject fboOut);
	void initializeFrameBuffers();
	void handleKeyboardInput();
	void handleKeyboardInputShaders();

	FrameBufferObject fboUnlit;
	FrameBufferObject fboBright;
	FrameBufferObject fboBlur, fboBlurB;
	FrameBufferObject shadowMap;

	std::shared_ptr<BombManager> bombManager;
	std::map<std::string, std::shared_ptr<GameObject>>* scene;
	std::map<std::string, std::shared_ptr<Player>>* players;
	std::map<std::string, std::shared_ptr<Material>>* materials;
	Pause* pause;
	Score* score;
	Camera * camera;
	int pausing;
	float pauseTimer;

	float outlineWidth = 4.0;
	bool outlineToggle = true;
	LightingMode currentLightingMode = TOON;

	float windowWidth = 1920.0;
	float windowHeight = 1080.0;


	// Lighting Controls
	float deskLamp = 0.8;
	float innerCutOff = 0.42; // Spot Light Size
	float outerCutOff = 0.47;
	glm::vec3 deskForward = glm::vec3(0.2, 1.0, 1.5); // Spot Light Direction
	float roomLight = 0.4;

	float ambient = 0.1; // Ambient Lighting
	float diffuse = 1.0f; // Diffuse Lighting
	float specular = 0.2f; // Specular Lighting
	float shininess = 2.0f; // Shinniness
	float rim = 0.5f; // Rim Lighting

	GLuint toonRamp;

	// Bloom Controls
	glm::vec4 bloomThreshold = glm::vec4(0.6f);
	int numBlurPasses = 4;
	bool bloomToggle = false;

	// For Toggling
	bool  ambientToggle = true;
	float ka = ambient; // Ambient Lighting

	bool  diffuseToggle = true;
	float kd = diffuse; // Diffuse Lighting

	bool  specularToggle = true;
	float ks = specular; // Specular Lighting

	bool  rimToggle = true;
	float kr = rim; // Rim Lighting

};


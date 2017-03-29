#pragma once

#include "GameState.h"
#include <memory>
#include <map>
#include "PauseState.h"
#include "ScoreState.h"
#include "camera.h"
#include "Player.h"
#include "FrameBufferObject.h"
#include "Texture2D.h"

//////////////////////////////////////////////////////////////////////
///////////////////////	Lighting Controls	//////////////////////////
enum LightingMode
{
	NOLIGHT,
	TOON
};

class LUT
{
public:
	bool load(std::string filePath);
	void bind(GLuint);
	void unbind(GLuint);
	float getSize() { return size; }

private:
	GLuint handle;
	float size;
};


class Game : public GameState
{
public:
	Game(std::map<std::string, std::shared_ptr<GameObject>>*,
		std::map<std::string, std::shared_ptr<Player>>*,
		std::map<std::string, std::shared_ptr<Material>>*,
		std::vector<std::shared_ptr<GameObject>>*,
		std::shared_ptr<BombManager>,
		Pause*, Score*, Camera*);

	void update(float dt);//where we do all the updates and controls
	void draw();// where we do all 
	void setPaused(int a_paused);
	void windowReshapeCallbackFunction(int w, int h);

private:
	
	//secondary functions (called in update and draw)
	int deathCheck();
	void resetPlayers();
	void updateScene(float dt);
	void drawScene(Camera* _camera, Camera* _shadow);

	void setMaterialForAllGameObjects(std::string materialName);
	void setMaterialForAllPlayerObjects(std::string materialName);

	void brightPass();
	void blurBrightPass();
	void colorCorrectionPass(FrameBufferObject fboIn, FrameBufferObject fboOut);

	void initializeFrameBuffers();
	void handleKeyboardInput();
	void handleKeyboardInputShaders();
private:
	enum LUT_MODE
	{
		LUT_OFF,
		LUT_CONTRAST,
		LUT_SEPIA
	};

	enum GAME_STATE
	{
		READYUP,
		MAIN
	};
	GAME_STATE currentGameState;
	void changeState(GAME_STATE);

private:
	FrameBufferObject fboUnlit;
	FrameBufferObject fboBright;
	FrameBufferObject fboBlurA, fboBlurB;
	FrameBufferObject fboColorCorrection;
	glm::vec4 clearColor = glm::vec4(0.3, 0.0, 0.0, 1.0);

	std::shared_ptr<BombManager> bombManager;
	std::map<std::string, std::shared_ptr<GameObject>>* scene;
	std::map<std::string, std::shared_ptr<Player>>* players;
	std::map<std::string, std::shared_ptr<Material>>* materials;
	std::vector<std::shared_ptr<GameObject>>* obstacles;

	Pause* pause;
	Score* score;
	Camera * camera;
	int pausing;
	float pauseTimer;

	LUT contrastLUT;
	LUT sepiaLUT;
	LUT_MODE colorCorrection;

	float outlineWidth = 4.0;
	bool outlineToggle = true;
	LightingMode currentLightingMode = TOON;

	float windowWidth = 1920.0;
	float windowHeight = 1080.0;

	// Lighting Controls
	float innerCutOff = 0.78; // Spot Light Size
	float outerCutOff = 0.81;
	glm::vec3 deskForward = glm::vec3(0.47f, 1.0f, 0.99f); // Spot Light Direction
	glm::vec4 lightPos = glm::vec4(60.0f, 94.0, -15.0f, 1.0f); // Spot Light

	glm::vec4 lightTwo = glm::vec4(0.0f, 80.0, 100.0f, 1.0f); // Room Light

	float deskLamp = 0.5;
	float roomLight = 0.5;
	Camera shadowCamera;
	FrameBufferObject shadowMap;

	float ambient = 0.1; // Ambient Lighting
	float diffuse = 1.0f; // Diffuse Lighting
	float specular = 0.2f; // Specular Lighting
	float shininess = 2.0f; // Shinniness
	float rim = 0.5f; // Rim Lighting

	GLuint toonRamp;

	// Bloom Controls
	TTK::Texture2D dirtMask;
	glm::vec4 bloomThreshold = glm::vec4(0.5f);
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
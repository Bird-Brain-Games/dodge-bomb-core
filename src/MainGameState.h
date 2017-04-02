#pragma once

#include "GameState.h"
#include <memory>
#include <map>
#include "PauseState.h"
#include "ScoreState.h"
#include "camera.h"
#include "Player.h"
#include "FrameBufferObject.h"
#include "particles.h"
#include "sound engine.h"

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
	enum LUT_MODE
	{
		LUT_OFF,
		LUT_CONTRAST,
		LUT_SEPIA
	};

	enum GAME_STATE
	{
		READYUP,
		COUNTDOWN,
		MAIN,
		WIN
	};

private:
	GAME_STATE currentGameState;
	void changeState(GAME_STATE);


public:
	Game(std::map<std::string, std::shared_ptr<GameObject>>*,
		std::map<std::string, std::shared_ptr<Player>>*,
		std::map<std::string, std::shared_ptr<Material>>*,
		std::map<std::string, std::shared_ptr<Texture>>*,
		std::vector<std::shared_ptr<GameObject>>*,
		std::vector<std::shared_ptr<GameObject>>*,
		std::shared_ptr<BombManager>,
		std::shared_ptr<Menu>,
		std::map<std::string, Sound>* _soundTemplates,
		Pause*, Score*, Camera*);

	void update(float dt);//where we do all the updates and controls
	void draw();// where we do all 
	void setPaused(int a_paused);
	void windowReshapeCallbackFunction(int w, int h);
	GAME_STATE getCurrentState() { return currentGameState; }

private:
	
	//secondary functions (called in update and draw)
	int deathCheck();
	void resetPlayers();
	void makePlayersInactive();
	void updateScene(float dt);
	void updateReadyPass(float dt);
	void drawScene(Camera* _camera, Camera* _shadow);

	void setMaterialForAllGameObjects(std::string materialName);
	void setMaterialForAllPlayerObjects(std::string materialName);

	void fboToScreen(FrameBufferObject& input);
	void depthOfField(FrameBufferObject& input, FrameBufferObject& output);
	void bloomPass(FrameBufferObject& input, FrameBufferObject& fboToDrawTo);
	void colorCorrectionPass(FrameBufferObject& fboIn, FrameBufferObject& fboOut);
	void bokehPass(FrameBufferObject& fboToSample, FrameBufferObject& fboToDrawTo, float filterAngleDeg);

	void initializeFrameBuffers();
	void handleKeyboardInput();
	void handleKeyboardInputShaders();

	void changeColorCorrection(LUT_MODE);



private:
	FrameBufferObject fboUnlit;
	FrameBufferObject fboBright;
	FrameBufferObject fboBlurA, fboBlurB;
	FrameBufferObject fboColorCorrection;
	FrameBufferObject bokehHorizontalfbo;
	FrameBufferObject bokehAfbo;
	FrameBufferObject bokehBfbo;
	FrameBufferObject fboBloomed;
	FrameBufferObject fboWithBokeh;
	FrameBufferObject spunkMap;

	glm::vec4 clearColor = glm::vec4(0.3, 0.0, 0.0, 0.0);

	std::shared_ptr<BombManager> bombManager;
	std::map<std::string, std::shared_ptr<GameObject>>* scene;
	std::map<std::string, std::shared_ptr<Player>>* players;
	std::map<std::string, std::shared_ptr<Material>>* materials;
	std::map<std::string, std::shared_ptr<Texture>>* textures;
	std::vector<std::shared_ptr<GameObject>>* obstacles;
	std::vector<std::shared_ptr<GameObject>>* readyUpRings;
	std::vector<glm::vec3> defaultPlayerPositions;
	std::map<std::string, Sound>* soundTemplates;

	Pause* pause;
	Score* score;
	Camera * camera;

	Sound m_gameMusic;
	Sound m_gameTrack1;
	Sound m_gameTrack2;
	Sound m_gameTrack3;
	float fadeTimer;
	int numTracksPlaying;

	Sound s_countDown;

	int pausing;
	float pauseTimer;
	float menuDelay;

	// Win state controls
	int winner;

	glm::vec3 winPlayerPosition = glm::vec3(5.0f, 43.0f, 0.0f);
	glm::vec3 winCameraPosition = glm::vec3(20.0f, 53.0f, 30.0f);
	glm::vec3 winCameraForward = glm::vec3(0.01f, -2.0f, -5.0f);
	glm::vec3 cameraDefaultPosition = glm::vec3(23.0f, 90.0f, 40.0f);
	glm::vec2 cameraDefaultAngle = glm::vec2(3.14159012f, 5.3f);
	glm::vec3 cameraDefaultForward;
	float innerDefault = 0.78;
	float outerDefault = 0.81;
	float innerWin = 0.1;
	float outerWin = 0.2;
	std::shared_ptr<Menu> winScreen;

	float playerMoveLerp;	// to be replaced with A*
	glm::vec3 playerStartPosition; // to be replaced with A*
	float cameraMoveLerp;

	LUT contrastLUT;
	LUT sepiaLUT;
	LUT* currentLUT;
	LUT_MODE colorCorrection;

	float outlineWidth = 4.0;
	bool outlineToggle = true;
	LightingMode currentLightingMode = TOON;

	float windowWidth = 1920.0;
	float windowHeight = 1080.0;

	// Ready-up controls
	std::shared_ptr<Menu> countdown;
	float currentCountdown = 0.0f;
	int numActivePlayers;
	int numDeadPlayers;

	// Lighting Controls
	float innerCutOff = 0.1; // Spot Light Size
	float outerCutOff = 0.2;
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
	glm::vec4 bloomThreshold = glm::vec4(0.4f);
	int numBlurPasses = 4;
	bool bloomToggle = true;
	bool depthToggle = false;

	// For Toggling
	bool  ambientToggle = true;
	float ka = ambient; // Ambient Lighting

	bool  diffuseToggle = true;
	float kd = diffuse; // Diffuse Lighting

	bool  specularToggle = true;
	float ks = specular; // Specular Lighting

	bool  rimToggle = true;
	float kr = rim; // Rim Lighting

private:
	//static 
	static float maxFadeTime;

	// A few conversions to know
	const float degToRad = 3.14159f / 180.0f;
	const float radToDeg = 180.0f / 3.14159f;

	float aspectRatio = windowWidth / windowHeight;

	float A = 0.4062;
	float f = 0.0453;
	float S1 = 11.5;
};
#include "MainGameState.h"
#include <iostream>
#include <fstream>

#include "IL\ilut.h"
#include "gl\freeglut.h"

#include "RigidBody.h"
#include "InputManager.h"
#include "Texture.h"
#include "aStar.h"


void calculateCollisions();

bool LUT::load(std::string filePath)
{
	std::vector<glm::vec3> values;
	GLuint texture3D;

	std::ifstream stream(filePath);
	if (!stream)
	{
		std::cout << "error: no file at " << filePath << std::endl;
		return false;
	}

	std::string line;
	glm::vec3 value;
	int LUTsize;

	while (!stream.eof())
	{
		getline(stream, line);
		if (line.empty()) continue;

		if (sscanf(line.c_str(), "%f %f %f",
			&value.x, &value.y, &value.z) == 3)
		{
			values.push_back(value);
			continue;
		}

		if (sscanf(line.c_str(), "LUT_3D_SIZE %d", &LUTsize) == 1)
			size = LUTsize;
	}

	if (values.size() != pow(LUTsize, 3.0))
	{
		std::cout << "LUT size is incorrect" << std::endl;
		return false;
	}

	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &texture3D);
	glBindTexture(GL_TEXTURE_3D, texture3D);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUTsize, LUTsize, LUTsize, 0, GL_RGB, GL_FLOAT, &values[0]);

	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(GL_TEXTURE_3D);

	handle = texture3D;
	return true;
}

void LUT::bind(GLuint textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_3D, handle);
}

void LUT::unbind(GLuint textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_3D, 0);
}

float Game::maxFadeTime = 0.3f;

Game::Game
(
	std::map<std::string, std::shared_ptr<GameObject>>* _scene,
	std::map<std::string, std::shared_ptr<Player>>* _player,
	std::map<std::string, std::shared_ptr<Material>>* _materials,
	std::map<std::string, std::shared_ptr<Texture>>* _textures,
	std::vector<std::shared_ptr<GameObject>>* _obstacles,
	std::vector<std::shared_ptr<GameObject>>* _readyUpRings,
	std::shared_ptr<BombManager> _manager,
	std::shared_ptr<Menu> _countdown,
	std::map<std::string, Sound>* _soundTemplates,
	Pause* _pause, Score* _score, Camera* _camera
)

	: obstacles(_obstacles),
	readyUpRings(_readyUpRings),
	countdown(_countdown),
	soundTemplates(_soundTemplates)
{
	scene = _scene;
	players = _player;
	materials = _materials;
	textures = _textures;
	pause = _pause;
	score = _score;
	camera = _camera;
	pausing = 0;
	pauseTimer = 2.0f;
	bombManager = _manager;
	_camera->setPosition(cameraDefaultPosition);
	_camera->setAngle(cameraDefaultAngle.x, cameraDefaultAngle.y);
	_camera->update();
	cameraDefaultForward = _camera->getForward();
	initializeFrameBuffers();

	winScreen = std::make_shared<Menu>(textures->at("win"), 1, 1);
	winScreen->setMaterial(materials->at("menu"));

	toonRamp = ilutGLLoadImage("Assets/img/toonRamp.png");
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, toonRamp);


	contrastLUT.load("Assets/img/Test1.CUBE");
	sepiaLUT.load("Assets/img/Test2.CUBE");
	colorCorrection = LUT_SEPIA;
	currentLUT = &sepiaLUT;

	aStar.initializePathNodes();

	defaultPlayerPositions.push_back(glm::vec3(0.0f, 39.0f, -16.0f));	// top left
	defaultPlayerPositions.push_back(glm::vec3(45.0f, 39.5f, -25.0f));	// top right
	defaultPlayerPositions.push_back(glm::vec3(-12.0f, 39.5f, 10.0f));	// bottom left
	defaultPlayerPositions.push_back(glm::vec3(57.0f, 39.5f, 7.0f));	// bottom right

	// Initialize sounds
	m_gameMusic = Sound(soundTemplates->at("m_readyMusic"));
	m_gameMusic.setPosition(glm::vec3(23.0f, 55.0f, 10.0f));
	m_gameMusic.setVolume(musicVolume);

	m_gameTrack1 = Sound(soundTemplates->at("m_gameTrack1"));
	m_gameTrack2 = Sound(soundTemplates->at("m_gameTrack2"));
	m_gameTrack3 = Sound(soundTemplates->at("m_gameTrack3"));
	m_gameTrack1.setPosition(glm::vec3(23.0f, 55.0f, 10.0f));
	m_gameTrack2.setPosition(glm::vec3(23.0f, 55.0f, 10.0f));
	m_gameTrack3.setPosition(glm::vec3(23.0f, 55.0f, 10.0f));
	m_gameTrack1.setVolume(musicVolume);


	s_countDown = Sound(soundTemplates->at("s_countdown"));
	s_countDown.setPosition(glm::vec3(23.0f, 55.0f, 10.0f));
	s_countDown.setVolume(0.4);

	s_lightOn = Sound(soundTemplates->at("s_lightOn"));
	s_lightOn.setPosition(glm::vec3(23.0f, 55.0f, 10.0f));
	
	currentGameState = MAIN;
	changeState(READYUP);
}

void Game::setPaused(int a_paused)
{
	if (a_paused == 0)
	{
		m_isPaused = false;
		m_gameTrack1.resume();
		m_gameTrack2.resume();
		m_gameTrack3.resume();
	}
	else if (a_paused == 1)
	{
		m_isPaused = true;
		m_gameTrack1.pause();
		m_gameTrack2.pause();
		m_gameTrack3.pause();
	}
	else if (a_paused == -1)
	{
		m_isPaused = false;
		// Reset all players

		//resetPlayers();
		makePlayersInactive();
		bombManager->clearAllBombs();
		changeState(READYUP);
		menuDelay = 0.3f;

		m_gameTrack1.stop();
		m_gameTrack2.stop();
		m_gameTrack3.stop();
		//m_gameMusic.play();
	}
}

void Game::resetPlayers()
{
	numActivePlayers = 0;
	for (auto it : *players)
	{
		if (it.second->isActive())
		{
			it.second->reset(defaultPlayerPositions.at(it.second->getPlayerNum()) + glm::vec3(0.0f, 2.0f, 0.0f));
			numActivePlayers++;
		}
	}
	/*players->at("bombot1")->reset(glm::vec3(-12.0f, 39.5f, 10.0f));
	players->at("bombot2")->reset(glm::vec3(0.0f, 39.5f, -16.0f));
	players->at("bombot3")->reset(glm::vec3(40.0f, 39.5f, -25.0f));
	players->at("bombot4")->reset(glm::vec3(57.0f, 39.5f, 7.0f));*/
}

void Game::makePlayersInactive()
{
	for (auto it : *players)
	{
		it.second->reset(glm::vec3(-1000.0f));
		it.second->setActive(false);
	}
}

void Game::updateReadyPass(float dt)
{
	const int maxNumPlayers = 4;
	bool activeState[maxNumPlayers];
	bool readyState[maxNumPlayers];
	int counter = 0;
	bool startPressed = false;

	for (auto it : *players)
	{
		// If active and presses B, set inactive
		if (it.second->getController()->conButton(XINPUT_GAMEPAD_B) &&
			it.second->isActive())
		{
			it.second->setActive(false);
			it.second->setPosition(glm::vec3(-1000.0f));
		}

		// Set the player as active if A pressed and not active
		if ((it.second->getController()->conButton(XINPUT_GAMEPAD_A) ||
			(it.second->getController()->conButton(XINPUT_GAMEPAD_X))) &&
			!it.second->isActive())
		{
			it.second->setActive(true);
			it.second->setPosition(glm::vec3(15.0f * it.second->getPlayerNum(), 35.0f, 0.0f));
		}

		// Ready up checks
		activeState[counter] = it.second->isActive();
		readyState[counter] = it.second->isReady();
		counter++;

		if (it.second->getController()->conButton(XINPUT_GAMEPAD_X))
		{
			startPressed = true;
		}
	}

	int playerCounter = 0;
	bool allReady = true;
	// Check if enough players are in the game, and if they are ready
	for (unsigned int i = 0; i < maxNumPlayers; i++)
	{
		playerCounter += activeState[i];
		if (activeState[i])
		{
			if (!readyState[i])
				allReady = false;
		}
	}

	// If everyone's ready, start the game!
	if (allReady && startPressed && playerCounter > 1)
		changeState(PRE_COUNTDOWN);


}

void Game::update(float dt)
{
	if (currentGameState == READYUP)
	{
		for (auto it : *readyUpRings)
		{
			((readyUpRing*)it.get())->setReady(false);
		}
	}


	// Step through world simulation with Bullet
	RigidBody::systemUpdate(dt, 10);
	calculateCollisions();

	handleKeyboardInput();

	// Change the state if everyone is ready
	if (currentGameState == READYUP)
	{
		if (menuDelay > 0.0f)
			menuDelay -= dt;
		else
			updateReadyPass(dt);
	}

	if (currentGameState == PRE_COUNTDOWN)
	{
		if (fadeLerp < timeToFade)
		{
			fadeLerp += dt;
			if (fadeLerp >= timeToFade)
			{
				fadeLerp = timeToFade;
				m_gameMusic.stop();
			}

			deskLamp = glm::mix(lampReady, 0.0f, fadeLerp);
			roomLight = glm::mix(roomReady, 0.0f, fadeLerp);
			m_gameMusic.setVolume(glm::mix(musicVolume, 0.0f, fadeLerp));
		}
		else if (darkTime > 0.0f)
		{
			darkTime -= dt;
			if (darkTime <= 0.0f)
			{
				darkTime = 0.0f;
				s_lightOn.play();
				deskLamp = lampMain;
				roomLight = roomMain;

				for (auto it : *obstacles)
				{
					it->setPosition(it->getWorldPosition() + glm::vec3(0.0f, 50.0f, 0.0f));
				}
				
				resetPlayers();

				scene->at("table")->setTexture(textures->at("table"));
			}
		}
		else if (lightTime > 0.0f)
		{
			lightTime -= dt;
			if (lightTime <= 0.0f)
			{
				lightTime = 0.0f;
				changeState(COUNTDOWN);
			}
		}
	}

	// Update the countdown if in countdown
	if (currentGameState == COUNTDOWN)
	{
		currentCountdown -= dt;
		if (currentCountdown < 0.0f)
			changeState(MAIN);
		else if (currentCountdown < 1.0f)
			countdown->setSpot(glm::vec2(1.0, 0.0));
		else if (currentCountdown < 2.0f)
			countdown->setSpot(glm::vec2(0.0, 0.0));
		else if (currentCountdown < 3.0f)
			countdown->setSpot(glm::vec2(1.0, 1.0));
		else if (currentCountdown <= 4.0f)
			countdown->setSpot(glm::vec2(0.0, 1.0));
	}

	// Update all gameobjects
	updateScene(dt);

	pauseTimer += dt;

	// Update main state
	if (currentGameState == MAIN)
	{
		int count = 0;
		for (auto it : *players)
		{
			count++;
			if (it.second->getController()->conButton(XINPUT_GAMEPAD_START) && currentGameState == MAIN)
				pausing = count;
		}

		if (pausing > 0 && pauseTimer > 1.0)
		{
			switch (pausing)
			{
			case 0:
				break;
			case 1:
				setPaused(true);
				pause->setPaused(false);
				pause->active = players->at("bombot1")->getController();
				pauseTimer = 0;
				break;
			case 2:
				setPaused(true);
				pause->setPaused(false);
				pause->active = players->at("bombot2")->getController();
				pauseTimer = 0;
				break;
			case 3:
				setPaused(true);
				pause->setPaused(false);
				pause->active = players->at("bombot3")->getController();
				pauseTimer = 0;
				break;
			case 4:
				setPaused(true);
				pause->setPaused(false);
				pause->active = players->at("bombot4")->getController();
				pauseTimer = 0;
				break;
			}
			pausing = 0;
		}
		winner = deathCheck();
		if (winner > 0)
		{
			winner--;
			roomLight = roomMain + 0.1f;
			deskLamp = lampMain;
			changeState(WIN);
		}

		int playerDeathCounter = 0;
		for (auto it : *players)
		{
			if (it.second->isActive() && it.second->getHealth() == 0)
				playerDeathCounter++;
		}

		if (numDeadPlayers != playerDeathCounter)
		{
			numDeadPlayers = playerDeathCounter;

			if (playerDeathCounter > 1)
				m_gameTrack3.setVolume(musicVolume - 0.05);
			else if (playerDeathCounter > 0)
				m_gameTrack2.setVolume(musicVolume - 0.05);

			roomLight -= 0.1f;
			deskLamp += 0.05f;
		}
	}

	// Update win state
	if (currentGameState == WIN)
	{
		std::shared_ptr<Player> winPlayer = players->at("bombot" + std::to_string(winner));
		// If the player is going to die from a bomb, set their health to 1.
		if (winPlayer->getHealth() == 0)
			winPlayer->setHealth(1);

		if (!bombManager->isEmpty()) return;	// DANGER
		// When all the bombs have finished exploding, 
		// move player to designated position

		// Move the player to the space
		if (playerMoveLerp >= 0.0f && playerMoveLerp != 1.0f)
		{
			aStar.traversePath(winPlayer, dt * 2.0f);
			if (!aStar.traverse)	// If done traversing
			{
				playerMoveLerp = 1.0f;
				depthToggle = true;
			}
		}
		// Move the camera once the player has moved to the space
		else if (playerMoveLerp == 1.0f && cameraMoveLerp >= 0.0f && cameraMoveLerp != 1.0f)
		{
			cameraMoveLerp += dt;
			if (cameraMoveLerp > 1.0f)
			{
				cameraMoveLerp = 1.0f;
				winPlayer->playWin();

			}

			if (forwardLerp <= 1.0f) forwardLerp += dt / 2.0f;
			if (forwardLerp > 1.0f)
			{
				forwardLerp = 1.0f;

			}


			camera->setPosition(glm::mix(cameraDefaultPosition, winCameraPosition, cameraMoveLerp));
			camera->setForward(glm::mix(cameraDefaultForward, winCameraForward, forwardLerp));
		
			A = glm::mix(0.5062f, 0.4062f, forwardLerp);

			innerCutOff = glm::mix(innerDefault, innerWin, cameraMoveLerp);
			outerCutOff = glm::mix(outerDefault, outerWin, cameraMoveLerp);
		}
		// Let the game be ended
		else
		{
			if (winPlayer->getController()->conButton(XINPUT_GAMEPAD_A))
			{
				this->m_isPaused = true;
				changeState(READYUP);
				//players->at("bombot" + std::to_string(winner))->setAnim("idle");
				m_parent->getGameState("MainMenu")->setPaused(0);
			}
		}
	}

	Sound::sys.update();
}


void Game::draw()
{
	//camera->setPosition(glm::vec3(20.0f, 53.0f, 30.0f));
	//camera->setForward(glm::vec3(0.01, -2.0, -5.0));
	///////////////////////////// zero pass: shadows.
	//note up vector maybe wrong.

	shadowCamera.shadowCam(glm::vec3(30.0f, 70.0f, -10.0f), glm::vec3(-2.0f, -6.0f, 1.5f), glm::vec3(0.0, 1.0, 0.0), 0.01f, 100.01f);
	setMaterialForAllGameObjects("shadow");
	setMaterialForAllPlayerObjects("shadow");

	shadowMap.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);
	drawScene(&shadowCamera, &shadowCamera);
	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);


	///////////////////////////// First Pass: Outlines
	fboUnlit.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);
	if (outlineToggle)
	{
		glCullFace(GL_FRONT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glDepthMask(GL_FALSE);
		glLineWidth(outlineWidth);

		// Clear back buffer
		//FrameBufferObject::clearFrameBuffer(glm::vec4(0.8f, 0.8f, 0.8f, 0.0f));

		// Tell all game objects to use the outline shading material
		setMaterialForAllGameObjects("outline");
		setMaterialForAllPlayerObjects("outline");
		materials->at("outline")->shader->bind();
		drawScene(camera, &shadowCamera);

		glCullFace(GL_BACK); std::map<std::string, std::shared_ptr<Material>> materials;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glDepthMask(GL_TRUE);
	}

	///////////////////////////// Second Pass: Lighting
	switch (currentLightingMode)
	{
		// Our Default is Toon shading
	case TOON:
	{


		setMaterialForAllGameObjects("toon");
		setMaterialForAllPlayerObjects("toon");

		materials->at("toon")->shader->bind();

		// Set material properties
		materials->at("toon")->vec4Uniforms["u_lightPos"] = camera->getView() * lightPos;
		materials->at("toon")->vec4Uniforms["u_lightTwo"] = camera->getView() * lightTwo;


		materials->at("toon")->intUniforms["u_toonRamp"] = 5;
		materials->at("toon")->intUniforms["u_diffuseTex"] = 31;
		materials->at("toon")->intUniforms["u_specularTex"] = 30;

		shadowMap.bindTextureForSampling(0, GL_TEXTURE29);
		materials->at("toon")->intUniforms["u_shadowMap"] = 29;

		materials->at("toon")->vec4Uniforms["u_bokehControls"] = glm::vec4(A, f, S1, 1.0);
		materials->at("toon")->vec4Uniforms["u_controls"] = glm::vec4(ka, kd, ks, kr);
		materials->at("toon")->vec4Uniforms["u_dimmers"] = glm::vec4(deskLamp, roomLight, innerCutOff, outerCutOff);
		materials->at("toon")->vec4Uniforms["u_spotDir"] = glm::vec4(deskForward, 1.0);
		materials->at("toon")->vec4Uniforms["u_shine"] = glm::vec4(shininess);



		materials->at("toon")->sendUniforms();
		drawScene(camera, &shadowCamera);
		materials->at("toon")->shader->unbind();
	}
	break;
	// Just displays Textures
	case NOLIGHT:
	{
		setMaterialForAllGameObjects("noLighting");
		setMaterialForAllPlayerObjects("toon");

		// Set material properties
		materials->at("noLighting")->shader->bind();

		materials->at("noLighting")->intUniforms["u_diffuseTex"] = 31;
		materials->at("noLighting")->intUniforms["u_specularTex"] = 30;

		materials->at("noLighting")->sendUniforms();
		drawScene(camera, &shadowCamera);
		materials->at("noLighting")->shader->unbind();
	}
	break;
	}

	// Draw the debug (if on)
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(camera->getView(), camera->getProj());

	//unbind the frame buffer
	fboUnlit.unbindFrameBuffer(windowWidth, windowHeight);


	//copy and enable the frame buffer
	fboParticle.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	fboUnlit.copyBuffer(fboUnlit.getWidth(), fboUnlit.getHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST, fboParticle.getId());
	fboParticle.bindFrameBufferForDrawing();


	//draw the particles
	players->at("bombot1")->sparks;

	drawSparks(camera);
	drawSmoke(camera);

	//unbind the frame buffer
	fboParticle.unbindFrameBuffer(windowWidth, windowHeight);



	// Unbind scene FBO


	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Post Processing /////////////////////////////


	FrameBufferObject* finalFBO;
	if (bloomToggle)
	{
		bloomPass(fboUnlit, fboBloomed);
		if (depthToggle)
		{
			depthOfField(fboBloomed, fboWithBokeh);
			fboToScreen(fboWithBokeh);
			finalFBO = &fboWithBokeh;
		}
		else
		{
			fboToScreen(fboBloomed);
			finalFBO = &fboBloomed;
		}
	}
	else if (depthToggle)
	{
		depthOfField(fboUnlit, fboWithBokeh);
		fboToScreen(fboWithBokeh);
		finalFBO = &fboWithBokeh;
	}
	else
	{
		fboToScreen(fboUnlit);
		finalFBO = &fboUnlit;
	}

	if (particleToggle)
	{
		if (particleProcessing == true)
		{
			bloomPass(fboParticle, bloomParticle); // sets up fbo's for particle pass
												   //first is the texture we want to add, second is the particle texture we are using.
			particlePass(*finalFBO, bloomParticle, fboFinalParticle);
		}
		else
		{
			particlePass(*finalFBO, fboParticle, fboFinalParticle);
		}
		fboToScreen(fboFinalParticle);
	}

	// Color correction
	if (currentLUT)	// if pointing to a LUT
	{
		if (particleToggle)
		{
			colorCorrectionPass(fboFinalParticle, fboColorCorrection);
		}
		else
		{
			if (depthToggle)
				colorCorrectionPass(fboWithBokeh, fboColorCorrection);
			else if (bloomToggle)
				colorCorrectionPass(fboBloomed, fboColorCorrection);
			else
				colorCorrectionPass(fboUnlit, fboColorCorrection);
		}
		fboToScreen(fboColorCorrection);
	}

}

void Game::windowReshapeCallbackFunction(int w, int h)
{
	/* Update our Window Properties */
	windowWidth = w;
	windowHeight = h;

	camera->setRatio(windowHeight, windowWidth);
}



void Game::updateScene(float dt)
{
	// Move light in simple circular path
	static float ang = 1.0f;

	ang += dt; // comment out to pause light

	camera->update();


	// Update all game objects
	for (auto itr = scene->begin(); itr != scene->end(); ++itr)
	{
		auto gameobject = itr->second;

		// Remember: root nodes are responsible for updating all of its children
		// So we need to make sure to only invoke update() for the root nodes.
		// Otherwise some objects would get updated twice in a frame!
		if (gameobject->isRoot())
			gameobject->update(dt);
	}
	// Update all player objects
	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		auto player = itr->second;

		// Remember: root nodes are responsible for updating all of its children
		// So we need to make sure to only invoke update() for the root nodes.
		// Otherwise some objects would get updated twice in a frame!
		if (player->isRoot())
			player->update(dt, 
				currentGameState != PRE_COUNTDOWN && 
				currentGameState != COUNTDOWN && 
				currentGameState != WIN);
	}

	bombManager->update(dt);
	bombManager->checkIfExploded(*camera);
}

void Game::initializeFrameBuffers()
{
	fboUnlit.createFrameBuffer(windowWidth, windowHeight, 2, true);
	spunkMap.createFrameBuffer(windowWidth, windowHeight, 1, true);
	fboBright.createFrameBuffer(160, 120, 1, false);
	fboBlurA.createFrameBuffer(160, 120, 1, false);
	fboBlurB.createFrameBuffer(160, 120, 1, false);
	shadowMap.createFrameBuffer(windowWidth * 2, windowHeight * 2, 1, true);
	fboColorCorrection.createFrameBuffer(windowWidth, windowHeight, 1, false);

	fboBloomed.createFrameBuffer(windowWidth, windowHeight, 1, false);

	bokehHorizontalfbo.createFrameBuffer(windowWidth, windowHeight, 1, true);
	bokehAfbo.createFrameBuffer(windowWidth, windowHeight, 1, true);
	bokehBfbo.createFrameBuffer(windowWidth, windowHeight, 1, true);
	fboWithBokeh.createFrameBuffer(windowWidth, windowHeight, 1, true);

	fboParticle.createFrameBuffer(windowWidth, windowHeight, 1, true);
	fboFinal.createFrameBuffer(windowWidth, windowHeight, 1, false);
	bloomParticle.createFrameBuffer(windowWidth, windowHeight, 1, false);
	fboFinalParticle.createFrameBuffer(windowWidth, windowHeight, 1, false);
}

void Game::drawScene(Camera* _camera, Camera* _shadow)
{
	scene->begin()->second->getMaterial()->shader->sendUniformInt("skinning", 0);
	for (auto itr = scene->begin(); itr != scene->end(); ++itr)
	{
		auto gameobject = itr->second;

		if (gameobject->isRoot())
			gameobject->draw(*_camera, *_shadow);
	}

	players->begin()->second->getMaterial()->shader->sendUniformInt("skinning", 1);
	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		auto playersObject = itr->second;

		if (playersObject->isRoot())
			playersObject->draw(*_camera, *_shadow);
	}

	scene->begin()->second->getMaterial()->shader->sendUniformInt("skinning", 0);
	bombManager->draw(*_camera, *_shadow);

	// If in countdown, draw countdown
	if (currentGameState == COUNTDOWN)
	{
		countdown->draw();
	}
	else if (currentGameState == WIN && cameraMoveLerp == 1.0f)
	{
		winScreen->draw();
	}
}

void Game::drawSmoke(Camera* _camera)
{
	materials->at("particles")->shader->bind();
	materials->at("particles")->vec4Uniforms["u_lightPos"] = camera->getView() * lightPos;
	materials->at("particles")->vec4Uniforms["u_lightTwo"] = camera->getView() * lightTwo;

	materials->at("particles")->vec4Uniforms["u_controls"] = glm::vec4(ka, kd, ks, kr);
	materials->at("particles")->vec4Uniforms["u_dimmers"] = glm::vec4(deskLamp, roomLight, innerCutOff, outerCutOff);
	materials->at("particles")->vec4Uniforms["u_spotDir"] = glm::vec4(deskForward, 1.0);
	materials->at("particles")->vec4Uniforms["u_shine"] = glm::vec4(shininess);

	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		auto playersObject = itr->second;

		if (playersObject->isRoot())
			playersObject->drawSmoke(*_camera);
	}

	materials->at("particles")->shader->unbind();
}
void Game::drawSparks(Camera* _camera)
{
	materials->at("particles")->shader->bind();
	materials->at("particles")->vec4Uniforms["u_lightPos"] = camera->getView() * lightPos;
	materials->at("particles")->vec4Uniforms["u_lightTwo"] = camera->getView() * lightTwo;

	materials->at("particles")->vec4Uniforms["u_controls"] = glm::vec4(ka, kd, ks, kr);
	materials->at("particles")->vec4Uniforms["u_dimmers"] = glm::vec4(deskLamp, roomLight, innerCutOff, outerCutOff);
	materials->at("particles")->vec4Uniforms["u_spotDir"] = glm::vec4(deskForward, 1.0);
	materials->at("particles")->vec4Uniforms["u_shine"] = glm::vec4(shininess);

	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		auto playersObject = itr->second;

		if (playersObject->isRoot())
			playersObject->drawSparks(*_camera);
	}

	materials->at("particles")->shader->unbind();
}

int Game::deathCheck()
{
	int counter = 0;
	int winner = 0;
	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		auto gameobject = itr->second;

		if (gameobject->getHealth() > 0 && gameobject->isActive())
		{
			counter++;
			winner = gameobject->getPlayerNum() + 1;
		}
	}
	if (counter > 1)
		return 0; //returns 0 if no winner
	else
		return winner + 1;
}

void Game::setMaterialForAllGameObjects(std::string materialName)
{
	auto mat = materials->at(materialName);
	for (auto itr = scene->begin(); itr != scene->end(); ++itr)
	{
		itr->second->setMaterial(mat);
	}

	bombManager->setMaterialForAllBombs(mat);
}

void Game::setMaterialForAllPlayerObjects(std::string materialName)
{
	auto mat = materials->at(materialName);
	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		itr->second->setMaterial(mat);
	}
}


void Game::bloomPass(FrameBufferObject& input, FrameBufferObject& fboToDrawTo)
{
	//////////////////////////////////////////////////////////////////////////
	// BRIGHT PASS
	////////////////////////////////////////////////////////////////////////// 
	fboBright.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);
	input.bindTextureForSampling(0, GL_TEXTURE0);

	static auto brightMaterial = materials->at("bright");
	brightMaterial->shader->bind();

	brightMaterial->vec4Uniforms["u_bloomThreshold"] = bloomThreshold;
	brightMaterial->intUniforms["u_tex"] = 0;
	brightMaterial->mat4Uniforms["u_mvp"] = glm::mat4();

	brightMaterial->sendUniforms();

	glDrawArrays(GL_POINTS, 0, 1);

	//////////////////////////////////////////////////////////////////////////
	// BLUR BRIGHT PASS HERE
	////////////////////////////////////////////////////////////////////////// 
	fboBlurA.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);
	fboBright.bindTextureForSampling(0, GL_TEXTURE0);

	static auto blurMaterial = materials->at("blur");

	// Corner, mid, centre, sigma
	// For 3x3 kernel
	static glm::vec4 kernelWeights(0.077847f, 0.123317f, 0.195346f, 1.0f);
	blurMaterial->shader->bind();

	blurMaterial->intUniforms["u_tex"] = 0;
	blurMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
	blurMaterial->vec4Uniforms["u_texelSize"] = glm::vec4(1.0 / 160, 1.0 / 120, 0.0, 0.0);
	blurMaterial->vec4Uniforms["u_kernel"] = kernelWeights;

	blurMaterial->sendUniforms();

	glDrawArrays(GL_POINTS, 0, 1);

	static int numBlurPasses = 4;
	for (int i = 0; i < numBlurPasses; i++)
	{
		if (i % 2 == 0)
		{
			fboBlurB.bindFrameBufferForDrawing();
			fboBlurA.bindTextureForSampling(0, GL_TEXTURE0);
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else
		{
			fboBlurA.bindFrameBufferForDrawing();
			fboBlurB.bindTextureForSampling(0, GL_TEXTURE0);
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// FINAL BLOOM COMP PASS HERE
	////////////////////////////////////////////////////////////////////////// 

	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
	fboToDrawTo.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);

	input.bindTextureForSampling(0, GL_TEXTURE0);
	fboBlurA.bindTextureForSampling(0, GL_TEXTURE1);

	static auto sunlitMaterial = materials->at("bloom");
	sunlitMaterial->shader->bind();
	sunlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
	sunlitMaterial->shader->sendUniformInt("u_scene", 0);
	sunlitMaterial->shader->sendUniformInt("u_bright", 1);

	sunlitMaterial->sendUniforms();

	glDrawArrays(GL_POINTS, 0, 1);

	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
}

void Game::fboToScreen(FrameBufferObject& input)
{
	input.bindTextureForSampling(0, GL_TEXTURE0);

	static auto unlitMaterial = materials->at("unlitTexture");
	unlitMaterial->shader->bind();
	unlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
	unlitMaterial->shader->sendUniformInt("u_tex", 0);
	unlitMaterial->sendUniforms();

	// Draw a full screen quad using the geometry shader
	glDrawArrays(GL_POINTS, 0, 1);
}

// This function is fully implemented!
void Game::bokehPass(FrameBufferObject& fboToSample, FrameBufferObject& fboToDrawTo, float filterAngleDeg)
{
	fboToDrawTo.bindFrameBufferForDrawing();
	fboToDrawTo.clearFrameBuffer(clearColor);

	// Bind FBO texture to texture unit zero
	/// TODO: bind scene fbo's depth texture to texture unit 0
	fboUnlit.bindDepthTextureForSampling(GL_TEXTURE0);

	fboToSample.bindTextureForSampling(0, GL_TEXTURE1);
	fboUnlit.bindTextureForSampling(1, GL_TEXTURE2);

	static auto bokehMaterial = materials->at("bokeh");

	// Tell opengl which shader we want it to use
	bokehMaterial->shader->bind();

	// set material uniforms
	bokehMaterial->intUniforms["u_depth"] = 0;
	bokehMaterial->intUniforms["u_tex"] = 1;
	bokehMaterial->intUniforms["u_texD"] = 2;


	bokehMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
	bokehMaterial->vec4Uniforms["u_cameraParams"] = glm::vec4(filterAngleDeg * degToRad, aspectRatio, 0.0f, 0.0f);

	// Send uniform variables to GPU
	bokehMaterial->sendUniforms();

	// Draw fullscreen quad
	glDrawArrays(GL_POINTS, 0, 1);
}

void Game::particlePass(FrameBufferObject& fboToSample, FrameBufferObject& fboBlend, FrameBufferObject& fboToDrawTo)
{
	fboToDrawTo.bindFrameBufferForDrawing();
	fboToDrawTo.clearFrameBuffer(clearColor);

	fboToSample.bindTextureForSampling(0, GL_TEXTURE0);
	fboBlend.bindTextureForSampling(0, GL_TEXTURE1);
	fboParticle.bindTextureForSampling(0, GL_TEXTURE2);

	static auto combine = materials->at("combine");
	combine->shader->bind();
	combine->intUniforms["u_particle"] = 1;
	combine->intUniforms["u_tex"] = 0;
	combine->intUniforms["u_activater"] = 2;
	combine->mat4Uniforms["u_mvp"] = glm::mat4();

	combine->sendUniforms();

	// Draw fullscreen quad
	glDrawArrays(GL_POINTS, 0, 1);
	fboToDrawTo.unbindFrameBuffer(windowWidth, windowHeight);
}

void Game::depthOfField(FrameBufferObject& input, FrameBufferObject& output)
{
	// Bokeh Blur Passes
	bokehPass(input, bokehHorizontalfbo, 0.0);
	bokehPass(bokehHorizontalfbo, bokehAfbo, 120.0);
	bokehPass(bokehHorizontalfbo, bokehBfbo, -120.0);

	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
	output.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);

	static auto bokehCompositeMaterial = materials->at("bokehComp");

	// Tell opengl which shader we want it to use
	bokehCompositeMaterial->shader->bind();


	bokehAfbo.bindTextureForSampling(0, GL_TEXTURE0);
	bokehBfbo.bindTextureForSampling(0, GL_TEXTURE1);

	// TODO: Set sampler values for the two textures
	bokehCompositeMaterial->shader->sendUniformInt("u_bokehA", 0);
	bokehCompositeMaterial->shader->sendUniformInt("u_bokehB", 1);
	bokehCompositeMaterial->mat4Uniforms["u_mvp"] = glm::mat4();

	// Send uniform varibles to GPU
	bokehCompositeMaterial->sendUniforms();

	// Draw fullscreen quad
	glDrawArrays(GL_POINTS, 0, 1);
	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
}

void Game::colorCorrectionPass(FrameBufferObject& fboIn, FrameBufferObject& fboOut)
{
	fboOut.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);
	fboIn.bindTextureForSampling(0, GL_TEXTURE0);

	static auto colorMaterial = materials->at("colorCorrection");

	//// Bind the LUT
	currentLUT->bind(GL_TEXTURE6);

	colorMaterial->shader->bind();
	colorMaterial->shader->sendUniformInt("u_tex", 0);
	colorMaterial->shader->sendUniformInt("u_lookup", 6);
	colorMaterial->shader->sendUniformFloat("u_mixAmount", 0.5f);
	colorMaterial->shader->sendUniformFloat("u_lookupSize", currentLUT->getSize());
	colorMaterial->sendUniforms();

	// Draw a full screen quad using the geometry shader
	glDrawArrays(GL_POINTS, 0, 1);
	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);

	contrastLUT.unbind(GL_TEXTURE6);
}


void Game::handleKeyboardInputShaders()
{
	// Lighting Mode
	if (KEYBOARD_INPUT->CheckPressEvent('2'))
	{
		currentLightingMode = NOLIGHT;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('1'))
	{
		currentLightingMode = TOON;
	}

	//particle stuff
	if (KEYBOARD_INPUT->CheckPressEvent('3'))
	{
		particleToggle = !particleToggle;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('4'))
	{
		particleProcessing = !particleProcessing;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('5'))
	{
		for (auto it : *players)
		{
			it.second->smoke.lightingToggle = 0.0f;
			it.second->smoke.mixer = 1.0;

			it.second->sparks.lightingToggle = 0.0f;
			it.second->sparks.mixer = 1.0;
		}
	}
	if (KEYBOARD_INPUT->CheckPressEvent('6'))
	{
		for (auto it : *players)
		{
			it.second->smoke.lightingToggle = 1.0f;
			it.second->smoke.mixer = 0.5;

			it.second->sparks.lightingToggle = 1.0f;
			it.second->sparks.mixer = 0.2;
		}
	}

	// Toggle Outlines
	if (KEYBOARD_INPUT->CheckPressEvent('7'))
	{
		outlineToggle = !outlineToggle;
	}
	// Toggle Bloom
	if (KEYBOARD_INPUT->CheckPressEvent('8'))
	{
		bloomToggle = !bloomToggle;
	}
	// Toggle LUT
	if (KEYBOARD_INPUT->CheckPressEvent('9'))
	{
		if (colorCorrection == LUT_CONTRAST)
		{
			changeColorCorrection(LUT_OFF);
		}
		else
		{
			changeColorCorrection(LUT_CONTRAST);
		}
	}
	// Toggle LUT
	if (KEYBOARD_INPUT->CheckPressEvent('0'))
	{
		if (colorCorrection == LUT_SEPIA)
		{
			changeColorCorrection(LUT_OFF);
		}
		else
		{
			changeColorCorrection(LUT_SEPIA);
		}
	}

	// Controls "Dimmers" for lights
	if (KEYBOARD_INPUT->CheckPressEvent('='))
	{
		deskLamp += 0.1;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('-'))
	{
		if (deskLamp > 0)
			deskLamp -= 0.1;
	}
	if (KEYBOARD_INPUT->CheckPressEvent(']'))
	{
		roomLight += 0.1;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('['))
	{
		if (roomLight > 0)
			roomLight -= 0.1;
	}

	// Bokeh Controls
	if (KEYBOARD_INPUT->CheckPressEvent('n'))
	{
		A -= 0.05;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('m'))
	{
		A += 0.05;
	}

	if (KEYBOARD_INPUT->CheckPressEvent(','))
	{
		f -= 0.005;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('.'))
	{
		f += 0.005;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('l'))
	{
		S1 -= 0.5;
	}
	if (KEYBOARD_INPUT->CheckPressEvent(';'))
	{
		S1 += 0.5;
	}


	// Toggles for each lighting component
	if (KEYBOARD_INPUT->CheckPressEvent('z')) // ambient
	{
		if (ambientToggle)
		{
			ka = 0.0f;
			ambientToggle = false;
		}
		else
		{
			ka = ambient;
			ambientToggle = true;
		}
	}
	if (KEYBOARD_INPUT->CheckPressEvent('x')) // diffuse
	{
		if (diffuseToggle)
		{
			kd = 0.0f;
			diffuseToggle = false;
		}
		else
		{
			kd = diffuse;
			diffuseToggle = true;
		}
	}
	if (KEYBOARD_INPUT->CheckPressEvent('c')) // specular
	{
		if (specularToggle)
		{
			ks = 0.0f;
			specularToggle = false;
		}
		else
		{
			ks = specular;
			specularToggle = true;
		}
	}
	if (KEYBOARD_INPUT->CheckPressEvent('v')) // rim
	{
		if (rimToggle)
		{
			kr = 0.0f;
			rimToggle = false;
		}
		else
		{
			kr = rim;
			rimToggle = true;
		}
	}
}


void Game::handleKeyboardInput()
{
	if (KEYBOARD_INPUT->CheckPressEvent(27))
	{
		glutLeaveMainLoop();
	}

	// Use the E key to set the debug draw
	if (KEYBOARD_INPUT->CheckPressEvent('e') || KEYBOARD_INPUT->CheckPressEvent('E'))
	{
		RigidBody::setDebugDraw(!RigidBody::isDrawingDebug());
	}

	// Move the camera
	if (KEYBOARD_INPUT->CheckPressEvent('w') || KEYBOARD_INPUT->CheckPressEvent('W'))
	{
		camera->moveForward();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('s') || KEYBOARD_INPUT->CheckPressEvent('S'))
	{
		camera->moveBackward();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('d') || KEYBOARD_INPUT->CheckPressEvent('D'))
	{
		camera->moveRight();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('a') || KEYBOARD_INPUT->CheckPressEvent('A'))
	{
		camera->moveLeft();
	}
	//if (KEYBOARD_INPUT->CheckPressEvent('j') || KEYBOARD_INPUT->CheckPressEvent('J'))
	//{
	//	gameobjects["bombot2"]->setPosition(
	//		gameobjects["bombot2"]->getWorldPosition() + glm::vec3(10.0, 0.0, 0.0));
	//}
	//if (KEYBOARD_INPUT->CheckPressEvent('k') || KEYBOARD_INPUT->CheckPressEvent('K'))
	//{
	//	gameobjects["bombot1"]->setScale(gameobjects["bombot1"]->getScale() + 0.1f);
	//}
	//if (KEYBOARD_INPUT->CheckPressEvent('l') || KEYBOARD_INPUT->CheckPressEvent('L'))
	//{
	//	gameobjects["table"]->setScale(gameobjects["table"]->getScale() - 0.1f);
	//}
	if (KEYBOARD_INPUT->CheckPressEvent('h') || KEYBOARD_INPUT->CheckPressEvent('H'))
	{
		changeState(PRE_COUNTDOWN);
	}
	if (KEYBOARD_INPUT->CheckPressEvent('j') || KEYBOARD_INPUT->CheckPressEvent('J'))
	{
		players->at("bombot2")->setActive(true);
		players->at("bombot2")->setPosition(glm::vec3(15.0f * players->at("bombot2")->getPlayerNum(), 35.0f, 0.0f));
	}
	if (KEYBOARD_INPUT->CheckPressEvent('k') || KEYBOARD_INPUT->CheckPressEvent('J'))
	{
		players->at("bombot3")->setActive(true);
		players->at("bombot3")->setPosition(glm::vec3(15.0f * players->at("bombot3")->getPlayerNum(), 35.0f, 0.0f));
	}
	if (KEYBOARD_INPUT->CheckPressEvent('l') || KEYBOARD_INPUT->CheckPressEvent('J'))
	{
		players->at("bombot4")->setActive(true);
		players->at("bombot4")->setPosition(glm::vec3(15.0f * players->at("bombot4")->getPlayerNum(), 35.0f, 0.0f));
	}




	handleKeyboardInputShaders();


	// Clear the keyboard input
	KEYBOARD_INPUT->WipeEventList();
}

void Game::changeState(Game::GAME_STATE newState)
{
	// Don't apply stuff if they are the same
	if (currentGameState == newState)	return;

	currentGameState = newState;
	switch (currentGameState)
	{
	case Game::READYUP:
		depthToggle = false;

		// reset the camera
		camera->setPosition(cameraDefaultPosition);
		camera->setAngle(cameraDefaultAngle.x, cameraDefaultAngle.y);
		innerCutOff = innerDefault;
		outerCutOff = outerDefault;

		deskLamp = lampReady;
		roomLight = roomReady;

		// Reset the sounds
		m_gameTrack1.stop();
		m_gameTrack2.stop();
		m_gameTrack3.stop();
		if (!m_gameMusic.isPlaying()) m_gameMusic.play();
		m_gameMusic.setVolume(musicVolume);

		for (auto it : *obstacles)
		{
			it->setPosition(it->getWorldPosition() + glm::vec3(0.0f, -50.0f, 0.0f));
		}
		for (auto it : *readyUpRings)
		{
			it->setPosition(it->getWorldPosition() + glm::vec3(0.0f, 50.0f, 0.0f));
		}

		// Set the tables texture
		scene->at("table")->setTexture(textures->at("readyTable"));
		break;

	case Game::PRE_COUNTDOWN:
		fadeLerp = 0.0f;
		darkTime = 2.0f;
		lightTime = 1.0f;
		bombManager->clearAllBombs();

		for (auto it : *readyUpRings)
		{
			it->setPosition(it->getWorldPosition() + glm::vec3(0.0f, -50.0f, 0.0f));
		}

		break;

	case Game::COUNTDOWN:
		s_countDown.play();

		currentCountdown = 4.0f;
		
		break;

	case Game::WIN:
		playerMoveLerp = 0.0f;
		cameraMoveLerp = 0.0f;
		forwardLerp = 0.0f;
		playerStartPosition = players->at("bombot" + std::to_string(winner))->getWorldPosition();

		aStar.traverse = true;
		aStar.newTraverse = true;
		aStar.findPath(players->at("bombot" + std::to_string(winner)));
		break;

	case Game::MAIN:
		m_gameTrack1.play();
		m_gameTrack2.play();
		m_gameTrack3.play();

		m_gameTrack2.setVolume(0.0f);
		m_gameTrack3.setVolume(0.0f);
		numTracksPlaying = 1;
		numDeadPlayers = 0;
		break;

	default:
		break;
	}
}

void Game::changeColorCorrection(LUT_MODE mode)
{
	if (colorCorrection == mode) return;
	colorCorrection = mode;

	switch (colorCorrection)
	{
	case Game::LUT_OFF:
		currentLUT = nullptr;
		break;
	case Game::LUT_CONTRAST:
		currentLUT = &contrastLUT;
		break;
	case Game::LUT_SEPIA:
		currentLUT = &sepiaLUT;
		break;
	default:
		break;
	}
}
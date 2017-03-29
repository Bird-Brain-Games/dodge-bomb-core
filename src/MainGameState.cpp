#include "MainGameState.h"
#include "RigidBody.h"
#include "IL\ilut.h"
#include "InputManager.h"
#include "gl\freeglut.h"
#include <fstream>
#include <iostream>


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

Game::Game
(
	std::map<std::string, std::shared_ptr<GameObject>>* _scene,
	std::map<std::string, std::shared_ptr<Player>>* _player,
	std::map<std::string, std::shared_ptr<Material>>* _materials,
	std::vector<std::shared_ptr<GameObject>>* _obstacles,
	std::shared_ptr<BombManager> _manager,
	Pause* _pause, Score* _score, Camera* _camera
)

	: obstacles(_obstacles)
{
	scene = _scene;
	players = _player;
	materials = _materials;
	pause = _pause;
	score = _score;
	camera = _camera;
	pausing = 0;
	pauseTimer = 2.0f;
	bombManager = _manager;
	_camera->setPosition(glm::vec3(23.0f, 90.0f, 40.0f));
	_camera->setAngle(3.14159012f, 5.3f);
	_camera->update();
	initializeFrameBuffers();
	currentGameState = MAIN;
	changeState(READYUP);

	toonRamp = ilutGLLoadImage("Assets/img/toonRamp.png");
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, toonRamp);
	
	contrastLUT.load("Assets/img/Test1.CUBE");
	sepiaLUT.load("Assets/img/Test2.CUBE");
	colorCorrection = LUT_OFF;

}

void Game::setPaused(int a_paused)
{
	if (a_paused == 0)
	{
		float count = 0.0f;
		m_isPaused = false;
	}
	else if (a_paused == 1)
		m_isPaused = true;
	else if (a_paused == -1)
	{
		m_isPaused = false;
		// Reset all players

		float count = 0.0f;
		resetPlayers();
		bombManager->clearAllBombs();
		currentGameState = READYUP;
	}
}

void Game::resetPlayers()
{
	players->at("bombot1")->reset(glm::vec3(-12.0f, 39.5f, 10.0f));
	players->at("bombot2")->reset(glm::vec3(0.0f, 39.5f, -16.0f));
	players->at("bombot3")->reset(glm::vec3(40.0f, 39.5f, -25.0f));
	players->at("bombot4")->reset(glm::vec3(57.0f, 39.5f, 7.0f));
}

void Game::update(float dt)
{


	// Step through world simulation with Bullet
	RigidBody::systemUpdate(dt, 10);
	calculateCollisions();

	handleKeyboardInput();

	// Change the state if everyone is ready
	if (currentGameState != MAIN)
	{
		bool allReady = true;
		bool startPressed = false;
		for (auto it : *players)
		{
			// Ready up
			if (!it.second->getController()->conButton(XINPUT_GAMEPAD_X))
			{
				allReady = false;
			}
			if (it.second->getController()->conButton(XINPUT_GAMEPAD_START))
			{
				startPressed = true;
			}
		}
		if (allReady && startPressed)
			changeState(MAIN);
	}

	// Update all gameobjects
	updateScene(dt);

	pauseTimer += dt;



	int count = 0;
	for (auto it : *players)
	{

		if (it.second->getController()->conButton(XINPUT_GAMEPAD_START))
			pausing = count;
		count++;
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
	}
	int winner = deathCheck();
	if (winner > 0)
	{
		this->m_isPaused = true;
		changeState(READYUP);
		score->active = players->at("bombot1")->getController();
		m_parent->getGameState("score")->setPaused(winner);
	}

}


void Game::draw()
{

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
		glLineWidth(outlineWidth);

		// Clear back buffer
		FrameBufferObject::clearFrameBuffer(glm::vec4(0.8f, 0.8f, 0.8f, 0.0f));

		// Tell all game objects to use the outline shading material
		setMaterialForAllGameObjects("outline");
		setMaterialForAllPlayerObjects("outline");
		materials->at("outline")->shader->bind();
		drawScene(camera, &shadowCamera);

		glCullFace(GL_BACK); std::map<std::string, std::shared_ptr<Material>> materials;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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


	fboUnlit.bindFrameBufferForDrawing();
	drawScene(camera, &shadowCamera);
	

	// Draw the debug (if on)
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(camera->getView(), camera->getProj());

	// Unbind scene FBO
	fboUnlit.unbindFrameBuffer(windowWidth, windowHeight);
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Post Processing

	if (bloomToggle)
	{

		//if (colorCorrection != LUT_OFF)
		//{
		//	fboColorCorrection.bindFrameBufferForDrawing();
		//	FrameBufferObject::clearFrameBuffer(clearColor);
		//}
		//else
		//{
		//	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
		//	FrameBufferObject::clearFrameBuffer(glm::vec4(1, 0, 0, 1));
		//	fboUnlit.bindTextureForSampling(0, GL_TEXTURE1);
		//}

		brightPass(); // Output FBO: fboBright
		blurBrightPass(); // Output FBO: fboBlurB

		FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
		FrameBufferObject::clearFrameBuffer(glm::vec4(0, 0, 1, 1));

		//FrameBufferObject::clearFrameBuffer(clearColor);
		fboUnlit.bindTextureForSampling(0, GL_TEXTURE0);
		fboBlurA.bindTextureForSampling(0, GL_TEXTURE1);

		static auto sunlitMaterial = materials->at("bloom");
		sunlitMaterial->shader->bind();
		sunlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
		sunlitMaterial->shader->sendUniformInt("u_scene", 0);
		sunlitMaterial->shader->sendUniformInt("u_bright", 1);
		sunlitMaterial->sendUniforms();

		// Draw a full screen quad using the geometry shader
		glDrawArrays(GL_POINTS, 0, 1);
	}
	else
	{
		//if (colorCorrection != LUT_OFF)
		//{
		//	//colorCorrectionPass(fboUnlit, fboColorCorrection);

		//	//FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
		//	fboColorCorrection.bindFrameBufferForDrawing();
		//	FrameBufferObject::clearFrameBuffer(clearColor);
		//	//fboColorCorrection.bindTextureForSampling(0, GL_TEXTURE0);
		//}
		//else
		//{
		//	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
		//	FrameBufferObject::clearFrameBuffer(glm::vec4(1, 0, 0, 1));
		//	//fboUnlit.bindTextureForSampling(0, GL_TEXTURE0);
		//}


		fboUnlit.bindTextureForSampling(0, GL_TEXTURE0);

		static auto unlitMaterial = materials->at("unlitTexture");
		unlitMaterial->shader->bind();
		unlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
		unlitMaterial->shader->sendUniformInt("u_tex", 0);
		unlitMaterial->sendUniforms();

		// Draw a full screen quad using the geometry shader
		glDrawArrays(GL_POINTS, 0, 1);
	}

	/////////////////////////////////////////////////////////////////////////////
	//////////////////////////	Color Correction
	//static auto colorMaterial = materials->at("colorCorrection");
	//switch (colorCorrection)
	//{
	//case Game::LUT_OFF:
	//	break;

	//case Game::LUT_CONTRAST:
	//	// Draw the corrected FBO to the screen

	//	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
	//	FrameBufferObject::clearFrameBuffer(glm::vec4(1, 0, 0, 1));

	//	fboColorCorrection.bindTextureForSampling(0, GL_TEXTURE0);

	//	// Bind the LUT
	//	contrastLUT.bind(GL_TEXTURE6);

	//	colorMaterial->shader->bind();
	//	colorMaterial->shader->sendUniformInt("u_tex", 0);
	//	colorMaterial->shader->sendUniformInt("u_lookup", 6);
	//	colorMaterial->shader->sendUniformFloat("u_mixAmount", 1.0f);
	//	colorMaterial->shader->sendUniformFloat("u_lookupSize", contrastLUT.getSize());
	//	colorMaterial->sendUniforms();

	//	// Draw a full screen quad using the geometry shader
	//	glDrawArrays(GL_POINTS, 0, 1);

	//	contrastLUT.unbind(GL_TEXTURE6);

	//	break;

	//case Game::LUT_SEPIA:
	//	FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
	//	FrameBufferObject::clearFrameBuffer(glm::vec4(1, 0, 0, 1));

	//	fboColorCorrection.bindTextureForSampling(0, GL_TEXTURE0);

	//	// Bind the LUT
	//	sepiaLUT.bind(GL_TEXTURE6);

	//	colorMaterial->shader->bind();
	//	colorMaterial->shader->sendUniformInt("u_tex", 0);
	//	colorMaterial->shader->sendUniformInt("u_lookup", 6);
	//	colorMaterial->shader->sendUniformFloat("u_mixAmount", 1.0f);
	//	colorMaterial->shader->sendUniformFloat("u_lookupSize", sepiaLUT.getSize());
	//	colorMaterial->sendUniforms();

	//	// Draw a full screen quad using the geometry shader
	//	glDrawArrays(GL_POINTS, 0, 1);

	//	sepiaLUT.unbind(GL_TEXTURE6);

	//	break;

	//default:
	//	break;
	//}

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
			player->update(dt);
	}

	bombManager->update(dt);
	bombManager->checkIfExploded(*camera);
}

void Game::initializeFrameBuffers()
{
	fboUnlit.createFrameBuffer(windowWidth, windowHeight, 2, true);
	fboBright.createFrameBuffer(160, 120, 1, false);
	fboBlurA.createFrameBuffer(160, 120, 1, false);
	fboBlurB.createFrameBuffer(160, 120, 1, false);
	shadowMap.createFrameBuffer(windowWidth * 2, windowHeight * 2, 1, true);
	fboColorCorrection.createFrameBuffer(windowWidth, windowHeight, 1, false);
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
}

int Game::deathCheck()
{
	int counter = 0;
	int winner = 0;
	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		auto gameobject = itr->second;

		if (gameobject->getHealth() > 0)
		{
			counter++;
			winner = gameobject->getPlayerNum() + 1;
		}
	}
	if (counter > 1)
		return false; //returns 0 if no winner
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


// Only takes the highest brightness from the FBO
void Game::brightPass()
{
	fboBright.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0f));
	fboUnlit.bindTextureForSampling(0, GL_TEXTURE0);

	static auto brightMaterial = materials->at("bright");
	brightMaterial->shader->bind();


	brightMaterial->vec4Uniforms["u_bloomThreshold"] = bloomThreshold;
	brightMaterial->intUniforms["u_tex"] = 0;
	brightMaterial->mat4Uniforms["u_mvp"] = glm::mat4();

	brightMaterial->sendUniforms();

	glDrawArrays(GL_POINTS, 0, 1);
}

void Game::blurBrightPass()
{
	//////////////////////////////////////////////////////////////////////////
	// BLUR BRIGHT PASS HERE
	////////////////////////////////////////////////////////////////////////// 
	fboBlurA.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0f));
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

	static int numBlurPasses = 10;
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
}

void Game::colorCorrectionPass(FrameBufferObject fboIn, FrameBufferObject fboOut)
{
	fboOut.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(glm::vec4(clearColor));
	fboIn.bindTextureForSampling(0, GL_TEXTURE0);

	static auto colorMaterial = materials->at("colorCorrection");

	//// Bind the LUT
	contrastLUT.bind(GL_TEXTURE6);

	colorMaterial->shader->bind();
	colorMaterial->shader->sendUniformInt("u_tex", 0);
	colorMaterial->shader->sendUniformInt("u_lookup", 6);
	colorMaterial->shader->sendUniformFloat("u_mixAmount", 1.0f);
	colorMaterial->shader->sendUniformFloat("u_lookupSize", contrastLUT.getSize());
	colorMaterial->sendUniforms();

	// Draw a full screen quad using the geometry shader
	glDrawArrays(GL_POINTS, 0, 1);

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


	// Toggle Outlines
	if (KEYBOARD_INPUT->CheckPressEvent('5'))
	{
		outlineToggle = !outlineToggle;
	}
	// Toggle Bloom
	if (KEYBOARD_INPUT->CheckPressEvent('6'))
	{
		bloomToggle = !bloomToggle;
	}
	// Toggle LUT
	if (KEYBOARD_INPUT->CheckPressEvent('7'))
	{
		if (colorCorrection == LUT_CONTRAST)
		{
			colorCorrection = LUT_OFF;
		}
		else
		{
			colorCorrection = LUT_CONTRAST;
		}
	}
	// Toggle LUT
	if (KEYBOARD_INPUT->CheckPressEvent('8'))
	{
		if (colorCorrection == LUT_SEPIA)
		{
			colorCorrection = LUT_OFF;
		}
		else
		{
			colorCorrection = LUT_SEPIA;
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
		camera->shakeScreen(1.0f);
	}
	// Reset all players




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
		for (auto it : *obstacles)
		{
			it->setPosition(it->getWorldPosition() + glm::vec3(0.0f, -50.0f, 0.0f));
		}
		break;
	case Game::MAIN:
		for (auto it : *obstacles)
		{
			it->setPosition(it->getWorldPosition() + glm::vec3(0.0f, 50.0f, 0.0f));
		}
		resetPlayers();
		break;
	default:
		break;
	}
}
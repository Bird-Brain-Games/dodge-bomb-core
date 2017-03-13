#include "states.h"


#include "RigidBody.h"

glm::vec4 lightPos;
glm::vec4 lightTwo;

// Cameras


Controller menus1(0);
Controller menus2(1);
Controller menus3(2);
Controller menus4(3);

void calculateCollisions();

void MainMenu::update(float dt)
{
	if (menus1.conButton(XINPUT_GAMEPAD_A))
	{
		setPaused(true);
		m_parent->getGameState("game")->setPaused(-1); // resets the players by passing in two.
	}
	Coords rStick = menus1.getRightStick();
	if (rStick.x > 0 && position < 3)
	{
		position++;
		atlas->setSpot(position, 0);
	}
	else if (rStick.x < 0 && position > 0)
	{
		position--;
		atlas->setSpot(position, 0);
	}
	atlas->incSpotR();
}
//returns a number based on who pressed a key. returns 0 if no key pressed
int allPress(int button)
{
	if (menus1.conButton(button))
	{
		return 1;
	}
	else if (menus2.conButton(button))
	{
		return 2;
	}
	else if (menus3.conButton(button))
	{
		return 3;
	}
	else if (menus4.conButton(button))
	{
		return 4;
	}
	else
		return 0;
}

void MainMenu::draw()
{
	atlas->draw();
}

MainMenu::MainMenu(std::shared_ptr<Menu> _atlas)
{
	atlas = _atlas;
	position = 0;
	frame = 0;
}

void MainMenu::setPaused(bool _state)
{
	m_isPaused = _state;
	if (!_state)
		atlas->setSpot(glm::vec2(0, 0));
}
Game::Game(std::map<std::string, std::shared_ptr<GameObject>>* _scene,
	std::map<std::string, std::shared_ptr<Player>>* _player,
	std::map<std::string, std::shared_ptr<Material>>* _materials,
	std::shared_ptr<BombManager> _manager,
	Pause* _pause, Score* _score, Camera* _camera)
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

	toonRamp = ilutGLLoadImage("../../Assets/img/toonRamp.png");
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, toonRamp);

}

void Game::setPaused(int a_paused)
{
	if (a_paused == 0)
	{
		float count = 0.0f;
		for (auto it : *players)
		{
			it.second->reset(glm::vec3(20.0f * count, 40.0f, 0.0f));
			count++;
		}
		m_isPaused = false;
	}
	else if (a_paused == 1)
		m_isPaused = true;
	else if (a_paused == -1)
	{
		m_isPaused = false;
		// Reset all players
		if (KEYBOARD_INPUT->CheckPressEvent('r') || KEYBOARD_INPUT->CheckPressEvent('R'))
		{
			float count = 0.0f;
			for (auto it : *players)
			{
				it.second->reset(glm::vec3(20.0f * count, 40.0f, 0.0f));
				count++;
			}
		}
	}
}

void Game::update(float dt)
{


	// Step through world simulation with Bullet
	RigidBody::systemUpdate(dt, 10);
	calculateCollisions();


	// Update all gameobjects
	updateScene(dt);



	pauseTimer += dt;

	pausing = allPress(XINPUT_GAMEPAD_START);
	if (pausing > 0 && pauseTimer > 1.0)
	{
		switch (pausing)
		{
		case 0:
			break;
		case 1:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus1;
			pauseTimer = 0;
			break;
		case 2:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus2;
			pauseTimer = 0;
			break;
		case 3:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus3;
			pauseTimer = 0;
			break;
		case 4:
			setPaused(true);
			pause->setPaused(false);
			pause->active = &menus4;
			pauseTimer = 0;
			break;
		}
	}
	if (!lifeCheck())
	{
		this->m_isPaused = true;
		score->m_isPaused = false;
		score->active = &menus1;
	}

}


void Game::draw()
{
	glm::vec4 clearColor = glm::vec4(0.3, 0.0, 0.0, 1.0);
	fboUnlit.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);

	///////////////////////////// First Pass: Outlines
	if (outlineToggle)
	{
		glCullFace(GL_FRONT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(outlineWidth);

		// Clear back buffer
		FrameBufferObject::clearFrameBuffer(glm::vec4(0.8f, 0.8f, 0.8f, 0.0f));

		// Tell all game objects to use the outline shading material
		setMaterialForAllGameObjects("outline");
		setMaterialForAllPlayerObjects("sobelPlayer");
		drawScene();

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

		setMaterialForAllPlayerObjects("toonPlayer");
		materials->at("toonPlayer");
		materials->at("toonPlayer")->shader->bind();

		materials->at("toonPlayer")->vec4Uniforms["u_lightPos"] = camera->getView() * lightPos;
		materials->at("toonPlayer")->intUniforms["u_diffuseTex"] = 31;
		materials->at("toonPlayer")->intUniforms["u_specularTex"] = 30;

		materials->at("toonPlayer")->vec4Uniforms["u_controls"] = glm::vec4(ka, kd, ks, kr);
		materials->at("toonPlayer")->vec4Uniforms["u_dimmers"] = glm::vec4(deskLamp, roomLight, innerCutOff, outerCutOff);
		materials->at("toonPlayer")->vec4Uniforms["u_spotDir"] = glm::vec4(deskForward, 1.0);
		materials->at("toonPlayer")->vec4Uniforms["u_shine"] = glm::vec4(shininess);

		materials->at("toonPlayer")->sendUniforms();
		materials->at("toonPlayer")->shader->unbind();

		materials->at("toon")->shader->bind();

		// Set material properties
		materials->at("toon")->vec4Uniforms["u_lightPos"] = camera->getView() * lightPos;
		materials->at("toon")->vec4Uniforms["u_lightTwo"] = camera->getView() * lightTwo;


		materials->at("toon")->intUniforms["u_toonRamp"] = 5;
		materials->at("toon")->intUniforms["u_diffuseTex"] = 31;
		materials->at("toon")->intUniforms["u_specularTex"] = 30;

		materials->at("toon")->vec4Uniforms["u_controls"] = glm::vec4(ka, kd, ks, kr);
		materials->at("toon")->vec4Uniforms["u_dimmers"] = glm::vec4(deskLamp, roomLight, innerCutOff, outerCutOff);
		materials->at("toon")->vec4Uniforms["u_spotDir"] = glm::vec4(deskForward, 1.0);
		materials->at("toon")->vec4Uniforms["u_shine"] = glm::vec4(shininess);


		materials->at("toon")->sendUniforms();
	}
	break;
	// Just displays Textures
	case NOLIGHT:
	{
		setMaterialForAllGameObjects("noLighting");

		// Set material properties
		materials->at("noLighting")->shader->bind();

		materials->at("noLighting")->intUniforms["u_diffuseTex"] = 31;
		materials->at("noLighting")->intUniforms["u_specularTex"] = 30;

		materials->at("noLighting")->sendUniforms();
	}
	break;
	setMaterialForAllPlayerObjects("toonPlayer");
	// Set material properties for all our non player objects

	materials->at("toon")->shader->unbind();

	//sets the material properties for all our player objects


	}
	drawScene();


	// Draw the debug (if on)
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(camera->getView(), camera->getProj());

	// Unbind scene FBO
	fboUnlit.unbindFrameBuffer(windowWidth, windowHeight);
	FrameBufferObject::clearFrameBuffer(clearColor);

	//////////////////////////////// Post Processing

	if (bloomToggle)
	{
		brightPass();
		blurPass(fboBright, fboBlur);

		fboBlur.bindTextureForSampling(0, GL_TEXTURE0);
		fboUnlit.bindTextureForSampling(0, GL_TEXTURE1);

		FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
		FrameBufferObject::clearFrameBuffer(glm::vec4(1, 0, 0, 1));

		static auto bloomMaterial = materials->at("bloom");

		bloomMaterial->shader->bind();
		bloomMaterial->shader->sendUniformInt("u_bright", 0);
		bloomMaterial->shader->sendUniformInt("u_scene", 1);
		bloomMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
		bloomMaterial->sendUniforms();

		// Draw a full screen quad using the geometry shader
		glDrawArrays(GL_POINTS, 0, 1);
	}
	else
	{
		fboUnlit.bindTextureForSampling(0, GL_TEXTURE0);
		static auto unlitMaterial = materials->at("unlitTexture");
		unlitMaterial->shader->bind();
		unlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
		unlitMaterial->shader->sendUniformInt("u_tex", 0);
		unlitMaterial->sendUniforms();

		// Draw a full screen quad using the geometry shader
		glDrawArrays(GL_POINTS, 0, 1);
	}
	//draw the scene to the fbo

	drawScene();
}

Pause::Pause(std::shared_ptr<Menu> _atlas)
{
	atlas = _atlas;
}

void Pause::update(float dt)
{
	pauseTimer += dt;
	if (active->conButton(XINPUT_GAMEPAD_START) && pauseTimer > 1.0)
	{
		setPaused(true);
		m_parent->getGameState("game")->setPaused(false);
		pauseTimer = 0;
	}

}
void Pause::draw()
{
	atlas->draw();
}
void Pause::setActive(Controller* _con)
{
	active = _con;
}

void Pause::setPaused(bool _state)
{
	m_isPaused = _state;
	if (!_state)
		atlas->setSpot(glm::vec2(1, 1));
}

Score::Score(std::shared_ptr<Menu> _atlas)
{
	atlas = _atlas;
}

void Score::setPaused(bool _state)
{
	m_isPaused = _state;
}

void Score::draw()
{
	atlas->draw();
}

void Score::update(float dt)
{
	pauseTimer += dt;
	if (active->conButton(XINPUT_GAMEPAD_A) && pauseTimer > 1.0)
	{
		setPaused(true);
		m_parent->getGameState("MainMenu")->setPaused(false);
		pauseTimer = 0;
	}
}

Debug::Debug(Camera* _camera)
{
	camera = _camera;
}

void Debug::update(float dt)
{
	if (KEYBOARD_INPUT->CheckPressEvent('e') || KEYBOARD_INPUT->CheckPressEvent('E'))
	{
		RigidBody::setDebugDraw(!RigidBody::isDrawingDebug());
	}
}

void Debug::draw()
{
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(camera->getView(), camera->getProj());
}


void Game::updateScene(float dt)
{
	// Move light in simple circular path
	static float ang = 1.0f;

	ang += dt; // comment out to pause light
	lightPos.x = 40.0f;
	lightPos.y = 65.0f;
	lightPos.z = 0.0f;
	lightPos.w = 1.0f;

	lightTwo.x = 0.0f;
	lightTwo.y = 80.0f;
	lightTwo.z = 100.0f;
	lightTwo.w = 1.0f;

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

	bombManager->update(dt);
}

void Game::initializeFrameBuffers()
{
	fboUnlit.createFrameBuffer(windowWidth, windowHeight, 2, true);
	fboBright.createFrameBuffer(80, 60, 1, false);
	fboBlur.createFrameBuffer(80, 60, 1, false);
	fboBlurB.createFrameBuffer(80, 60, 1, false);
	shadowMap.createFrameBuffer(windowWidth, windowHeight, 1, true);
}



void Game::drawScene()
{
	for (auto itr = scene->begin(); itr != scene->end(); ++itr)
	{
		auto gameobject = itr->second;

		if (gameobject->isRoot())
			gameobject->draw(*camera);
	}

	bombManager->draw(*camera);
}


bool Game::lifeCheck()
{
	int counter = 0;
	for (auto itr = players->begin(); itr != players->end(); ++itr)
	{
		auto gameobject = itr->second;

		if (gameobject->getHealth() > 0)
			counter++;
	}
	if (counter > 1)
		return true;
	else
		return false;
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

// Blurs the FBO
void Game::blurPass(FrameBufferObject fboIn, FrameBufferObject fboOut)
{
	fboOut.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0f));
	fboIn.bindTextureForSampling(0, GL_TEXTURE0);

	static auto blurMaterial = materials->at("blur");

	// Corner, mid, centre, sigma
	// For 3x3 kernel
	static glm::vec4 kernelWeights(0.077847f, 0.123317f, 0.195346f, 1.0f);
	blurMaterial->shader->bind();

	blurMaterial->intUniforms["u_tex"] = 0;
	blurMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
	blurMaterial->vec4Uniforms["u_texelSize"] = glm::vec4(1.0 / 80, 1.0 / 60, 0.0, 0.0);
	blurMaterial->vec4Uniforms["u_kernel"] = kernelWeights;

	blurMaterial->sendUniforms();

	// Draw a full screen quad using the geometry shader
	glDrawArrays(GL_POINTS, 0, 1);

	for (int i = 0; i < numBlurPasses; i++)
	{
		if (i % 2 == 0)
		{
			fboBlurB.bindFrameBufferForDrawing();
			fboOut.bindTextureForSampling(0, GL_TEXTURE0);
			// Draw a full screen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else
		{
			fboOut.bindFrameBufferForDrawing();
			fboBlurB.bindTextureForSampling(0, GL_TEXTURE0);
			// Draw a full screen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}
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
		if (outlineToggle)
			outlineToggle = false;
		else
			outlineToggle = true;
	}
	// Toggle Bloom
	if (KEYBOARD_INPUT->CheckPressEvent('6'))
	{
		if (bloomToggle)
			bloomToggle = false;
		else
			bloomToggle = true;
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
		camera->shakeScreen();
	}
	// Reset all players




	handleKeyboardInputShaders();


	// Clear the keyboard input
	KEYBOARD_INPUT->WipeEventList();
}
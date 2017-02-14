#pragma once
// (C) Bird Brain Games 2017
// main.cpp 

// Core Libraries
#include <iostream>
#include <string>
#include <math.h>

// 3rd Party Libraries
#include "gl\glew.h"
#include "gl\freeglut.h"
#include <GLM\glm.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <GLM\gtx\rotate_vector.hpp>
#include <IL\ilut.h>

// user headers
#include "World.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Shader.h"
#include "RigidBody.h"
#include "camera.h"
#include "material.h"
#include "menu.h"
#include "sound engine.h"

Sound theme;

Controller menus(0);

bool motion = false;
bool mouseClick = false;

// create game object
std::vector<GameObject*> objects;
std::vector<Player*> players;

std::vector<Texture*> textures;
GameWorld *world;
std::shared_ptr<Material> defaultMaterial;
std::shared_ptr<Material> animation;

// Create Shader
glm::vec3 lightPosition(0.0, 0.0, 10.0);

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Miliseconds per frame

Camera camera;

float mousepositionX;
float mousepositionY;
float lastMousepositionX;
float lastMousepositionY;

bool mouseMovement = true;
// A few conversions to know
const float degToRad = 3.14159f / 180.0f;
const float radToDeg = 180.0f / 3.14159f;

Controller *con;


int keyDown[255];

Menu* menu;

bool atlas = true;

// Player Starting Positions
glm::vec3 player1Start = glm::vec3(5.f, 45.0f, 5.f);
glm::vec3 player2Start = glm::vec3(5.f, 45.0f, -5.f);
glm::vec3 player3Start = glm::vec3(-5.f, 45.0f, 5.f);
glm::vec3 player4Start = glm::vec3(-5.f, 45.0f, -5.f);

void newGame()
{
	objects[0]->setTransform(player1Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
	objects[1]->setTransform(player2Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
	objects[2]->setTransform(player3Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
	objects[3]->setTransform(player4Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
	atlas = true;
}

// separate, cleaner, draw function
void drawObjects()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	for (unsigned int i = 0; i < objects.size(); i++)
	{
	objects[i]->draw(camera);

	glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void shaderInit()
{
	//fix the shade so we dont have to do two steps. or maybe not?
	defaultMaterial = std::make_shared<Material>();
	defaultMaterial->shader->load("shaders//blinnphong_v.glsl", "shaders//blinnphong_f.glsl");
	animation = std::make_shared<Material>();
	animation->shader->load("shaders//skinning.vert", "shaders//shader_texture.frag");
	/* Initialize Shader */


	defaultMaterial->shader->bind();
	glEnableVertexAttribArray(4);	glBindAttribLocation(defaultMaterial->shader->getID(), 4, "vPos");
	glEnableVertexAttribArray(5);	glBindAttribLocation(defaultMaterial->shader->getID(), 5, "texture");
	glEnableVertexAttribArray(6);	glBindAttribLocation(defaultMaterial->shader->getID(), 6, "normal");
	glEnableVertexAttribArray(7);	glBindAttribLocation(defaultMaterial->shader->getID(), 7, "color");
	defaultMaterial->shader->unbind();

	animation->shader->bind();
	glBindAttribLocation(animation->shader->getID(), 4, "inPosition");
	glBindAttribLocation(animation->shader->getID(), 5, "vertexUV");
	glBindAttribLocation(animation->shader->getID(), 6, "normal");
	glBindAttribLocation(animation->shader->getID(), 8, "bones");
	glBindAttribLocation(animation->shader->getID(), 9, "weights");
	animation->shader->unbind();
}

void initObjects()
{

	// World class manages memory
	world = new GameWorld();

	// Load Textures
	Texture* ballTex = new Texture("assets//img//Blake.png", "assets//img//Blake.png", 10.0f);
	Texture* groundTex = new Texture("assets//img//desk (diffuse).png", "assets//img//desk (diffuse).png", 10.0f);
	Texture* robot = new Texture("assets//img//bombot.png", "assets//img//bombot.png", 10.0f);
	Texture* bomb = new Texture("assets//img//redTex.png", "assets//img//redTex.png", 10.0f);
	Texture* atlas = new Texture("assets//img//menu_atlas.png", "assets//img//menu_atlas.png", 10.0f);

	textures.push_back(ballTex);
	textures.push_back(groundTex);
	textures.push_back(robot);
	textures.push_back(bomb);
	textures.push_back(atlas);

	glBindTexture(GL_TEXTURE_2D, 0);

	RigidBody *box = new RigidBody();
	//box->load("assets\\bullet\\box5x5.btdata");



	RigidBody *table = new RigidBody();
	table->load("assets\\bullet\\table.btdata");

	// Load the box model
	LoadObject* groundModel = world->getModel("assets\\obj\\5x5box.obj");

	// Load the player animation


	// Load the table model
	LoadObject* tableModel = world->getModel("assets\\obj\\desk2.obj");

	// Create the game objects
	GameObject* ground = new GameObject(groundModel, box, textures[1], defaultMaterial);
	
	GameObject* desk = new GameObject(tableModel, table, textures[1], defaultMaterial);

	// all loading for player
	{
		ANILoader* ani = world->getAniModel("assets\\htr\\finalBombot.htr");
		Holder* robotModel = new Holder(ani);
		RigidBody *rbRobot = new RigidBody(btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
		rbRobot->load("assets\\bullet\\bombot.btdata");
		RigidBody *bombBody = new RigidBody();
		bombBody->load("assets\\bullet\\smolBotTemp.btdata");
		LoadObject* bombModel = new LoadObject();
		bombModel->load("assets\\obj\\bomb.obj");
		GameObject* bomb = new GameObject(bombModel, bombBody, textures[3], defaultMaterial);
		Player* robot = new Player(bomb, 1, robotModel, rbRobot, textures[2], animation);
		robot->setTransform(player1Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
		robot->getRigidBody()->getBody()->applyCentralImpulse(btVector3(0, 1, 0));
		robot->getRigidBody()->getBody()->setActivationState(DISABLE_DEACTIVATION);	// Rigidbody no longer deactivates (must do for each player)
		objects.push_back(robot);
	}

	{
		ANILoader* ani = world->getAniModel("assets\\htr\\finalBombot.htr");
		Holder* robotModel = new Holder(ani);
		RigidBody *rbRobot = new RigidBody(btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
		rbRobot->load("assets\\bullet\\bombot.btdata");
		RigidBody *bombBody = new RigidBody();
		bombBody->load("assets\\bullet\\smolBotTemp.btdata");
		LoadObject* bombModel = new LoadObject();
		bombModel->load("assets\\obj\\bomb.obj");
		GameObject* bomb = new GameObject(bombModel, bombBody, textures[3], defaultMaterial);
		Player* robot = new Player(bomb, 0, robotModel, rbRobot, textures[2], animation);
		robot->setTransform(player2Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
		robot->getRigidBody()->getBody()->applyCentralImpulse(btVector3(0, 1, 0));
		robot->getRigidBody()->getBody()->setActivationState(DISABLE_DEACTIVATION);	// Rigidbody no longer deactivates (must do for each player)
		objects.push_back(robot);
	}

	{
		ANILoader* ani = world->getAniModel("assets\\htr\\finalBombot.htr");
		Holder* robotModel = new Holder(ani);
		RigidBody *rbRobot = new RigidBody(btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
		rbRobot->load("assets\\bullet\\bombot.btdata");
		RigidBody *bombBody = new RigidBody();
		bombBody->load("assets\\bullet\\smolBotTemp.btdata");
		LoadObject* bombModel = new LoadObject();
		bombModel->load("assets\\obj\\bomb.obj");
		GameObject* bomb = new GameObject(bombModel, bombBody, textures[3], defaultMaterial);
		Player* robot = new Player(bomb, 2, robotModel, rbRobot, textures[2], animation);
		robot->setTransform(player3Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
		robot->getRigidBody()->getBody()->applyCentralImpulse(btVector3(0, 1, 0));
		robot->getRigidBody()->getBody()->setActivationState(DISABLE_DEACTIVATION);	// Rigidbody no longer deactivates (must do for each player)
		objects.push_back(robot);
	}

	{
		ANILoader* ani = world->getAniModel("assets\\htr\\finalBombot.htr");
		Holder* robotModel = new Holder(ani);
		RigidBody *rbRobot = new RigidBody(btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
		rbRobot->load("assets\\bullet\\bombot.btdata");
		RigidBody *bombBody = new RigidBody();
		bombBody->load("assets\\bullet\\smolBotTemp.btdata");
		LoadObject* bombModel = new LoadObject();
		bombModel->load("assets\\obj\\bomb.obj");
		GameObject* bomb = new GameObject(bombModel, bombBody, textures[3], defaultMaterial);
		Player* robot = new Player(bomb, 3, robotModel, rbRobot, textures[2], animation);
		robot->setTransform(player4Start, glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
		robot->getRigidBody()->getBody()->applyCentralImpulse(btVector3(0, 1, 0));
		robot->getRigidBody()->getBody()->setActivationState(DISABLE_DEACTIVATION);	// Rigidbody no longer deactivates (must do for each player)
		//robot->getRigidBody()->setDeactive();	// Rigidbody no longer deactivates (must do for each player)
		objects.push_back(robot);
	}
	

	objects.push_back(desk);
	//	objects.push_back(ground);

		//menu stuff
	menu = new Menu(textures[4], 2, 2);

	camera.setPosition(glm::vec3(0, 50, 100));
	camera.setAngle(3.14159012f, 5.98318052f);
	//camera.setAngle(-1.57, 1.57); // show from bottom so better see bomb throw
	camera.setPosition(glm::vec3(0.0f, 25.0f, 70.0f));
	camera.setProperties(44.00002, 1080 / 720, 0.1f, 10000.0f, 0.001f);

	// initializes the positions of all the starting objects
	RigidBody::systemUpdate(1, 10);
	//RigidBody::setDebugDraw(true);
}

/* function DisplayCallbackFunction(void)
* Description:
*  - this is the openGL display routine
*  - this draws the sprites appropriately
*/
void DisplayCallbackFunction(void)
{


	////////////////////////////////////////////////////////////////// Clear our screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	////////////////////////////////////////////////////////////////// Draw Our Scene



	drawObjects();
	//objects[1]->draw(camera);

	//objects[0]->draw(camera);
	if (atlas == true)
	{
	menu->draw();
	}

	// Draw the debug (if on)
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(camera.getView(), camera.getProj());

	glutSwapBuffers();
}

/* function void KeyboardCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is pressed
*/
void KeyboardCallbackFunction(unsigned char key, int x, int y)
{
	KEYBOARD_INPUT->SetActive(key, true);
}

/* function void KeyboardUpCallbackFunction(unsigned char, int,int)
* Description:
*   - this handles keyboard input when a button is lifted
*/
void KeyboardUpCallbackFunction(unsigned char key, int x, int y)
{
	KEYBOARD_INPUT->SetActive(key, false);
}

/* function TimerCallbackFunction(int value)
* Description:
*  - this is called many times per second
*  - this enables you to animate things
*  - no drawing, just changing the state
*  - changes the frame number and calls for a redisplay
*  - FRAME_DELAY is the number of milliseconds to wait before calling the timer again
*/
float rotation = 0;
void TimerCallbackFunction(int value)
{

	
	//std::cout << rotation << std::endl;
	//// process inputs
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
	if (KEYBOARD_INPUT->CheckPressEvent('i') || KEYBOARD_INPUT->CheckPressEvent('I'))
	{
		camera.moveForward();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('k') || KEYBOARD_INPUT->CheckPressEvent('K'))
	{
		camera.moveBackward();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('j') || KEYBOARD_INPUT->CheckPressEvent('J'))
	{
		camera.moveRight();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('l') || KEYBOARD_INPUT->CheckPressEvent('L'))
	{
		camera.moveLeft();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('y') || KEYBOARD_INPUT->CheckPressEvent('Y'))
	{
		rotation += 1;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('h') || KEYBOARD_INPUT->CheckPressEvent('H'))
	{
		rotation -= 1;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('z') || KEYBOARD_INPUT->CheckPressEvent('Z'))
	{
		atlas = !atlas;
	}
	if (menus.conButton(XINPUT_GAMEPAD_A))
	{
		atlas = false;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('x') || KEYBOARD_INPUT->CheckPressEvent('X'))
	{
		menu->incSpot();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('n') || KEYBOARD_INPUT->CheckPressEvent('N'))
	{
		newGame();
	}
	
	// Clear the keyboard input
	KEYBOARD_INPUT->WipeEventList();



	//// update physics
	static unsigned int oldTimeSinceStart = 0;
	unsigned int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	unsigned int deltaT = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	float deltaTasSeconds = float(deltaT) / 1000.0f;

	objects[1]->update(deltaTasSeconds);


	// Bullet step through world simulation
	RigidBody::systemUpdate(deltaTasSeconds, 10);
	

	for (unsigned int i = 0; i < objects.size(); i++)
	{
	objects[i]->update(deltaTasSeconds);
	}

	//// force draw call next tick
	glutPostRedisplay();

	//camera update
	camera.update();

	//sound update
	theme.systemUpdate();

	//// delay timestep to maintain framerate
	//camera rotation
	if (mouseClick == true)
	{
		camera.mouseMotion(mousepositionX, mousepositionY, lastMousepositionX, lastMousepositionY);
	}
	if (motion == false)
	{
		lastMousepositionX = mousepositionX;
		lastMousepositionY = mousepositionY;
	}
	motion = false;
	
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*  - currently set up for an orthographic view (2D only)
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	// switch to projection because we're changing projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / h, 0.1f, 10000.0f);
	camera.setProperties(45.0f, float(w / h), 0.1f, 10000.0f, 0.01f);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	
	if (state == GLUT_DOWN)
	{
		mouseClick = true;
	}
	else
	{
		mouseClick = false;
	}

	lastMousepositionX = mousepositionX;
	lastMousepositionY = mousepositionY;
	mousepositionX = x;
	mousepositionY = y;
}


/* function MouseMotionCallbackFunction()
* Description:
*   - this is called when the mouse is clicked and moves
*/
void MouseMotionCallbackFunction(int x, int y)
{
	motion = true;
	lastMousepositionX = mousepositionX;
	lastMousepositionY = mousepositionY;
	mousepositionX = x;
	mousepositionY = y;
}

/* function MousePassiveMotionCallbackFunction()
* Description:
*   - this is called when the mouse is moved in the window
*/
void MousePassiveMotionCallbackFunction(int x, int y)
{
	motion = true;
	lastMousepositionX = mousepositionX;
	lastMousepositionY = mousepositionY;
	mousepositionX = x;
	mousepositionY = y;
}


/* function CloseCallbackFunction()
* Description:
*	- this is called when the window is closed
*/
void CloseCallbackFunction()
{

	KEYBOARD_INPUT->Destroy();

	delete world;
	world = nullptr;

	// Destroy loaded textures
	for (unsigned i = 0; i < textures.size(); i++)
	{
		delete textures.at(i);
		textures.at(i) = nullptr;
	}

	// Destroy loaded objects
	for (unsigned i = 0; i < objects.size(); i++)
	{
		delete objects.at(i);
		objects.at(i) = nullptr;
	}
}


/* function InitErrorFuncCallbackFunction()
* Description:
*	- the error messages created on init are sent here
*/
void InitErrorFuncCallbackFunction(const char *fmt, va_list ap)
{
	printf(fmt, ap);
	system("pause");
	exit(-1);
}

/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/


int main(int argc, char **argv)
{
	theme.load("assets\\media\\themes.wav");
	theme.play();
	//theme.pause();
	// Set up FreeGLUT error callbacks
	glutInitErrorFunc(InitErrorFuncCallbackFunction);

	// initialize the window and OpenGL properly
	glutInit(&argc, argv);
	glutInitWindowSize(1080, 720);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Dodge Bomb");

	// Initialize OpenGL Extention Wrangler
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		std::cerr << "Glew failed to initialize!" << std::endl;
		exit(-1);
	}

	// check OpenGL version
	printf("Detecting OpenGL version... ");
	const unsigned char *version = glGetString(GL_VERSION);
	printf("Using OpenGL version %s\n", version);


	// set up our function callbacks
	glutDisplayFunc(DisplayCallbackFunction); // draw
	glutKeyboardFunc(KeyboardCallbackFunction); // keyDown
	glutKeyboardUpFunc(KeyboardUpCallbackFunction); // keyUp
	glutReshapeFunc(WindowReshapeCallbackFunction); // windowResized
	glutMouseFunc(MouseClickCallbackFunction); // mouseClick
	glutMotionFunc(MouseMotionCallbackFunction); // mouseMovedActive
	glutPassiveMotionFunc(MousePassiveMotionCallbackFunction); // mouseMovedPassive
	glutTimerFunc(1, TimerCallbackFunction, 0); // timer or tick
	glutCloseFunc(CloseCallbackFunction);

	////////////////////////////////////////////	Call some OpenGL parameters
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// Turn on the lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH); //GL_FLAT



	////////////////////////////////////////////	Textures & Texture parameters
	glEnable(GL_TEXTURE_2D);
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	// Magnification, minification, mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	shaderInit();

	// Load objects
	initObjects();
	//initWorld();

	/* start the event handler */
	glutMainLoop();

	return 0;
}

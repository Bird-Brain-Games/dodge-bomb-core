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

// create game object
std::vector<GameObject*> objects;
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


int keyDown[255];


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

	RigidBody *box = new RigidBody();
	box->load("assets\\bullet\\box5x5.btdata");

	RigidBody *rbRobot = new RigidBody();
	rbRobot->load("assets\\bullet\\bombot.btdata");

	// Load the box model
	std::shared_ptr<LoadObject> groundModel = world->getModel("assets\\obj\\5x5box.obj");

	// Load the player animation
	std::shared_ptr<ANILoader> ani = world->getAniModel("assets\\htr\\finalBombot.htr");
	std::shared_ptr<Holder> robotModel = std::make_shared<Holder>(ani->getRootNode(), ani);

	// Create the game objects
	GameObject* ground = new GameObject(groundModel, box, textures[1], defaultMaterial);
	GameObject* robot = new GameObject(robotModel, rbRobot, textures[2], animation);
	
	// set the 
	robot->setTransform(glm::vec3(0.f, 80.f, 0.f), glm::vec4(0.0f, 0.0f, 0.0f, 1.f));

	objects.push_back(ground);
	objects.push_back(robot);

	camera.setProperties(44.00002, 1080 / 720, 0.1f, 10000.0f, 0.1f);
	camera.setAngle(2.5f, 0.01f);
	camera.setPosition(glm::vec3(-8.5f, -1.0f, 12.0f));
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




	objects[1]->draw(camera);

	objects[0]->draw(camera);


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

	switch (key)
	{
	case 27: // the escape key
		//exit(0);
		glutLeaveMainLoop();
		break;
	}
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
void TimerCallbackFunction(int value)
{
	//// process inputs
	if (KEYBOARD_INPUT->CheckPressEvent('i') || KEYBOARD_INPUT->CheckPressEvent('I'))
	{
		RigidBody::setDebugDraw(true);
	}
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

	//// force draw call next tick
	glutPostRedisplay();

	camera.update();

	//// delay timestep to maintain framerate
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
	camera.setProperties(45.0f, float(w / h), 0.1f, 10000.0f, 0.1f);
	//glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	// Handle mouse clicks
	// Handle mouse clicks
	//cameraLock = true;
	//if (state == GLUT_DOWN)
	//{
	//	if (button == 0)
	//	{
	//		cameraLock = false;
	//	}
	//	else
	//	{
	//		cameraLock = true;
	//	}
	//}
}


/* function MouseMotionCallbackFunction()
* Description:
*   - this is called when the mouse is clicked and moves
*/
void MouseMotionCallbackFunction(int x, int y)
{
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

	// Load Textures
	Texture* ballTex = new Texture("assets//img//Blake.png", "assets//img//Blake.png", 10.0f);
	Texture* groundTex = new Texture("assets//img//Blake.png", "assets//img//Blake.png", 10.0f);
	Texture* robot = new Texture("assets//img//bombot.png", "assets//img//bombot.png", 10.0f);

	textures.push_back(ballTex);
	textures.push_back(groundTex);
	textures.push_back(robot);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Load objects
	initObjects();
	//initWorld();

	/* start the event handler */
	glutMainLoop();

	return 0;
}
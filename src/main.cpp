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

// create game object
std::vector<GameObject*> objects;
std::vector<Texture*> textures;
GameWorld *world;

// Create Shader
Shader *shader;
Shader *meshSkin;
glm::vec3 lightPosition(0.0, 0.0, 10.0);

// Monitor our Projections
glm::mat4x4 projectionMatrix;
glm::mat4x4 modelViewMatrix;

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Miliseconds per frame

int windowWidth = 1024;
int windowHeight = 768;

float mousepositionX;
float mousepositionY;
float lastMousepositionX;
float lastMousepositionY;
void makeMatricies();
glm::vec3 cameraPosition(0.0f, 0.0f, 10.0f);
glm::vec3 forwardVector(0.0f, 0.0f, -1.0f);
glm::vec3 rightVector;
float movementScalar = 0.1f;
bool cameraLock = true;
bool mouseMovement = true;
// A few conversions to know
const float degToRad = 3.14159f / 180.0f;
const float radToDeg = 180.0f / 3.14159f;

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
int keyDown[255];


// separate, cleaner, draw function
void drawObjects()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		objects[i]->draw(shader);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void initObjects()
{
	// World class manages memory
	world = new GameWorld();

	RigidBody *box = new RigidBody();
	box->load("obj\\box5x5.btdata");

	RigidBody *rbRobot = new RigidBody();
	rbRobot->load("obj\\bombot.btdata");

	// Load the box model
	LoadObject* groundModel = world->getModel("obj\\5x5box.obj");

	// Load the player animation
	ANILoader* ani = world->getAniModel("assets\\htr\\finalBombot.htr");
	Holder* robotModel = new Holder(ani->getRootNode(), ani);

	// Create the game objects
	GameObject* ground = new GameObject(groundModel, box, textures[1]);
	GameObject* robot = new GameObject(robotModel, rbRobot, textures[2]);
	
	// set the 
	robot->setTransform(glm::vec3(0.f, 80.f, 0.f), glm::vec4(180.f, 180.f, 180.f, 1.f));

	objects.push_back(ground);
	objects.push_back(robot);
}

/* function DisplayCallbackFunction(void)
* Description:
*  - this is the openGL display routine
*  - this draws the sprites appropriately
*/
void DisplayCallbackFunction(void)
{

	makeMatricies();

	////////////////////////////////////////////////////////////////// Clear our screen
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	////////////////////////////////////////////////////////////////// Draw Our Scene
	glViewport(0, 0, windowWidth, windowHeight);
	projectionMatrix = getProjectionMatrix();
	modelViewMatrix = getViewMatrix();

	meshSkin->bind();
	meshSkin->uniformMat4x4("mvm", &modelViewMatrix);
	meshSkin->uniformMat4x4("prm", &projectionMatrix);

	glBindTexture(GL_TEXTURE_2D, 0);
	objects[1]->draw(meshSkin);
	meshSkin->unbind();

	shader->bind();
	shader->uniformMat4x4("mvm", &modelViewMatrix);
	shader->uniformMat4x4("prm", &projectionMatrix);
	glBindTexture(GL_TEXTURE_2D, 0);
	objects[0]->draw(shader);
	shader->unbind();

	// Draw the debug (if on)
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(modelViewMatrix, projectionMatrix);

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
	windowWidth = w;
	windowHeight = h;
	//glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	// Handle mouse clicks
	// Handle mouse clicks
	cameraLock = true;
	if (state == GLUT_DOWN)
	{
		if (button == 0)
		{
			cameraLock = false;
		}
		else
		{
			cameraLock = true;
		}
	}
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
	delete shader; shader = nullptr;
	delete meshSkin; meshSkin = nullptr;
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
	glutInitWindowSize(windowWidth, windowHeight);
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

	/* Initialize Shader */
	shader = new Shader("shaders//blinnphong_v.glsl", "shaders//blinnphong_f.glsl");
	shader->bind();

	meshSkin = new Shader("shaders//skinning.vert", "shaders//shader_texture.frag");


	glEnableVertexAttribArray(4);	glBindAttribLocation(shader->getID(), 4, "vPos");
	glEnableVertexAttribArray(5);	glBindAttribLocation(shader->getID(), 5, "texture");
	glEnableVertexAttribArray(6);	glBindAttribLocation(shader->getID(), 6, "normal");
	glEnableVertexAttribArray(7);	glBindAttribLocation(shader->getID(), 7, "color");
	
	shader->unbind();
	meshSkin->bind();
	glBindAttribLocation(meshSkin->getID(), 4, "inPosition");
	glBindAttribLocation(meshSkin->getID(), 5, "vertexUV");
	glBindAttribLocation(meshSkin->getID(), 6, "normal");
	glBindAttribLocation(meshSkin->getID(), 8, "bones");
	glBindAttribLocation(meshSkin->getID(), 9, "weights");
	meshSkin->unbind();

	// Load Textures
	Texture* ballTex = new Texture("img//Blake.png", "img//Blake.png", 10.0f);
	Texture* groundTex = new Texture("img//Blake.png", "img//Blake.png", 10.0f);
	Texture* robot = new Texture("img//bombot.png", "img//bombot.png", 10.0f);

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



//camera variables
glm::vec3 up;
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
//camera defaults
glm::vec3 direction;
glm::vec3 position = glm::vec3(-8.5f, -1.0f, 12.0f);
glm::vec3 gameDefaultPos = glm::vec3(0.0f);
glm::vec3 menuDefaultPos = glm::vec3(0.0f);
glm::vec2 gameDefaultAngle(0.0f);
glm::vec2 menuDefaultAngle(0.0f);
glm::vec2 currentAngles = glm::vec2(2.5f, 0.01f);
float mouseSpeed = 0.005f;
float speed = 0.07f;
float initialFoV = 45.0f;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

bool debug = false;



void makeMatricies()
{
	if (cameraLock == false)
	{
		float tempX = float(lastMousepositionX - mousepositionX);
		float tempY = float(lastMousepositionY - mousepositionY);
		currentAngles.x += mouseSpeed * tempX;
		currentAngles.y += mouseSpeed * tempY;
	}

	direction = glm::vec3(
		cos(currentAngles.y) * sin(currentAngles.x),
		sin(currentAngles.y),
		cos(currentAngles.y) * cos(currentAngles.x)
	);
	glm::vec3 right = glm::vec3(
		sin(currentAngles.x - 3.14f / 2.0f),
		0,
		cos(currentAngles.x - 3.14f / 2.0f)
	);
	up = glm::cross(right, direction);

	if (KEYBOARD_INPUT->CheckPressEvent('w'))
	{
		position += direction *  speed;
	}
	// Move backward
	if (KEYBOARD_INPUT->CheckPressEvent('s'))
	{
		position -= direction *  speed;
	}
	// Strafe right
	if (KEYBOARD_INPUT->CheckPressEvent('d'))
	{
		position += right *  speed;
	}
	// Strafe left
	if (KEYBOARD_INPUT->CheckPressEvent('a'))
	{
		position -= right * speed;
	}


	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(initialFoV, (float)windowWidth / windowHeight, 0.1f, 10000.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

}
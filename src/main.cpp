// Graham Watson 100522240
// Robert Savaglio 100591436



// Core Libraries
#include <iostream>
#include <string>
#include <math.h>
#include <map> // for std::map
#include <memory> // for std::shared_ptr

// 3rd Party Libraries
#include <GLEW\glew.h>
#include "gl\freeglut.h"
#include <IL/il.h> // for ilInit()
#include <IL\ilut.h>
#include <glm\vec3.hpp>
#include <glm\gtx\color_space.hpp>
//#include "vld.h"

// User Libraries
#include "Shader.h"
#include "ShaderProgram.h"
#include "loadObject.h"
#include "GameObject.h"
#include "Player.h"
#include "FrameBufferObject.h"
#include "InputManager.h"
#include "menu.h"

// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Milliseconds per frame

int windowWidth = 800;
int windowHeight = 600;

glm::vec3 mousePosition; // x,y,0
glm::vec3 mousePositionFlipped; // x, height - y, 0

// A few conversions to know
const float degToRad = 3.14159f / 180.0f;
const float radToDeg = 180.0f / 3.14159f;

float deltaTime = 0.0f; // amount of time since last update (set every frame in timer callback)

glm::vec3 position;
float movementSpeed = 5.0f;
glm::vec4 lightPos;

// Cameras
Camera playerCamera; // the camera you move around with wasd

// Asset databases
std::map<std::string, std::shared_ptr<LoadObject>> meshes;
std::map<std::string, std::shared_ptr<GameObject>> gameobjects;
std::map<std::string, std::shared_ptr<Texture>> textures;

// Materials
std::map<std::string, std::shared_ptr<Material>> materials;

// Controls
bool inMenu = false;
std::unique_ptr<Menu> mainMenu;

// Framebuffer objects
FrameBufferObject fboUnlit;
FrameBufferObject fboBright;
FrameBufferObject fboBlurA, fboBlurB;

enum FBOMode
{
	DEFAULT,
	BRIGHT_PASS,
	BLURRED_BRIGHT_PASS,
	BLOOM
};

FBOMode currentMode = DEFAULT;

void initializeShaders()
{
	std::string shaderPath = "Shaders/";

	// Load shaders

	// Vertex Shaders
	Shader v_default, v_passThru, v_null;
	v_default.loadShaderFromFile(shaderPath + "default_v.glsl", GL_VERTEX_SHADER);
	v_passThru.loadShaderFromFile(shaderPath + "passThru_v.glsl", GL_VERTEX_SHADER);
	v_null.loadShaderFromFile(shaderPath + "null.vert", GL_VERTEX_SHADER);

	// Fragment Shaders
	Shader f_default, f_unlitTex, f_bright, f_composite, f_blur, f_texColor;
	f_default.loadShaderFromFile(shaderPath + "default_f.glsl", GL_FRAGMENT_SHADER);
	f_bright.loadShaderFromFile(shaderPath + "bright_f.glsl", GL_FRAGMENT_SHADER);
	f_unlitTex.loadShaderFromFile(shaderPath + "unlitTexture_f.glsl", GL_FRAGMENT_SHADER);
	f_composite.loadShaderFromFile(shaderPath + "bloomComposite_f.glsl", GL_FRAGMENT_SHADER);
	f_blur.loadShaderFromFile(shaderPath + "gaussianBlur_f.glsl", GL_FRAGMENT_SHADER);
	f_texColor.loadShaderFromFile(shaderPath + "shader_texture.frag", GL_FRAGMENT_SHADER);

	// Geometry Shaders
	Shader g_quad, g_menu;
	g_quad.loadShaderFromFile(shaderPath + "quad.geom", GL_GEOMETRY_SHADER);
	g_menu.loadShaderFromFile(shaderPath + "menu.geom", GL_GEOMETRY_SHADER);

	// Default material that all objects use
	materials["default"] = std::make_shared<Material>();
	materials["default"]->shader->attachShader(v_default);
	materials["default"]->shader->attachShader(f_default);
	materials["default"]->shader->linkProgram();

	// Material used for menu full screen drawing
	materials["menu"] = std::make_shared<Material>();
	materials["menu"]->shader->attachShader(v_null);
	materials["menu"]->shader->attachShader(f_texColor);
	materials["menu"]->shader->attachShader(g_menu);
	materials["menu"]->shader->linkProgram();

	// Unlit texture material
	materials["unlitTexture"] = std::make_shared<Material>();
	materials["unlitTexture"]->shader->attachShader(v_passThru);
	materials["unlitTexture"]->shader->attachShader(f_unlitTex);
	materials["unlitTexture"]->shader->attachShader(g_quad);
	materials["unlitTexture"]->shader->linkProgram();

	// Invert filter material
	materials["bright"] = std::make_shared<Material>();
	materials["bright"]->shader->attachShader(v_passThru);
	materials["bright"]->shader->attachShader(f_bright);
	materials["bright"]->shader->attachShader(g_quad);
	materials["bright"]->shader->linkProgram();

	// Sobel filter material
	materials["bloom"] = std::make_shared<Material>();
	materials["bloom"]->shader->attachShader(v_passThru);
	materials["bloom"]->shader->attachShader(f_composite);
	materials["bloom"]->shader->attachShader(g_quad);
	materials["bloom"]->shader->linkProgram();

	// Box blur filter
	materials["blur"] = std::make_shared<Material>();
	materials["blur"]->shader->attachShader(v_passThru);
	materials["blur"]->shader->attachShader(f_blur);
	materials["blur"]->shader->attachShader(g_quad);
	materials["blur"]->shader->linkProgram();
}

void initializeScene()
{
	///////////////////////////////////////////////////////////////////////////
	////////////////////////////	MESHES		///////////////////////////////
	std::string meshPath = "Assets/obj/";
	
	// Initialize all meshes
	std::shared_ptr<LoadObject> tableMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> barrelMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> cannonMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> sphereMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> bombotMesh = std::make_shared<LoadObject>();

	// Load all meshes
	tableMesh->load(meshPath + "table.obj");
	barrelMesh->load(meshPath + "barrel.obj");
	cannonMesh->load(meshPath + "cannon.obj");
	sphereMesh->load(meshPath + "sphere.obj");
	bombotMesh->load(meshPath + "smolbot.obj");

	// Add all meshes to map
	meshes["table"] = tableMesh;
	meshes["barrel"] = barrelMesh;
	meshes["cannon"] = cannonMesh;
	meshes["sphere"] = sphereMesh;
	meshes["bombot"] = bombotMesh;

	///////////////////////////////////////////////////////////////////////////
	////////////////////////////	TEXTURES	///////////////////////////////
	// Load textures (WIP)
	// Has to take char* due to ILUT
	char diffuseTex[] = "Assets/img/Blake.png";
	std::shared_ptr<Texture> defaultTex = std::make_shared<Texture>(diffuseTex, diffuseTex, 1.0f);

	char bombotTex[] = "Assets/img/bombot(diffuse).png";
	std::shared_ptr<Texture> bombotTexMap = std::make_shared<Texture>(bombotTex, bombotTex, 1.0f);

	//Add textures to the map
	textures["default"] = defaultTex;
	textures["bombot"] = bombotTexMap;

	///////////////////////////////////////////////////////////////////////////
	////////////////////////	GAME OBJECTS	///////////////////////////////
	auto defaultMaterial = materials["default"];

	gameobjects["table"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), tableMesh, defaultMaterial, defaultTex);

	gameobjects["barrel"] = std::make_shared<GameObject>(
		glm::vec3(-5.f, 45.0f, -5.f), barrelMesh, defaultMaterial, nullptr);

	gameobjects["cannon"] = std::make_shared<GameObject>(
		glm::vec3(-5.f, 45.0f, -5.f), cannonMesh, defaultMaterial, nullptr);

	gameobjects["sphere"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), sphereMesh, defaultMaterial, nullptr);

	gameobjects["bombot1"] = std::make_shared<Player>(
		glm::vec3(0.0f, 5.0f, 0.0f), bombotMesh, defaultMaterial, bombotTexMap, 1);
	/*
	gameobjects["bombot2"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), bombotMesh, defaultMaterial, nullptr);
	gameobjects["bombot3"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), bombotMesh, defaultMaterial, nullptr);
	gameobjects["bombot4"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), bombotMesh, defaultMaterial, nullptr);*/
	
	///////////////////////////////////////////////////////////////////////////
	////////////////////////	RIGID BODIES	///////////////////////////////
	
	// Create rigidbody paths
	std::string tableBodyPath = "assets\\bullet\\table.btdata";
	std::string bombotBodyPath = "assets\\bullet\\bombot.btdata";

	// Create rigidbodies
	std::unique_ptr<RigidBody> tableBody;
	std::unique_ptr<RigidBody> bombot1Body;

	tableBody = std::make_unique<RigidBody>();
	bombot1Body = std::make_unique<RigidBody>();

	// Load rigidbodies
	tableBody->load(tableBodyPath);
	bombot1Body->load(bombotBodyPath);

	// Attach rigidbodies
	gameobjects["table"]->attachRigidBody(tableBody);
	gameobjects["bombot1"]->attachRigidBody(bombot1Body);


	///////////////////////////////////////////////////////////////////////////
	////////////////////////	PROPERTIES		///////////////////////////////
	
	// Set object properties

	// Set menu properties
	mainMenu = std::make_unique<Menu>(defaultTex);
	mainMenu->setMaterial(materials["menu"]);

	// Set default camera properties (WIP)
	playerCamera.setPosition(glm::vec3(0.0f, 25.0f, 70.0f));
	playerCamera.setAngle(3.14159012f, 5.98318052f);
	//playerCamera.setProperties(44.00002, (float)windowWidth / (float)windowHeight, 0.1f, 10000.0f, 0.001f);
	playerCamera.update();
}

void initializeFrameBuffers()
{
	fboUnlit.createFrameBuffer(windowWidth, windowHeight, 1, true);
	fboBright.createFrameBuffer(80, 60, 1, false);
	fboBlurA.createFrameBuffer(80, 60, 1, false);
	fboBlurB.createFrameBuffer(80, 60, 1, false);
}

void updateScene()
{
	// Move light in simple circular path
	static float ang = 1.0f;

	ang += deltaTime; // comment out to pause light
	lightPos.x = cos(ang) * 10.0f;
	lightPos.y = cos(ang*4.0f) * 2.0f + 10.0f;
	lightPos.z = sin(ang) * 10.0f;
	lightPos.w = 1.0f;

	gameobjects["sphere"]->setPosition(lightPos);

	// Update all game objects
	for (auto itr = gameobjects.begin(); itr != gameobjects.end(); ++itr)
	{
		auto gameobject = itr->second;

		// Remember: root nodes are responsible for updating all of its children
		// So we need to make sure to only invoke update() for the root nodes.
		// Otherwise some objects would get updated twice in a frame!
		if (gameobject->isRoot())
			gameobject->update(deltaTime);
	}
}


void drawScene(Camera& cam)
{
	for (auto itr = gameobjects.begin(); itr != gameobjects.end(); ++itr)
	{
		auto gameobject = itr->second;

		if (gameobject->isRoot())
			gameobject->draw(cam);
	}
}

void setMaterialForAllGameObjects(std::string materialName)
{
	auto mat = materials[materialName];
	for (auto itr = gameobjects.begin(); itr != gameobjects.end(); ++itr)
	{
		itr->second->setMaterial(mat);
	}
}

// Only takes the highest brightness from the FBO
void brightPass()
{
	fboBright.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0f));
	fboUnlit.bindTextureForSampling(0, GL_TEXTURE0);

	static auto brightMaterial = materials["bright"];
	brightMaterial->shader->bind();

	static glm::vec4 bloomThreshold(0.2f);
	brightMaterial->vec4Uniforms["u_bloomThreshold"] = bloomThreshold;
	brightMaterial->intUniforms["u_tex"] = 0;
	brightMaterial->mat4Uniforms["u_mvp"] = glm::mat4();

	brightMaterial->sendUniforms();

	glDrawArrays(GL_POINTS, 0, 1);
}

// Blurs the FBO
void blurBrightPass()
{
	fboBlurA.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(glm::vec4(0.0f));
	fboBright.bindTextureForSampling(0, GL_TEXTURE0);

	static auto blurMaterial = materials["blur"];

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

	static int numBlurPasses = 4;
	for (int i = 0; i < numBlurPasses; i++)
	{
		if (i % 2 == 0)
		{
			fboBlurB.bindFrameBufferForDrawing();
			fboBlurA.bindTextureForSampling(0, GL_TEXTURE0);
			// Draw a full screen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else
		{
			fboBlurA.bindFrameBufferForDrawing();
			fboBlurB.bindTextureForSampling(0, GL_TEXTURE0);
			// Draw a full screen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}
}

// This is where we draw stuff
void DisplayCallbackFunction(void)
{
	glm::vec4 clearColor = glm::vec4(0.3, 0.0, 0.0, 1.0);

	// bind scene FBO
	fboUnlit.bindFrameBufferForDrawing();
	FrameBufferObject::clearFrameBuffer(clearColor);

	// Set material properties
	materials["default"]->vec4Uniforms["u_lightPos"] = playerCamera.getView() * lightPos;

	materials["default"]->intUniforms["u_diffuseTex"] = 31;
	materials["default"]->intUniforms["u_specularTex"] = 30;
	//material->vec4Uniforms["u_shininess"] =

	// draw the scene to the fbo
	if (!inMenu)
		drawScene(playerCamera);
	else
		mainMenu->draw();

	// Draw the debug (if on)
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(playerCamera.getView(), playerCamera.getProj());

	// Unbind scene FBO
	fboUnlit.unbindFrameBuffer(windowWidth, windowHeight);
	FrameBufferObject::clearFrameBuffer(clearColor);

	static auto unlitMaterial = materials["unlitTexture"];

	// Apply a post process filter
	switch (currentMode)
	{
		// No filter
		case DEFAULT: // press 1
		{
			fboUnlit.bindTextureForSampling(0, GL_TEXTURE0);
			
			// Tell opengl which shader we want it to use
			unlitMaterial->shader->bind();

			// Tell the sampler2d named "u_tex" to look at texture unit 0
			unlitMaterial->shader->sendUniformInt("u_tex", 0);
			unlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();

			// Send uniform varibles to GPU
			unlitMaterial->sendUniforms();

			// Draw fullscreen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
		break;

		// Extract highlights
		case BRIGHT_PASS: // press 2
		{
			brightPass();

			fboBright.bindTextureForSampling(0, GL_TEXTURE0);
			
			FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
			FrameBufferObject::clearFrameBuffer(glm::vec4(1,0,0,1));
			unlitMaterial->shader->bind();
			unlitMaterial->shader->sendUniformInt("u_tex", 0);
			unlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
			unlitMaterial->sendUniforms();

			// Draw a full screen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
		break;

		// Blur highlights
		case BLURRED_BRIGHT_PASS: // press 3
		{
			brightPass();
			blurBrightPass();

			fboBlurA.bindTextureForSampling(0, GL_TEXTURE0);
			
			FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
			FrameBufferObject::clearFrameBuffer(clearColor);
			unlitMaterial->shader->bind();
			unlitMaterial->shader->sendUniformInt("u_tex", 0);
			unlitMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
			unlitMaterial->sendUniforms();

			// Draw a full screen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
		break;

		// Composite the bloom effect
		case BLOOM: // press 4
		{
			brightPass();
			blurBrightPass();

			fboBlurA.bindTextureForSampling(0, GL_TEXTURE0);
			fboUnlit.bindTextureForSampling(0, GL_TEXTURE1);

			FrameBufferObject::unbindFrameBuffer(windowWidth, windowHeight);
			FrameBufferObject::clearFrameBuffer(glm::vec4(1, 0, 0, 1));

			static auto bloomMaterial = materials["bloom"];

			bloomMaterial->shader->bind();
			bloomMaterial->shader->sendUniformInt("u_bright", 0);
			bloomMaterial->shader->sendUniformInt("u_scene", 1);
			bloomMaterial->mat4Uniforms["u_mvp"] = glm::mat4();
			bloomMaterial->sendUniforms();

			// Draw a full screen quad using the geometry shader
			glDrawArrays(GL_POINTS, 0, 1);
		}
		break;
	}

	/* Swap Buffers to Make it show up on screen */
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

void handleKeyboardInput()
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
		playerCamera.moveForward();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('s') || KEYBOARD_INPUT->CheckPressEvent('S'))
	{
		playerCamera.moveBackward();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('d') || KEYBOARD_INPUT->CheckPressEvent('D'))
	{
		playerCamera.moveRight();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('a') || KEYBOARD_INPUT->CheckPressEvent('A'))
	{
		playerCamera.moveLeft();
	}
	if (KEYBOARD_INPUT->CheckPressEvent('j') || KEYBOARD_INPUT->CheckPressEvent('J'))
	{
		gameobjects["table"]->setPosition(
			gameobjects["table"]->getWorldPosition() + glm::vec3(10.0, 0.0, 0.0));
	}

	// Switch video modes
	if (KEYBOARD_INPUT->CheckPressEvent('1'))
	{
		currentMode = DEFAULT;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('2'))
	{
		currentMode = BRIGHT_PASS;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('3'))
	{
		currentMode = BLURRED_BRIGHT_PASS;
	}
	if (KEYBOARD_INPUT->CheckPressEvent('4'))
	{
		currentMode = BLOOM;
	}

	// Clear the keyboard input
	KEYBOARD_INPUT->WipeEventList();
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
	// Calculate new deltaT for potential updates and physics calculations
	static int elapsedTimeAtLastTick = 0;
	int totalElapsedTime = glutGet(GLUT_ELAPSED_TIME);

	deltaTime = totalElapsedTime - elapsedTimeAtLastTick;
	deltaTime /= 1000.0f;
	elapsedTimeAtLastTick = totalElapsedTime;

	// Handle all inputs 
	handleKeyboardInput();

	// Step through world simulation with Bullet
	RigidBody::systemUpdate(deltaTime, 10);
	
	// Update the camera's position
	playerCamera.update();

	// Update all gameobjects
	updateScene();

	/* this call makes it actually show up on screen */
	glutPostRedisplay();
	/* this call gives it a proper frame delay to hit our target FPS */
	glutTimerFunc(FRAME_DELAY, TimerCallbackFunction, 0);
}

/* function WindowReshapeCallbackFunction()
* Description:
*  - this is called whenever the window is resized
*  - and sets up the projection matrix properly
*/
void WindowReshapeCallbackFunction(int w, int h)
{
	/* Update our Window Properties */
	windowWidth = w;
	windowHeight = h;

	// TODO: more needed?

	//// switch to projection because we're changing projection
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(45.0f, (float)w / h, 0.1f, 10000.0f);
	//camera.setProperties(45.0f, float(w / h), 0.1f, 10000.0f, 0.01f);
	//glViewport(0, 0, w, h);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	playerCamera.setRatio(windowHeight, windowWidth);
}


void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped = mousePosition;
	mousePositionFlipped.y = windowHeight - mousePosition.y;
}

// Called when the mouse is clicked and moves
void MouseMotionCallbackFunction(int x, int y)
{
	/*if (mousePosition.length() > 0)
		playerCamera.processMouseMotion(x, y, mousePosition.x, mousePosition.y, deltaTime);*/

	mousePosition.x = x;
	mousePosition.y = y;

	mousePositionFlipped = mousePosition;
	mousePositionFlipped.y = windowHeight - mousePosition.y;
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

/* function CloseCallbackFunction()
* Description:
*	- this is called when the window is closed
*/
void CloseCallbackFunction()
{
	KEYBOARD_INPUT->Destroy();

}

/* function main()
* Description:
*  - this is the main function
*  - does initialization and then calls glutMainLoop() to start the event handler
*/
int main(int argc, char **argv)
{
	// Memory Leak Detection
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Set up FreeGLUT error callbacks
	glutInitErrorFunc(InitErrorFuncCallbackFunction);

	/* initialize the window and OpenGL properly */
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Dodge Bomb");

	// Init GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "TTK::InitializeTTK Error: GLEW failed to init" << std::endl;
	}
	printf("OpenGL version: %s, GLSL version: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	/* set up our function callbacks */
	glutDisplayFunc(DisplayCallbackFunction);
	glutKeyboardFunc(KeyboardCallbackFunction);
	glutKeyboardUpFunc(KeyboardUpCallbackFunction);
	glutReshapeFunc(WindowReshapeCallbackFunction);
	glutMouseFunc(MouseClickCallbackFunction);
	glutMotionFunc(MouseMotionCallbackFunction);
	glutTimerFunc(1, TimerCallbackFunction, 0);
	glutCloseFunc(CloseCallbackFunction);


	// Init IL
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	// Init GL
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Initialize scene
	initializeShaders();
	initializeScene();
	initializeFrameBuffers();

	/* Start Game Loop */
	deltaTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime /= 1000.0f;

	glutMainLoop();
	return 0;
}
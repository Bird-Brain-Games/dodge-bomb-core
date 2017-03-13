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
#include "ANILoader.h"
#include "GameState.h"
#include "states.h"
// Defines and Core variables
#define FRAMES_PER_SECOND 60
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // Milliseconds per frame



glm::vec3 mousePosition; // x,y,0
glm::vec3 mousePositionFlipped; // x, height - y, 0

// A few conversions to know
const float degToRad = 3.14159f / 180.0f;
const float radToDeg = 180.0f / 3.14159f;

float deltaTime = 0.0f; // amount of time since last update (set every frame in timer callback)

//state machine
GameStateManager states;
MainMenu* mainMenu;
Game* game;
Score* score;
Pause* pause;



glm::vec3 position;
float movementSpeed = 5.0f;

Camera playerCamera; // the camera you move around with wasd
Camera shadowCamera; // Camera for the shadow map


// Asset databases
std::map<std::string, std::shared_ptr<Loader>> meshes;
std::map<std::string, std::shared_ptr<GameObject>> gameobjects;
std::map<std::string, std::shared_ptr<Player>> players;
std::map<std::string, std::shared_ptr<Texture>> textures;

// Materials
std::map<std::string, std::shared_ptr<Material>> materials;

// Controls
bool inMenu = false;
std::shared_ptr<Menu> menu;
std::shared_ptr<BombManager> bombManager;










void bulletNearCallback(btBroadphasePair& collisionPair,
	btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo);

void initializeShaders()
{
	std::string shaderPath = "Shaders/";

	// Load shaders

	// Vertex Shaders
	Shader v_default, v_passThru, v_null, v_skinning, v_shadow;
	v_default.loadShaderFromFile(shaderPath + "default_v.glsl", GL_VERTEX_SHADER);
	v_passThru.loadShaderFromFile(shaderPath + "passThru_v.glsl", GL_VERTEX_SHADER);
	v_null.loadShaderFromFile(shaderPath + "null.vert", GL_VERTEX_SHADER);
	v_skinning.loadShaderFromFile(shaderPath + "skinning.vert", GL_VERTEX_SHADER);
	v_shadow.loadShaderFromFile(shaderPath + "shadowMap_v.glsl", GL_VERTEX_SHADER);

	// Fragment Shaders
	Shader f_default, f_unlitTex, f_bright, f_composite, f_blur, f_texColor, f_noLighting, f_toon, f_sobel, f_shadow;
	f_default.loadShaderFromFile(shaderPath + "default_f.glsl", GL_FRAGMENT_SHADER);
	f_bright.loadShaderFromFile(shaderPath + "bright_f.glsl", GL_FRAGMENT_SHADER);
	f_unlitTex.loadShaderFromFile(shaderPath + "unlitTexture_f.glsl", GL_FRAGMENT_SHADER);
	f_composite.loadShaderFromFile(shaderPath + "bloomComposite_f.glsl", GL_FRAGMENT_SHADER);
	f_blur.loadShaderFromFile(shaderPath + "gaussianBlur_f.glsl", GL_FRAGMENT_SHADER);
	f_texColor.loadShaderFromFile(shaderPath + "shader_texture.frag", GL_FRAGMENT_SHADER);
	f_noLighting.loadShaderFromFile(shaderPath + "noLighting_f.glsl", GL_FRAGMENT_SHADER);
	f_toon.loadShaderFromFile(shaderPath + "toon_f.glsl", GL_FRAGMENT_SHADER);
	f_sobel.loadShaderFromFile(shaderPath + "sobel_f.glsl", GL_FRAGMENT_SHADER);
	f_shadow.loadShaderFromFile(shaderPath + "shadowMap_f.glsl", GL_FRAGMENT_SHADER);

	// Geometry Shaders
	Shader g_quad, g_menu;
	g_quad.loadShaderFromFile(shaderPath + "quad.geom", GL_GEOMETRY_SHADER);
	g_menu.loadShaderFromFile(shaderPath + "menu.geom", GL_GEOMETRY_SHADER);

	// No Lighting material
	materials["noLighting"] = std::make_shared<Material>("noLighting");
	materials["noLighting"]->shader->attachShader(v_default);
	materials["noLighting"]->shader->attachShader(f_noLighting);
	materials["noLighting"]->shader->linkProgram();

	// Default material that all objects use
	materials["default"] = std::make_shared<Material>("default");
	materials["default"]->shader->attachShader(v_default);
	materials["default"]->shader->attachShader(f_default);
	materials["default"]->shader->linkProgram();

	// Default material that all objects use
	materials["toon"] = std::make_shared<Material>("toon");
	materials["toon"]->shader->attachShader(v_default);
	materials["toon"]->shader->attachShader(f_toon);
	materials["toon"]->shader->linkProgram();

	//material for our players and there meshes.
	materials["toonPlayer"] = std::make_shared<Material>("toonPlayer");
	materials["toonPlayer"]->shader->attachShader(v_skinning);
	materials["toonPlayer"]->shader->attachShader(f_toon);
	materials["toonPlayer"]->shader->linkProgram();

	// Material used for menu full screen drawing
	materials["menu"] = std::make_shared<Material>("menu");
	materials["menu"]->shader->attachShader(v_null);
	materials["menu"]->shader->attachShader(f_texColor);
	materials["menu"]->shader->attachShader(g_menu);
	materials["menu"]->shader->linkProgram();

	// Unlit texture material
	materials["unlitTexture"] = std::make_shared<Material>("unlitTexture");
	materials["unlitTexture"]->shader->attachShader(v_passThru);
	materials["unlitTexture"]->shader->attachShader(f_unlitTex);
	materials["unlitTexture"]->shader->attachShader(g_quad);
	materials["unlitTexture"]->shader->linkProgram();

	// Invert filter material
	materials["bright"] = std::make_shared<Material>("bright");
	materials["bright"]->shader->attachShader(v_passThru);
	materials["bright"]->shader->attachShader(f_bright);
	materials["bright"]->shader->attachShader(g_quad);
	materials["bright"]->shader->linkProgram();

	// Sobel filter material
	materials["bloom"] = std::make_shared<Material>("bloom");
	materials["bloom"]->shader->attachShader(v_passThru);
	materials["bloom"]->shader->attachShader(f_composite);
	materials["bloom"]->shader->attachShader(g_quad);
	materials["bloom"]->shader->linkProgram();

	// Box blur filter
	materials["blur"] = std::make_shared<Material>("blur");
	materials["blur"]->shader->attachShader(v_passThru);
	materials["blur"]->shader->attachShader(f_blur);
	materials["blur"]->shader->attachShader(g_quad);
	materials["blur"]->shader->linkProgram();

	// Sobel filter material
	materials["outline"] = std::make_shared<Material>("outline");
	materials["outline"]->shader->attachShader(v_passThru);
	materials["outline"]->shader->attachShader(f_sobel);
	materials["outline"]->shader->linkProgram();

	// sobel filter for player
	materials["sobelPlayer"] = std::make_shared<Material>("sobelPlayer");
	materials["sobelPlayer"]->shader->attachShader(v_skinning);
	materials["sobelPlayer"]->shader->attachShader(f_sobel);
	materials["sobelPlayer"]->shader->linkProgram();

	// Shadow filter material
	materials["shadow"] = std::make_shared<Material>("shadow");
	materials["shadow"]->shader->attachShader(v_shadow);
	materials["shadow"]->shader->attachShader(f_shadow);
	materials["shadow"]->shader->linkProgram();
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
	std::shared_ptr<LoadObject> corkboardMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> roomMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> bombMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> boatMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> booksMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> boulderMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> crateMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> palmtreeMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> lampcupMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> organizerMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> mapMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> markerMesh = std::make_shared<LoadObject>();

	std::shared_ptr<Holder> bombotMesh = std::make_shared<Holder>();
	std::shared_ptr<Holder> bombotMesh2 = std::make_shared<Holder>();
	std::shared_ptr<Holder> bombotMesh3 = std::make_shared<Holder>();
	std::shared_ptr<Holder> bombotMesh4 = std::make_shared<Holder>();

	//loadAnimations(bombotMesh2);

	// Load all meshes
	tableMesh->load(meshPath + "translatedtable.obj");
	barrelMesh->load(meshPath + "barrel.obj");
	cannonMesh->load(meshPath + "scaledcannon.obj");
	sphereMesh->load(meshPath + "sphere.obj");

	// function wrapper for loading animations. makes less cluttered
	loadAnimations(bombotMesh);
	loadAnimations(bombotMesh2);
	loadAnimations(bombotMesh3);
	loadAnimations(bombotMesh4);




	corkboardMesh->load(meshPath + "scaledcorkboard.obj");
	roomMesh->load(meshPath + "scaledroom.obj");
	bombMesh->load(meshPath + "bomb.obj");
	boatMesh->load(meshPath + "scaledboat.obj");
	booksMesh->load(meshPath + "books.obj");
	boulderMesh->load(meshPath + "scaledboulder.obj");
	crateMesh->load(meshPath + "scaledcrate.obj");
	palmtreeMesh->load(meshPath + "palmtree.obj");
	lampcupMesh->load(meshPath + "lampcup.obj");
	organizerMesh->load(meshPath + "scaledorganizer.obj");
	mapMesh->load(meshPath + "scaledmap.obj");
	markerMesh->load(meshPath + "scaledmarker.obj");

	// Add all meshes to map
	meshes["table"] = tableMesh;
	meshes["barrel"] = barrelMesh;
	meshes["cannon"] = cannonMesh;
	meshes["sphere"] = sphereMesh;
	meshes["bombot"] = bombotMesh;
	meshes["bombot2"] = bombotMesh2;
	meshes["corkboard"] = corkboardMesh;
	meshes["room"] = roomMesh;
	meshes["bomb"] = bombMesh;
	meshes["boat"] = boatMesh;
	meshes["books"] = booksMesh;
	meshes["boulder"] = boulderMesh;
	meshes["crate"] = crateMesh;
	meshes["palmtree"] = palmtreeMesh;
	meshes["lampcup"] = lampcupMesh;
	meshes["organizer"] = organizerMesh;
	meshes["map"] = mapMesh;
	meshes["marker"] = markerMesh;

	///////////////////////////////////////////////////////////////////////////
	////////////////////////////	TEXTURES	///////////////////////////////
	// Load textures (WIP)
	// Has to take char* due to ILUT

	char bombotTex[] = "Assets/img/bombot(diffuse)2-dash.png";
	char bombotSpec[] = "Assets/img/bombot_specular.png";
	std::shared_ptr<Texture> bombotTexMap = std::make_shared<Texture>(bombotTex, bombotSpec, 1.0f);

	char bombTex[] = "Assets/img/bomb(diffuse).jpg";
	std::shared_ptr<Texture> bombTexMap = std::make_shared<Texture>(bombTex, bombTex, 1.0f);

	char diffuseTex[] = "Assets/img/desk (diffuse).png";
	std::shared_ptr<Texture> deskTexMap = std::make_shared<Texture>(diffuseTex, diffuseTex, 1.0f);

	char corkboardTex[] = "Assets/img/corkboard(diffuse).png";
	std::shared_ptr<Texture> corkboardTexMap = std::make_shared<Texture>(corkboardTex, corkboardTex, 1.0f);

	char organizerTex[] = "Assets/img/organizer(diffuse).png";
	std::shared_ptr<Texture> organizerTexMap = std::make_shared<Texture>(organizerTex, organizerTex, 1.0f);

	char roomTex[] = "Assets/img/room(diffuse).png";
	std::shared_ptr<Texture> roomTexMap = std::make_shared<Texture>(roomTex, roomTex, 1.0f);

	char booksTex[] = "Assets/img/books(diffuse).png";
	std::shared_ptr<Texture> booksTexMap = std::make_shared<Texture>(booksTex, booksTex, 1.0f);

	char mapTex[] = "Assets/img/map(diffuse).png";
	std::shared_ptr<Texture> mapTexMap = std::make_shared<Texture>(mapTex, mapTex, 1.0f);

	char markerTex[] = "Assets/img/blake.png";
	std::shared_ptr<Texture> markerTexMap = std::make_shared<Texture>(markerTex, markerTex, 1.0f);

	char boulderTex[] = "Assets/img/boulder(diffuse).png";
	std::shared_ptr<Texture> boulderTexMap = std::make_shared<Texture>(boulderTex, boulderTex, 1.0f);

	char crateTex[] = "Assets/img/crate(diffuse).png";
	std::shared_ptr<Texture> crateTexMap = std::make_shared<Texture>(crateTex, crateTex, 1.0f);

	char palmtreeTex[] = "Assets/img/palmtree(diffuse).png";
	std::shared_ptr<Texture> palmtreeTexMap = std::make_shared<Texture>(palmtreeTex, palmtreeTex, 1.0f);

	char lampcupTex[] = "Assets/img/lampcup(diffuse).png";
	std::shared_ptr<Texture> lampcupTexMap = std::make_shared<Texture>(lampcupTex, lampcupTex, 1.0f);

	char barrelTex[] = "Assets/img/barrel(diffuse).jpg";
	std::shared_ptr<Texture> barrelTexMap = std::make_shared<Texture>(barrelTex, barrelTex, 1.0f);

	char cannonTex[] = "Assets/img/cannon(diffuse).jpg";
	std::shared_ptr<Texture> cannonTexMap = std::make_shared<Texture>(cannonTex, cannonTex, 1.0f);

	char boatTex[] = "Assets/img/boat(diffuse).png";
	std::shared_ptr<Texture> boatTexMap = std::make_shared<Texture>(boatTex, boatTex, 1.0f);

	char menuTex[] = "Assets/img/menu_atlas.png.png";
	std::shared_ptr<Texture> menuTexMap = std::make_shared<Texture>(menuTex, menuTex, 1.0f);



	//Add textures to the map
	textures["default"] = deskTexMap;
	textures["bombot"] = bombotTexMap;
	textures["corkboard"] = corkboardTexMap;
	textures["room"] = roomTexMap;
	textures["bomb1"] = bombTexMap;
	textures["barrel"] = barrelTexMap;
	textures["cannon"] = cannonTexMap;
	textures["boat"] = boatTexMap;
	textures["books"] = booksTexMap;
	textures["boulder"] = boulderTexMap;
	textures["crate"] = crateTexMap;
	textures["palmtree"] = palmtreeTexMap;
	textures["lampcup"] = lampcupTexMap;
	textures["organizer"] = organizerTexMap;
	textures["map"] = mapTexMap;
	textures["marker"] = markerTexMap;
	textures["menu"] = menuTexMap;


	///////////////////////////////////////////////////////////////////////////
	////////////////////////	GAME OBJECTS	///////////////////////////////
	auto defaultMaterial = materials["default"];

	// Set the Scene

	gameobjects["table"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), tableMesh, defaultMaterial, deskTexMap);

	gameobjects["sphere"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), sphereMesh, defaultMaterial, nullptr);

	gameobjects["corkboard"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), corkboardMesh, defaultMaterial, corkboardTexMap);

	gameobjects["organizer"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), organizerMesh, defaultMaterial, organizerTexMap);

	gameobjects["room"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), roomMesh, defaultMaterial, roomTexMap);

	gameobjects["books"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), booksMesh, defaultMaterial, booksTexMap);

	gameobjects["map"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), mapMesh, defaultMaterial, mapTexMap);

	gameobjects["marker"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), markerMesh, defaultMaterial, markerTexMap);

	gameobjects["lampcup"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), lampcupMesh, defaultMaterial, lampcupTexMap);

	gameobjects["sketch"] = std::make_shared<GameObject>(
		glm::vec3(-16.719f, 44.256f, -20.508f), nullptr, defaultMaterial, nullptr);
	gameobjects["sketch"]->setRotationAngleX(15.781 * degToRad);
	gameobjects["sketch"]->setRotationAngleY(-33.145 * degToRad);
	gameobjects["sketch"]->setRotationAngleZ(-0.142);

	gameobjects["trumps_wall"] = std::make_shared<GameObject>(
		glm::vec3(-48.796f, 46.3f, -14.61f), nullptr, defaultMaterial, nullptr);

	gameobjects["botwall"] = std::make_shared<GameObject>(
		glm::vec3(33.384f, 39.0f, 36.964f), nullptr, defaultMaterial, nullptr);

	gameobjects["topwall"] = std::make_shared<GameObject>(
		glm::vec3(33.384f, 39.0f, -44.964f), nullptr, defaultMaterial, nullptr);

	gameobjects["booksbody"] = std::make_shared<GameObject>(
		glm::vec3(-5.232f, 46.713f, -34.777f), nullptr, defaultMaterial, nullptr);

	// Build the play area

	//gameobjects["barrel"] = std::make_shared<GameObject>(
	//	glm::vec3(15.f, 42.0f, 10.f), barrelMesh, defaultMaterial, barrelTexMap);

	gameobjects["barrelTR"] = std::make_shared<GameObject>(
		glm::vec3(37.f, 42.0f, -2.f), barrelMesh, defaultMaterial, barrelTexMap);

	//gameobjects["barrelBR"] = std::make_shared<GameObject>(
	//	glm::vec3(40.f, 42.0f, 25.f), barrelMesh, defaultMaterial, barrelTexMap);

	//gameobjects["barrelBL"] = std::make_shared<GameObject>(
	//	glm::vec3(-15.f, 42.0f, 30.f), barrelMesh, defaultMaterial, barrelTexMap);
	//
	//gameobjects["barrelTL"] = std::make_shared<GameObject>(
	//	glm::vec3(-15.f, 42.0f, -10.f), barrelMesh, defaultMaterial, barrelTexMap);

	gameobjects["barrel1"] = std::make_shared<GameObject>(
		glm::vec3(-5.f, 42.0f, 23.f), barrelMesh, defaultMaterial, barrelTexMap);

	gameobjects["boulder2"] = std::make_shared<GameObject>(
		glm::vec3(-10.0f, 41.0f, 18.0f), boulderMesh, defaultMaterial, boulderTexMap);

	gameobjects["boulder"] = std::make_shared<GameObject>(
		glm::vec3(18.0f, 41.0f, 8.0f), boulderMesh, defaultMaterial, boulderTexMap);

	gameobjects["cannon"] = std::make_shared<GameObject>(
		glm::vec3(20.f, 40.5f, 15.f), cannonMesh, defaultMaterial, cannonTexMap);

	gameobjects["crate"] = std::make_shared<GameObject>(
		glm::vec3(13.0f, 40.0f, 14.0f), crateMesh, defaultMaterial, crateTexMap);

	gameobjects["crate2"] = std::make_shared<GameObject>(
		glm::vec3(10.0f, 40.0f, 9.0f), crateMesh, defaultMaterial, crateTexMap);

	gameobjects["boat"] = std::make_shared<GameObject>(
		glm::vec3(35.0f, 41.0f, 23.0f), boatMesh, defaultMaterial, boatTexMap);
	gameobjects["boat"]->setRotationAngleY(-45 * degToRad);

	//gameobjects["palmtree"] = std::make_shared<GameObject>(
	//	glm::vec3(0.0f, 45.0f, 0.0f), palmtreeMesh, defaultMaterial, palmtreeTexMap);




	players["bombot1"] = std::make_shared<Player>(
		glm::vec3(0.0f, 39.5f, 0.0f), bombotMesh, defaultMaterial, bombotTexMap, 0);
	gameobjects["bombot1"] = players["bombot1"];

	players["bombot2"] = std::make_shared<Player>(
		glm::vec3(10.0f, 39.5f, 0.0f), bombotMesh2, defaultMaterial, bombotTexMap, 1);
	gameobjects["bombot2"] = players["bombot2"];
	/*
	gameobjects["bombot3"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), bombotMesh, defaultMaterial, bombotTexMap, 2);
	gameobjects["bombot4"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), bombotMesh, defaultMaterial, bombotTexMap, 3);
	*/

	///////////////////////////////////////////////////////////////////////////
	////////////////////////	RIGID BODIES	///////////////////////////////

	// Create rigidbody paths
	std::string tableBodyPath = "assets\\bullet\\table.btdata";
	std::string bombotBodyPath = "assets\\bullet\\bombot.btdata";
	std::string sphereBodyPath = "assets\\bullet\\sphere.btdata";
	std::string bombBodyPath = "assets\\bullet\\bomb.btdata";
	std::string barrelBodyPath = "assets\\bullet\\barrel.btdata";
	std::string boulderBodyPath = "assets\\bullet\\scaledboulder.btdata";
	std::string crateBodyPath = "assets\\bullet\\scaledcrate.btdata";
	std::string cannonBodyPath = "assets\\bullet\\scaledcannon.btdata";
	std::string boatBodyPath = "assets\\bullet\\scaledboat.btdata";
	std::string sketchBodyPath = "assets\\bullet\\sketch.btdata";
	std::string trumpBodyPath = "assets\\bullet\\Trumps_wall.btdata";
	std::string booksBodyPath = "assets\\bullet\\books.btdata";
	std::string botwallBodyPath = "assets\\bullet\\botwall.btdata";
	std::string topwallBodyPath = "assets\\bullet\\topwall.btdata";

	// Create rigidbodies
	std::unique_ptr<RigidBody> tableBody;
	std::unique_ptr<RigidBody> bombot1Body;
	std::unique_ptr<RigidBody> bombot2Body;
	std::unique_ptr<RigidBody> sphereBody;
	std::unique_ptr<RigidBody> barrelBody;
	std::unique_ptr<RigidBody> barrel1Body;
	std::unique_ptr<RigidBody> boulderBody;
	std::unique_ptr<RigidBody> boulder2Body;
	std::unique_ptr<RigidBody> crateBody;
	std::unique_ptr<RigidBody> crate2Body;
	std::unique_ptr<RigidBody> cannonBody;
	std::unique_ptr<RigidBody> boatBody;
	std::unique_ptr<RigidBody> sketchBody;
	std::unique_ptr<RigidBody> trumpBody;
	std::unique_ptr<RigidBody> booksBody;
	std::unique_ptr<RigidBody> botwallBody;
	std::unique_ptr<RigidBody> topwallBody;

	tableBody = std::make_unique<RigidBody>();
	bombot1Body = std::make_unique<RigidBody>(btBroadphaseProxy::CharacterFilter);
	bombot2Body = std::make_unique<RigidBody>(btBroadphaseProxy::CharacterFilter);
	sphereBody = std::make_unique<RigidBody>();
	barrelBody = std::make_unique<RigidBody>();
	barrel1Body = std::make_unique<RigidBody>();
	boulderBody = std::make_unique<RigidBody>();
	boulder2Body = std::make_unique<RigidBody>();
	crateBody = std::make_unique<RigidBody>();
	crate2Body = std::make_unique<RigidBody>();
	cannonBody = std::make_unique<RigidBody>();
	boatBody = std::make_unique<RigidBody>();
	sketchBody = std::make_unique<RigidBody>();
	trumpBody = std::make_unique<RigidBody>();
	booksBody = std::make_unique<RigidBody>();
	botwallBody = std::make_unique<RigidBody>();
	topwallBody = std::make_unique<RigidBody>();

	// Load rigidbodies
	tableBody->load(tableBodyPath);
	bombot1Body->load(bombotBodyPath);
	bombot2Body->load(bombotBodyPath);
	sphereBody->load(sphereBodyPath, btCollisionObject::CF_KINEMATIC_OBJECT);
	barrelBody->load(barrelBodyPath);
	barrel1Body->load(barrelBodyPath);
	boulderBody->load(boulderBodyPath);
	boulder2Body->load(boulderBodyPath);
	crateBody->load(crateBodyPath);
	crate2Body->load(crateBodyPath);
	cannonBody->load(cannonBodyPath);
	boatBody->load(boatBodyPath);
	sketchBody->load(sketchBodyPath);
	trumpBody->load(trumpBodyPath);
	booksBody->load(booksBodyPath);
	botwallBody->load(botwallBodyPath);
	topwallBody->load(topwallBodyPath);

	// Attach rigidbodies
	gameobjects["table"]->attachRigidBody(tableBody);
	gameobjects["bombot1"]->attachRigidBody(bombot1Body);
	gameobjects["bombot2"]->attachRigidBody(bombot2Body);
	gameobjects["sphere"]->attachRigidBody(sphereBody);
	gameobjects["barrelTR"]->attachRigidBody(barrelBody);
	gameobjects["barrel1"]->attachRigidBody(barrel1Body);
	gameobjects["boulder"]->attachRigidBody(boulderBody);
	gameobjects["boulder2"]->attachRigidBody(boulder2Body);
	gameobjects["crate"]->attachRigidBody(crateBody);
	gameobjects["crate2"]->attachRigidBody(crate2Body);
	gameobjects["cannon"]->attachRigidBody(cannonBody);
	gameobjects["boat"]->attachRigidBody(boatBody);
	gameobjects["sketch"]->attachRigidBody(sketchBody);
	gameobjects["trumps_wall"]->attachRigidBody(trumpBody);
	gameobjects["booksbody"]->attachRigidBody(booksBody);
	gameobjects["botwall"]->attachRigidBody(botwallBody);
	gameobjects["topwall"]->attachRigidBody(topwallBody);

	///////////////////////////////////////////////////////////////////////////
	////////////////////////	PROPERTIES		///////////////////////////////

	// Set up the bomb manager
	bombManager = std::make_shared<BombManager>();
	bombManager->init(bombMesh,
		bombTexMap,		// Player1 bomb texture
		bombTexMap,		// Player2 bomb texture
		bombTexMap,		// Player3 bomb texture
		bombTexMap,		// Player4 bomb texture
		sphereMesh,		// Explosion mesh
		markerTexMap,	// Explosion texture
		sphereBodyPath,	// Explosion rigidbody
		defaultMaterial,
		bombBodyPath);

	players["bombot1"]->attachBombManager(bombManager);
	players["bombot2"]->attachBombManager(bombManager);

	// Set the outline colors
	gameobjects["bombot1"]->setOutlineColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	gameobjects["bombot2"]->setOutlineColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Set up the bullet callbacks
	RigidBody::getDispatcher()->setNearCallback((btNearCallback)bulletNearCallback);

	// Set menu properties
	menu = std::make_shared<Menu>(markerTexMap);
	menu->setMaterial(materials["menu"]);


	// Set default camera properties (WIP)



}

void bulletNearCallback(btBroadphasePair& collisionPair,
	btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo)
{
	// Do your collision logic here
	// Only dispatch the Bullet collision information if you want the physics to continue
	// From Bullet user manual

	if (collisionPair.m_pProxy0->m_collisionFilterGroup == btBroadphaseProxy::SensorTrigger &&
		collisionPair.m_pProxy1->m_collisionFilterGroup == btBroadphaseProxy::DebrisFilter ||
		collisionPair.m_pProxy0->m_collisionFilterGroup == btBroadphaseProxy::DebrisFilter &&
		collisionPair.m_pProxy1->m_collisionFilterGroup == btBroadphaseProxy::SensorTrigger)
	{
		return;
	}

	// Tell the dispatcher to do the collision information
	dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
}

void collideWithCorrectType(Player* player, GameObject* object)
{
	switch (object->getColliderType())
	{
	case COLLIDER_DEFAULT:
		break;
	case PLAYER:
		player->checkCollisionWith((Player*)object);
		break;
	case BOMB_BASE:
		player->checkCollisionWith((Bomb*)object);
		break;
	case BOMB_EXPLOSION:
		player->checkCollisionWith((Explosion*)object);
		break;
	default:
		break;
	}
}

void calculateCollisions()
{
	// Basis taken from 
	// http://www.bulletphysics.org/mediawiki-1.5.8/index.php?title=Collision_Callbacks_and_Triggers
	btDispatcher* dispatcher = RigidBody::getDispatcher();
	int numManifolds = dispatcher->getNumManifolds();

	short objAGroup, objBGroup;

	for (int i = 0; i < numManifolds; i++)
	{
		//std::cout << numManifolds << std::endl;
		btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);
		const btCollisionObject* objA = contactManifold->getBody0();
		const btCollisionObject* objB = contactManifold->getBody1();

		objAGroup = objA->getBroadphaseHandle()->m_collisionFilterGroup;
		objBGroup = objB->getBroadphaseHandle()->m_collisionFilterGroup;

		// Check if the collision isn't with geometry
		if (!objA->isStaticObject() &&
			!objB->isStaticObject())
		{
			// Tell the player to collide with stuff
			if (objAGroup == btBroadphaseProxy::CharacterFilter)
			{
				Player* p = (Player*)objA->getUserPointer();
				collideWithCorrectType(p, (GameObject*)objB->getUserPointer());
			}
			else if (objBGroup == btBroadphaseProxy::CharacterFilter)
			{
				Player* p = (Player*)objB->getUserPointer();
				collideWithCorrectType(p, (GameObject*)objA->getUserPointer());
			}
		}
	}
}



// This is where we draw stuff
void DisplayCallbackFunction(void)
{


	///////////Shadow Map
	//shadowMap.bindFrameBufferForDrawing();
	//FrameBufferObject::clearFrameBuffer(clearColor);

	//setMaterialForAllGameObjects("shadow");
	//materials["shadow"]->mat4Uniforms["u_bias"] = biasMatrix;

	//drawScene(playerCamera);

	//shadowMap.unbindFrameBuffer(windowWidth, windowHeight);

	// bind scene FBO





	states.draw();

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
	

	// Update the camera's position
	playerCamera.update();

	states.update(deltaTime);

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
	//windowWidth = w;
	//windowHeight = h;

	// TODO: more needed?

	//// switch to projection because we're changing projection
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(45.0f, (float)w / h, 0.1f, 10000.0f);
	//camera.setProperties(45.0f, float(w / h), 0.1f, 10000.0f, 0.01f);
	//glViewport(0, 0, w, h);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//playerCamera.setRatio(windowHeight, windowWidth);
}


void MouseClickCallbackFunction(int button, int state, int x, int y)
{
	mousePosition.x = x;
	mousePosition.y = y;

	//mousePositionFlipped = mousePosition;
	//mousePositionFlipped.y = windowHeight - mousePosition.y;
}

// Called when the mouse is clicked and moves
void MouseMotionCallbackFunction(int x, int y)
{
	/*if (mousePosition.length() > 0)
		playerCamera.processMouseMotion(x, y, mousePosition.x, mousePosition.y, deltaTime);*/

	mousePosition.x = x;
	mousePosition.y = y;

	//mousePositionFlipped = mousePosition;
	//mousePositionFlipped.y = windowHeight - mousePosition.y;
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
	glutInitWindowSize(1920, 1080);
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


	/* Start Game Loop */
	deltaTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime /= 1000.0f;

	mainMenu = new MainMenu(menu);
	mainMenu->setPaused(false);

	pause = new Pause(menu);
	pause->setPaused(true);

	score = new Score(menu);
	score->setPaused(true);

	game = new Game(&gameobjects, &players, &materials, bombManager, pause, score, &playerCamera);
	game->setPaused(true);

	states.addGameState("MainMenu", mainMenu);
	states.addGameState("game", game);
	states.addGameState("pause", pause);
	states.addGameState("score", score);

	glutMainLoop();
	return 0;
}
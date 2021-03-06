// Graham Watson 100522240
// Robert Savaglio 100591436

// Core Libraries
#include <iostream>
#include <string>
#include <math.h>
#include <map> // for std::map
#include <memory> // for std::shared_ptr
#include <chrono> // for time

// 3rd Party Libraries
#include <GLEW\glew.h>
#include "gl\freeglut.h"
#include <IL/il.h> // for ilInit()
#include <IL\ilu.h>
#include <glm\vec3.hpp>
#include <glm\gtx\color_space.hpp>
//#include "vld.h"

// User Libraries
#include "Shader.h"
#include "ShaderProgram.h"
#include "loadObject.h"
#include "InputManager.h"
#include "ANILoader.h"

#include "MainMenuState.h"
#include "MainGameState.h"

// aStar Libs
#include "aStar.h"

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
std::vector<std::shared_ptr<GameObject>> obstacles;
std::vector<std::shared_ptr<GameObject>> readyUpRings;
std::map<std::string, std::shared_ptr<Player>> players;
std::map<std::string, std::shared_ptr<Texture>> textures;

//Sound Stuff
//SoundDriver* audio_Driver;
std::map<std::string, Sound> soundTemplates;

// Materials
std::map<std::string, std::shared_ptr<Material>> materials;

// Controls
bool inMenu = false;
std::shared_ptr<Menu> startMenu;
std::shared_ptr<Menu> scoreMenu;
std::shared_ptr<Menu> scoreMenu2;
std::shared_ptr<Menu> pauseMenu;

std::shared_ptr<BombManager> bombManager;

void bulletNearCallback(btBroadphasePair& collisionPair,
	btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo);

void initializeShaders()
{
	std::string shaderPath = "Shaders/";

	// Load shaders

	// Vertex Shaders
	Shader v_default, v_passThru, v_null, v_skinning, v_particle;
	v_default.loadShaderFromFile(shaderPath + "default_v.glsl", GL_VERTEX_SHADER);
	v_passThru.loadShaderFromFile(shaderPath + "passThru_v.glsl", GL_VERTEX_SHADER);
	v_null.loadShaderFromFile(shaderPath + "null.vert", GL_VERTEX_SHADER);
	v_skinning.loadShaderFromFile(shaderPath + "skinning.vert", GL_VERTEX_SHADER);
	v_particle.loadShaderFromFile(shaderPath + "particles_v.glsl", GL_VERTEX_SHADER);

	// Fragment Shaders
	Shader f_default, f_unlitTex, f_bright, f_composite, f_blur, f_texColor,
		f_noLighting, f_toon, f_outline, f_sobel, f_shadow, f_colorCorrection, 
		f_particles, f_combination, f_bokeh, f_bokehComp, f_node;
	f_default.loadShaderFromFile(shaderPath + "default_f.glsl", GL_FRAGMENT_SHADER);//
	f_bright.loadShaderFromFile(shaderPath + "bright_f.glsl", GL_FRAGMENT_SHADER);//
	f_unlitTex.loadShaderFromFile(shaderPath + "unlitTexture_f.glsl", GL_FRAGMENT_SHADER);
	f_composite.loadShaderFromFile(shaderPath + "pls.glsl", GL_FRAGMENT_SHADER);//
	f_particles.loadShaderFromFile(shaderPath + "test_f.glsl", GL_FRAGMENT_SHADER);
	f_blur.loadShaderFromFile(shaderPath + "gaussianBlur_f.glsl", GL_FRAGMENT_SHADER); //a
	f_texColor.loadShaderFromFile(shaderPath + "shader_texture.frag", GL_FRAGMENT_SHADER);//
	f_noLighting.loadShaderFromFile(shaderPath + "noLighting_f.glsl", GL_FRAGMENT_SHADER);//
	f_toon.loadShaderFromFile(shaderPath + "toon_f.glsl", GL_FRAGMENT_SHADER);//
	f_outline.loadShaderFromFile(shaderPath + "outline_f.glsl", GL_FRAGMENT_SHADER);//
	f_shadow.loadShaderFromFile(shaderPath + "shadowMap_f.glsl", GL_FRAGMENT_SHADER);//
	f_colorCorrection.loadShaderFromFile(shaderPath + "color_f.glsl", GL_FRAGMENT_SHADER);//
	f_combination.loadShaderFromFile(shaderPath + "combinataion_f.glsl", GL_FRAGMENT_SHADER);
	f_node.loadShaderFromFile(shaderPath + "node.frag", GL_FRAGMENT_SHADER);
	f_bokeh.loadShaderFromFile(shaderPath + "bokeh_f.glsl", GL_FRAGMENT_SHADER);//
	f_bokehComp.loadShaderFromFile(shaderPath + "bokehComposite_f.glsl", GL_FRAGMENT_SHADER);//

	// Geometry Shaders
	Shader g_quad, g_menu, g_node, g_particles;
	g_quad.loadShaderFromFile(shaderPath + "quad.geom", GL_GEOMETRY_SHADER);
	g_menu.loadShaderFromFile(shaderPath + "menu.geom", GL_GEOMETRY_SHADER);
	g_node.loadShaderFromFile(shaderPath + "node.geom", GL_GEOMETRY_SHADER);
	g_particles.loadShaderFromFile(shaderPath + "particles.geom", GL_GEOMETRY_SHADER);

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
	materials["toon"]->shader->attachShader(v_skinning);
	materials["toon"]->shader->attachShader(f_toon);
	materials["toon"]->shader->linkProgram();

	// Applies color correction to the scene
	materials["colorCorrection"] = std::make_shared<Material>("colorCorrection");
	materials["colorCorrection"]->shader->attachShader(v_passThru);
	materials["colorCorrection"]->shader->attachShader(f_colorCorrection);
	materials["colorCorrection"]->shader->attachShader(g_quad);
	materials["colorCorrection"]->shader->linkProgram();


	// Material used for menu full screen drawing
	materials["menu"] = std::make_shared<Material>("menu");
	materials["menu"]->shader->attachShader(v_null);
	materials["menu"]->shader->attachShader(f_texColor);
	materials["menu"]->shader->attachShader(g_menu);
	materials["menu"]->shader->linkProgram();

	// Material used for node drawing
	materials["node"] = std::make_shared<Material>("menu");
	materials["node"]->shader->attachShader(v_passThru);
	materials["node"]->shader->attachShader(f_node);
	materials["node"]->shader->attachShader(g_node);
	materials["node"]->shader->linkProgram();

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
	materials["outline"]->shader->attachShader(v_skinning);
	materials["outline"]->shader->attachShader(f_outline);
	materials["outline"]->shader->linkProgram();

	// Shadow filter material
	materials["shadow"] = std::make_shared<Material>("shadow");
	materials["shadow"]->shader->attachShader(v_skinning);
	materials["shadow"]->shader->attachShader(f_shadow);
	materials["shadow"]->shader->linkProgram();

	// Unlit texture material with point-to-quad geometry shader
	materials["particles"] = std::make_shared<Material>("particles");
	materials["particles"]->shader->attachShader(v_particle);
	materials["particles"]->shader->attachShader(g_particles); // Geometry Shader!
	materials["particles"]->shader->attachShader(f_particles);
	materials["particles"]->shader->linkProgram();

	materials["bokeh"] = std::make_shared<Material>("bokeh");
	materials["bokeh"]->shader->attachShader(v_passThru);
	materials["bokeh"]->shader->attachShader(g_quad); // Geometry Shader!
	materials["bokeh"]->shader->attachShader(f_bokeh);
	materials["bokeh"]->shader->linkProgram();
	
	materials["combine"] = std::make_shared<Material>("particleCombination");
	materials["combine"]->shader->attachShader(v_passThru); 
	materials["combine"]->shader->attachShader(g_quad); 
	materials["combine"]->shader->attachShader(f_combination);
	materials["combine"]->shader->linkProgram();

	materials["bokehComp"] = std::make_shared<Material>("bokehComp");
	materials["bokehComp"]->shader->attachShader(v_passThru);
	materials["bokehComp"]->shader->attachShader(g_quad); // Geometry Shader!
	materials["bokehComp"]->shader->attachShader(f_bokehComp);
	materials["bokehComp"]->shader->linkProgram();

}

void initializeScene()
{
	//aStar.initializePathNodes();

	///////////////////////////////////////////////////////////////////////////
	////////////////////////////	MESHES		///////////////////////////////
	std::string meshPath = "Assets/obj/";

	// Initialize all meshes
	std::shared_ptr<LoadObject> tableMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> barrelMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> cannonMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> sphereMesh = std::make_shared<LoadObject>();
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
	std::shared_ptr<LoadObject> readyTopMesh = std::make_shared<LoadObject>();
	std::shared_ptr<LoadObject> readyBottomMesh = std::make_shared<LoadObject>();

	std::shared_ptr<Holder> bombotMesh = std::make_shared<Holder>();
	std::shared_ptr<Holder> bombotMesh2 = std::make_shared<Holder>();
	std::shared_ptr<Holder> bombotMesh3 = std::make_shared<Holder>();
	std::shared_ptr<Holder> bombotMesh4 = std::make_shared<Holder>();

	// Test animation load times
	std::cout << "Loading animations...";
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	// function wrapper for loading animations. makes less cluttered
	loadAnimations(bombotMesh);
	loadAnimations(bombotMesh2);
	loadAnimations(bombotMesh3);
	loadAnimations(bombotMesh4);

	// Report animation load times
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << " success, total " << duration << "ms taken" << std::endl;

	// Test mesh load times
	std::cout << "Loading meshes...";
	t1 = std::chrono::high_resolution_clock::now();

	// Load all meshes
	tableMesh->load(meshPath + "translatedtable.obj");
	barrelMesh->load(meshPath + "barrel.obj");
	cannonMesh->load(meshPath + "scaledcannon.obj");
	sphereMesh->load(meshPath + "sphere.obj");
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
	readyTopMesh->load(meshPath + "topLeft.obj");
	readyBottomMesh->load(meshPath + "bottomLeft.obj");

	// Report mesh load times
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << " success, total " << duration << "ms taken" << std::endl << std::endl;

	// Add all meshes to map
	meshes["table"] = tableMesh;
	meshes["barrel"] = barrelMesh;
	meshes["cannon"] = cannonMesh;
	meshes["sphere"] = sphereMesh;
	meshes["bombot"] = bombotMesh;
	meshes["bombot2"] = bombotMesh2;
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
	meshes["readyTop"] = readyTopMesh;
	meshes["readyBottom"] = readyBottomMesh;

	///////////////////////////////////////////////////////////////////////////
	////////////////////////////	TEXTURES	///////////////////////////////
	// Load textures (WIP)
	// Has to take char* due to ILUT (DOESN'T HAVE TO ANYMORE)

	// Test texture load times
	std::cout << "Loading textures..." << std::endl;
	t1 = std::chrono::high_resolution_clock::now();

	std::string bombot1Tex = "Assets/img/bombot(diffuse)2.png";
	std::string bombot2Tex = "Assets/img/bombot(diffuse)1.png";
	std::string bombot3Tex = "Assets/img/bombot(diffuse)3.png";
	std::string bombot4Tex = "Assets/img/bombot(diffuse)4.png";
	std::string bombotSpec = "Assets/img/bombot(spec)2-dash.png";

	std::shared_ptr<Texture> bombot1TexMap = std::make_shared<Texture>(bombot1Tex, bombotSpec, 1.0f);
	std::shared_ptr<Texture> bombot2TexMap = std::make_shared<Texture>(bombot2Tex, bombotSpec, 1.0f);
	std::shared_ptr<Texture> bombot3TexMap = std::make_shared<Texture>(bombot3Tex, bombotSpec, 1.0f);
	std::shared_ptr<Texture> bombot4TexMap = std::make_shared<Texture>(bombot4Tex, bombotSpec, 1.0f);

	std::string bombTex1 = "Assets/img/bomb1.png";
	std::shared_ptr<Texture> bombTexMap1 = std::make_shared<Texture>(bombTex1, bombTex1, 1.0f);

	std::string bombTex2 = "Assets/img/bomb2.png";
	std::shared_ptr<Texture> bombTexMap2 = std::make_shared<Texture>(bombTex2, bombTex2, 1.0f);

	std::string bombTex3 = "Assets/img/bomb3.png";
	std::shared_ptr<Texture> bombTexMap3 = std::make_shared<Texture>(bombTex3, bombTex3, 1.0f);

	std::string bombTex4 = "Assets/img/bomb4.png";
	std::shared_ptr<Texture> bombTexMap4 = std::make_shared<Texture>(bombTex4, bombTex4, 1.0f);

	std::string explosionTex1 = "Assets/img/ex-1.png";
	std::shared_ptr<Texture> explosionTexMap1 = std::make_shared<Texture>(explosionTex1, explosionTex1, 1.0f);

	std::string explosionTex2 = "Assets/img/ex-2.png";
	std::shared_ptr<Texture> explosionTexMap2 = std::make_shared<Texture>(explosionTex2, explosionTex2, 1.0f);

	std::string explosionTex3 = "Assets/img/ex-3.png";
	std::shared_ptr<Texture> explosionTexMap3 = std::make_shared<Texture>(explosionTex3, explosionTex3, 1.0f);

	std::string explosionTex4 = "Assets/img/ex-4.png";
	std::shared_ptr<Texture> explosionTexMap4 = std::make_shared<Texture>(explosionTex4, explosionTex4, 1.0f);

	std::string readyTex1 = "Assets/img/rg.png";
	std::shared_ptr<Texture> readyTexMap1 = std::make_shared<Texture>(readyTex1, readyTex1, 1.0f);

	std::string readyTex2 = "Assets/img/yb.png";
	std::shared_ptr<Texture> readyTexMap2 = std::make_shared<Texture>(readyTex2, readyTex2, 1.0f);

	std::string winTex = "Assets/img/win(scaled).png";
	std::shared_ptr<Texture> winTexMap = std::make_shared<Texture>(winTex, winTex, 1.0f);

	std::string deskTex = "Assets/img/desk (diffuse).png";
	std::shared_ptr<Texture> deskTexMap = std::make_shared<Texture>(deskTex, deskTex, 1.0f);

	std::string deskReadyTex = "Assets/img/deskReady (diffuse).png";
	std::shared_ptr<Texture> deskReadyTexMap = std::make_shared<Texture>(deskReadyTex, deskReadyTex, 1.0f);

	std::string organizerTex = "Assets/img/organizer(diffuse).png";
	std::shared_ptr<Texture> organizerTexMap = std::make_shared<Texture>(organizerTex, organizerTex, 1.0f);

	std::string roomTex = "Assets/img/room(diffuse).png";
	std::shared_ptr<Texture> roomTexMap = std::make_shared<Texture>(roomTex, roomTex, 1.0f);

	std::string booksTex = "Assets/img/books(diffuse).png";
	std::shared_ptr<Texture> booksTexMap = std::make_shared<Texture>(booksTex, booksTex, 1.0f);

	std::string mapTex = "Assets/img/map(diffuse).png";
	std::shared_ptr<Texture> mapTexMap = std::make_shared<Texture>(mapTex, mapTex, 1.0f);

	std::string boulderTex = "Assets/img/boulder(diffuse).png";
	std::shared_ptr<Texture> boulderTexMap = std::make_shared<Texture>(boulderTex, boulderTex, 1.0f);

	std::string crateTex = "Assets/img/crate(diffuse).png";
	std::shared_ptr<Texture> crateTexMap = std::make_shared<Texture>(crateTex, crateTex, 1.0f);

	std::string palmtreeTex = "Assets/img/palmtree(diffuse).png";
	std::shared_ptr<Texture> palmtreeTexMap = std::make_shared<Texture>(palmtreeTex, palmtreeTex, 1.0f);

	std::string lampcupTex = "Assets/img/lampcup(diffuse).png";
	std::shared_ptr<Texture> lampcupTexMap = std::make_shared<Texture>(lampcupTex, lampcupTex, 1.0f);

	std::string barrelTex = "Assets/img/barrel(diffuse).jpg";
	std::shared_ptr<Texture> barrelTexMap = std::make_shared<Texture>(barrelTex, barrelTex, 1.0f);

	std::string cannonTex = "Assets/img/cannon(diffuse).jpg";
	std::shared_ptr<Texture> cannonTexMap = std::make_shared<Texture>(cannonTex, cannonTex, 1.0f);

	std::string boatTex = "Assets/img/boat(diffuse).png";
	std::shared_ptr<Texture> boatTexMap = std::make_shared<Texture>(boatTex, boatTex, 1.0f);

	std::string smoke = "Assets/img/smoke.png";
	std::shared_ptr<Texture> smokeTexMap = std::make_shared<Texture>(smoke, smoke, 1.0f);

	std::string spark = "Assets/img/sparks.png";
	std::shared_ptr<Texture> sparkTexMap = std::make_shared<Texture>(spark, spark, 1.0f);

	std::string explosion = "Assets/img/explosion.png";
	std::shared_ptr<Texture> explosionTexMap = std::make_shared<Texture>(explosion, explosion, 1.0f);

	// Report texture load times
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << "success, texture loading took " << duration << "ms" << std::endl << std::endl;

	//Add textures to the map
	textures["table"] = deskTexMap;
	textures["readyTable"] = deskReadyTexMap;
	textures["bombot1"] = bombot1TexMap;
	textures["bombot2"] = bombot2TexMap;
	textures["bombot3"] = bombot3TexMap;
	textures["bombot4"] = bombot4TexMap;
	textures["room"] = roomTexMap;
	textures["bomb1"] = bombTexMap1;
	textures["bomb2"] = bombTexMap2;
	textures["bomb3"] = bombTexMap3;
	textures["bomb4"] = bombTexMap4;
	textures["explosion1"] = explosionTexMap1;
	textures["explosion2"] = explosionTexMap2;
	textures["explosion3"] = explosionTexMap3;
	textures["explosion4"] = explosionTexMap4;
	textures["readyL"] = readyTexMap1;
	textures["readyR"] = readyTexMap2;
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
	textures["smoke"] = smokeTexMap;
	textures["spark"] = sparkTexMap;
	textures["win"] = winTexMap;
	textures["explosion"] = explosionTexMap;

	///////////////////////////////////////////////////////////////////////////
	////////////////////////		SOUNDS		///////////////////////////////

	// Test sound  load times
	std::cout << "Initializing sounds...";
	t1 = std::chrono::high_resolution_clock::now();

	std::string soundPath = "assets/media/";
	//soundTemplates[""] = Sound(soundPath + ".wav", false);

	// Menu sounds
	soundTemplates["m_mainMenu"] = Sound(soundPath + "MenuTheme.wav");
	soundTemplates["s_menuSelect"] = Sound(soundPath + "select_fx.wav", false);
	soundTemplates["s_menuSwitch"] = Sound(soundPath + "switch_fx.wav", false);

	// Battle music
	soundTemplates["m_readyMusic"] = Sound(soundPath + "MenuTheme.wav", true);
	soundTemplates["m_gameTrack1"] = Sound(soundPath + "Music Layers/layer_1.wav", true);
	soundTemplates["m_gameTrack2"] = Sound(soundPath + "Music Layers/layer_2.wav", true);
	soundTemplates["m_gameTrack3"] = Sound(soundPath + "Music Layers/layer_3.wav", true);

	soundTemplates["s_countdown"] = Sound(soundPath + "3_2_1_DodgeBomb.wav", false);
	soundTemplates["s_lightOn"] = Sound(soundPath + "light_on.wav", false);

	// Battle SFX
	soundTemplates["s_bombExplosion1"] = Sound(soundPath + "bomb sounds/bomb_1.wav", false);
	soundTemplates["s_bombExplosion2"] = Sound(soundPath + "bomb sounds/bomb_2.wav", false);
	soundTemplates["s_bombExplosion3"] = Sound(soundPath + "bomb sounds/bomb_3.wav", false);
	soundTemplates["s_bombExplosion4"] = Sound(soundPath + "bomb sounds/bomb_4.wav", false);
	soundTemplates["s_bombExplosion5"] = Sound(soundPath + "bomb sounds/bomb_5.wav", false);
	soundTemplates["s_bombExplosion6"] = Sound(soundPath + "bomb sounds/bomb_6.wav", false);
	soundTemplates["s_bombExplosion7"] = Sound(soundPath + "bomb sounds/bomb_7.wav", false);
	soundTemplates["s_bombExplosion8"] = Sound(soundPath + "bomb sounds/bomb_8.wav", false);

	soundTemplates["s_damage1"] = Sound(soundPath + "Hit sounds/blue_hit.wav", false);
	soundTemplates["s_damage2"] = Sound(soundPath + "Hit sounds/red_hit.wav", false);
	soundTemplates["s_damage3"] = Sound(soundPath + "Hit sounds/green_hit.wav", false);
	soundTemplates["s_damage4"] = Sound(soundPath + "Hit sounds/yellow_hit.wav", false);

	soundTemplates["s_footstep1"] = Sound(soundPath + "walk sounds/blue_walk.wav", true);
	soundTemplates["s_footstep2"] = Sound(soundPath + "walk sounds/red_walk.wav", true);
	soundTemplates["s_footstep3"] = Sound(soundPath + "walk sounds/green_walk.wav", true);
	soundTemplates["s_footstep4"] = Sound(soundPath + "walk sounds/yellow_walk.wav", true);

	soundTemplates["d_ready1_1"] = Sound(soundPath + "Bombot Dialogue/blue_ready_1.wav", false);
	soundTemplates["d_ready2_1"] = Sound(soundPath + "Bombot Dialogue/red_ready_1.wav", false);
	soundTemplates["d_ready3_1"] = Sound(soundPath + "Bombot Dialogue/green_ready_1.wav", false);
	soundTemplates["d_ready4_1"] = Sound(soundPath + "Bombot Dialogue/yellow_ready_1.wav", false);

	soundTemplates["d_ready1_2"] = Sound(soundPath + "Bombot Dialogue/blue_ready_2.wav", false);
	soundTemplates["d_ready2_2"] = Sound(soundPath + "Bombot Dialogue/red_ready_2.wav", false);
	soundTemplates["d_ready3_2"] = Sound(soundPath + "Bombot Dialogue/green_ready_2.wav", false);
	soundTemplates["d_ready4_2"] = Sound(soundPath + "Bombot Dialogue/yellow_ready_2.wav", false);

	soundTemplates["d_win1_1"] = Sound(soundPath + "Bombot Dialogue/blue_win_1.wav", false);
	soundTemplates["d_win2_1"] = Sound(soundPath + "Bombot Dialogue/red_win_1.wav", false);
	soundTemplates["d_win3_1"] = Sound(soundPath + "Bombot Dialogue/green_win_1.wav", false);
	soundTemplates["d_win4_1"] = Sound(soundPath + "Bombot Dialogue/yellow_win_1.wav", false);

	soundTemplates["d_win1_2"] = Sound(soundPath + "Bombot Dialogue/blue_win_2.wav", false);
	soundTemplates["d_win2_2"] = Sound(soundPath + "Bombot Dialogue/red_win_2.wav", false);
	soundTemplates["d_win3_2"] = Sound(soundPath + "Bombot Dialogue/green_win_2.wav", false);
	soundTemplates["d_win4_2"] = Sound(soundPath + "Bombot Dialogue/yellow_win_2.wav", false);

	Sound::sys.update();


	// Report sound init times
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << " success, " << duration << "ms taken" << std::endl;

	///////////////////////////////////////////////////////////////////////////
	////////////////////////	GAME OBJECTS	///////////////////////////////
	auto defaultMaterial = materials["default"];
	
	// Set the Scene

	// Test gameobject load times
	std::cout << "Initializing gameObjects...";
	t1 = std::chrono::high_resolution_clock::now();

	gameobjects["readyBlue"] = std::make_shared<readyUpRing>(
		glm::vec3(0.0f, -11.0f, -16.0f), readyBottomMesh, defaultMaterial, readyTexMap2, 0);
	gameobjects["readyBlue"]->setScale(glm::vec3(0.5f));
	gameobjects["readyBlue"]->setRotationAngleY(90.0f * degToRad);
	readyUpRings.push_back(gameobjects["readyBlue"]);

	gameobjects["readyRed"] = std::make_shared<readyUpRing>(
		glm::vec3(45.0f, -11.0f, -16.0f), readyTopMesh, defaultMaterial, readyTexMap1, 1);
	gameobjects["readyRed"]->setRotationAngleY(90.0f * degToRad);
	gameobjects["readyRed"]->setScale(glm::vec3(0.5f));
	readyUpRings.push_back(gameobjects["readyRed"]);

	gameobjects["readyGreen"] = std::make_shared<readyUpRing>(
		glm::vec3(-7.0f, -11.0f, 13.0f), readyBottomMesh, defaultMaterial, readyTexMap1, 2);
	gameobjects["readyGreen"]->setScale(glm::vec3(0.5f));
	readyUpRings.push_back(gameobjects["readyGreen"]);

	gameobjects["readyYellow"] = std::make_shared<readyUpRing>(
		glm::vec3(53.0f, -11.0f, 13.0f), readyTopMesh, defaultMaterial, readyTexMap2, 3);
	gameobjects["readyYellow"]->setRotationAngleY(180.0f * degToRad);
	gameobjects["readyYellow"]->setScale(glm::vec3(0.5f));
	readyUpRings.push_back(gameobjects["readyYellow"]);

	gameobjects["table"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), tableMesh, defaultMaterial, deskTexMap);

	gameobjects["sphere"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 5.0f, 0.0f), sphereMesh, defaultMaterial, nullptr);

	gameobjects["organizer"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), organizerMesh, defaultMaterial, organizerTexMap);

	gameobjects["room"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), roomMesh, defaultMaterial, roomTexMap);

	gameobjects["books"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), booksMesh, defaultMaterial, booksTexMap);

	gameobjects["map"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), mapMesh, defaultMaterial, mapTexMap);

	gameobjects["marker"] = std::make_shared<GameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f), markerMesh, defaultMaterial, lampcupTexMap);

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
		glm::vec3(33.384f, 39.0f, 39.964f), nullptr, defaultMaterial, nullptr);

	gameobjects["botleftwall"] = std::make_shared<GameObject>(
		glm::vec3(-17.384f, 39.0f, 39.964f), nullptr, defaultMaterial, nullptr);
	gameobjects["botleftwall"]->setRotationAngleY(70 * degToRad);

	gameobjects["rightwall"] = std::make_shared<GameObject>(
		glm::vec3(67.384f, 39.0f, -9.964f), nullptr, defaultMaterial, nullptr);
	gameobjects["rightwall"]->setRotationAngleY(90 * degToRad);

	gameobjects["topwall"] = std::make_shared<GameObject>(
		glm::vec3(33.384f, 39.0f, -44.964f), nullptr, defaultMaterial, nullptr);

	gameobjects["booksbody"] = std::make_shared<GameObject>(
		glm::vec3(-5.232f, 46.713f, -34.777f), nullptr, defaultMaterial, nullptr);

	gameobjects["lamp"] = std::make_shared<GameObject>(
		glm::vec3(67.9f, 38.756f, -31.772f), nullptr, defaultMaterial, nullptr);

	//////////////////////////////////////////////////////////////////////////////
	// Build the play area

	//gameobjects["barrel"] = std::make_shared<GameObject>(
	//	glm::vec3(15.f, 42.0f, 10.f), barrelMesh, defaultMaterial, barrelTexMap);

	gameobjects["barrelTR"] = std::make_shared<GameObject>(
		//glm::vec3(23.0f, 45.0f, 0.0f), barrelMesh, defaultMaterial, barrelTexMap);
		glm::vec3(37.f, -20.0f, -2.0f), barrelMesh, defaultMaterial, barrelTexMap);
	gameobjects["barrelTR"]->setScale(glm::vec3(1.2));
	obstacles.push_back(gameobjects["barrelTR"]);

	//gameobjects["listener"] = std::make_shared<GameObject>(
	//	glm::vec3(23.0f, 45.0f, 0.0f), barrelMesh, defaultMaterial, barrelTexMap);

	//gameobjects["barrelBR"] = std::make_shared<GameObject>(
	//	glm::vec3(40.f, 42.0f, 25.f), barrelMesh, defaultMaterial, barrelTexMap);

	//gameobjects["barrelBL"] = std::make_shared<GameObject>(
	//	glm::vec3(-15.f, 42.0f, 30.f), barrelMesh, defaultMaterial, barrelTexMap);
	//
	//gameobjects["barrelTL"] = std::make_shared<GameObject>(
	//	glm::vec3(-15.f, 42.0f, -10.f), barrelMesh, defaultMaterial, barrelTexMap);

	gameobjects["barrel1"] = std::make_shared<GameObject>(
		glm::vec3(-2.0f, 42.0f, 0.0f), barrelMesh, defaultMaterial, barrelTexMap);
	gameobjects["barrel1"]->setScale(glm::vec3(1.2));
	obstacles.push_back(gameobjects["barrel1"]);

	gameobjects["boulder2"] = std::make_shared<GameObject>(
		glm::vec3(-5.f, 42.0f, 23.f), boulderMesh, defaultMaterial, boulderTexMap);
	gameobjects["boulder2"]->setScale(glm::vec3(1.3));
	obstacles.push_back(gameobjects["boulder2"]);

	gameobjects["boulder"] = std::make_shared<GameObject>(
		glm::vec3(21.0f, 42.0f, -18.0f), boulderMesh, defaultMaterial, boulderTexMap);
	gameobjects["boulder"]->setScale(glm::vec3(1.6));
	obstacles.push_back(gameobjects["boulder"]);

	gameobjects["cannon"] = std::make_shared<GameObject>(
		glm::vec3(23.f, 43.0f, 10.f), cannonMesh, defaultMaterial, cannonTexMap);
	gameobjects["cannon"]->setScale(glm::vec3(1.7f));
	//gameobjects["cannon"]->emissiveLight = 0.3f;
	obstacles.push_back(gameobjects["cannon"]);

	gameobjects["cannonbox"] = std::make_shared<GameObject>(
		glm::vec3(23.f, 43.0f, 10.f), nullptr, defaultMaterial, nullptr);
	gameobjects["cannonbox"]->setRotationAngleZ(68 * degToRad);
	gameobjects["cannonbox"]->setScale(glm::vec3(1.7f));
	obstacles.push_back(gameobjects["cannonbox"]);

	gameobjects["crate"] = std::make_shared<GameObject>(
		glm::vec3(45.0f, 42.0f, -8.0f), crateMesh, defaultMaterial, crateTexMap);
	gameobjects["crate"]->setScale(glm::vec3(1.6));
	obstacles.push_back(gameobjects["crate"]);

	gameobjects["crate2"] = std::make_shared<GameObject>(
		glm::vec3(15.0f, -20.0f, 2.0f), crateMesh, defaultMaterial, crateTexMap);
	gameobjects["crate2"]->setScale(glm::vec3(1.3));
	obstacles.push_back(gameobjects["crate2"]);

	gameobjects["boat"] = std::make_shared<GameObject>(
		glm::vec3(45.0f, 41.0f, 23.0f), boatMesh, defaultMaterial, boatTexMap);
	gameobjects["boat"]->setRotationAngleY(-45 * degToRad);
	obstacles.push_back(gameobjects["boat"]);

	//gameobjects["palmtree"] = std::make_shared<GameObject>(
	//	glm::vec3(0.0f, 45.0f, 0.0f), palmtreeMesh, defaultMaterial, palmtreeTexMap);
	// A Star
	/*
	{
		//////////////////////////////////A STAR BEGIN ///////////////////////////////////////////
		//gameobjects["EndNode"] = std::make_shared<GameObject>(
		//	aStar["EndNode"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		//gameobjects["EndNode"]->setScale(glm::vec3(0.6));

		//first row of nodes
		gameobjects["onethree"] = std::make_shared<GameObject>(
			aStar["onethree"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["onethree"]->setScale(glm::vec3(0.6));

		gameobjects["onefour"] = std::make_shared<GameObject>(
			aStar["onefour"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["onefour"]->setScale(glm::vec3(0.6));

		gameobjects["onefive"] = std::make_shared<GameObject>(
			aStar["onefive"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["onefive"]->setScale(glm::vec3(0.6));

		gameobjects["onesix"] = std::make_shared<GameObject>(
			aStar["onesix"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["onesix"]->setScale(glm::vec3(0.6));

		//second row of nodes
		gameobjects["twoone"] = std::make_shared<GameObject>(
			aStar["twoone"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["twoone"]->setScale(glm::vec3(0.6));

		gameobjects["twotwo"] = std::make_shared<GameObject>(
			aStar["twotwo"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["twotwo"]->setScale(glm::vec3(0.6));

		gameobjects["twothree"] = std::make_shared<GameObject>(
			aStar["twothree"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["twothree"]->setScale(glm::vec3(0.6));

		gameobjects["twosix"] = std::make_shared<GameObject>(
			aStar["twosix"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["twosix"]->setScale(glm::vec3(0.6));

		gameobjects["twoseven"] = std::make_shared<GameObject>(
			aStar["twoseven"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["twoseven"]->setScale(glm::vec3(0.6));

		gameobjects["twoeight"] = std::make_shared<GameObject>(
			aStar["twoeight"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["twoeight"]->setScale(glm::vec3(0.6));

		//third row
		gameobjects["threeone"] = std::make_shared<GameObject>(
			aStar["threeone"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["threeone"]->setScale(glm::vec3(0.6));

		gameobjects["threetwo"] = std::make_shared<GameObject>(
			aStar["threetwo"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["threetwo"]->setScale(glm::vec3(0.6));

		gameobjects["threethree"] = std::make_shared<GameObject>(
			aStar["threethree"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["threethree"]->setScale(glm::vec3(0.6));

		gameobjects["threefour"] = std::make_shared<GameObject>(
			aStar["threefour"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["threefour"]->setScale(glm::vec3(0.6));

		gameobjects["threefive"] = std::make_shared<GameObject>(
			aStar["threefive"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["threefive"]->setScale(glm::vec3(0.6));

		gameobjects["threesix"] = std::make_shared<GameObject>(
			aStar["threesix"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["threesix"]->setScale(glm::vec3(0.6));

		gameobjects["threeeight"] = std::make_shared<GameObject>(
			aStar["threeeight"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["threeeight"]->setScale(glm::vec3(0.6));

		//fourth row
		gameobjects["fourone"] = std::make_shared<GameObject>(
			aStar["fourone"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fourone"]->setScale(glm::vec3(0.6));

		gameobjects["fourthree"] = std::make_shared<GameObject>(
			aStar["fourthree"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fourthree"]->setScale(glm::vec3(0.6));

		gameobjects["fourfour"] = std::make_shared<GameObject>(
			aStar["fourfour"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fourfour"]->setScale(glm::vec3(0.6));

		gameobjects["fourfive"] = std::make_shared<GameObject>(
			aStar["fourfive"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fourfive"]->setScale(glm::vec3(0.6));

		gameobjects["foursix"] = std::make_shared<GameObject>(
			aStar["foursix"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["foursix"]->setScale(glm::vec3(0.6));

		gameobjects["fourseven"] = std::make_shared<GameObject>(
			aStar["fourseven"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fourseven"]->setScale(glm::vec3(0.6));

		gameobjects["foureight"] = std::make_shared<GameObject>(
			aStar["foureight"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["foureight"]->setScale(glm::vec3(0.6));

		//fifth row
		gameobjects["fiveone"] = std::make_shared<GameObject>(
			aStar["fiveone"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fiveone"]->setScale(glm::vec3(0.6));

		gameobjects["fivetwo"] = std::make_shared<GameObject>(
			aStar["fivetwo"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fivetwo"]->setScale(glm::vec3(0.6));

		gameobjects["fivethree"] = std::make_shared<GameObject>(
			aStar["fivethree"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fivethree"]->setScale(glm::vec3(0.6));

		gameobjects["fivefour"] = std::make_shared<GameObject>(
			aStar["fivefour"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fivefour"]->setScale(glm::vec3(0.6));

		gameobjects["fivesix"] = std::make_shared<GameObject>(
			aStar["fivesix"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fivesix"]->setScale(glm::vec3(0.6));

		gameobjects["fiveseven"] = std::make_shared<GameObject>(
			aStar["fiveseven"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fiveseven"]->setScale(glm::vec3(0.6));

		gameobjects["fiveeight"] = std::make_shared<GameObject>(
			aStar["fiveeight"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["fiveeight"]->setScale(glm::vec3(0.6));

		//sixth row
		gameobjects["sixone"] = std::make_shared<GameObject>(
			aStar["sixone"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sixone"]->setScale(glm::vec3(0.6));

		gameobjects["sixthree"] = std::make_shared<GameObject>(
			aStar["sixthree"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sixthree"]->setScale(glm::vec3(0.6));

		gameobjects["sixfour"] = std::make_shared<GameObject>(
			aStar["sixfour"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sixfour"]->setScale(glm::vec3(0.6));

		gameobjects["sixfive"] = std::make_shared<GameObject>(
			aStar["sixfive"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sixfive"]->setScale(glm::vec3(0.6));

		gameobjects["sixsix"] = std::make_shared<GameObject>(
			aStar["sixsix"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sixsix"]->setScale(glm::vec3(0.6));

		gameobjects["sixeight"] = std::make_shared<GameObject>(
			aStar["sixeight"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sixeight"]->setScale(glm::vec3(0.6));

		//seventh row
		gameobjects["sevenone"] = std::make_shared<GameObject>(
			aStar["sevenone"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sevenone"]->setScale(glm::vec3(0.6));

		gameobjects["seventwo"] = std::make_shared<GameObject>(
			aStar["seventwo"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["seventwo"]->setScale(glm::vec3(0.6));

		gameobjects["seventhree"] = std::make_shared<GameObject>(
			aStar["seventhree"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["seventhree"]->setScale(glm::vec3(0.6));

		gameobjects["sevenfour"] = std::make_shared<GameObject>(
			aStar["sevenfour"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sevenfour"]->setScale(glm::vec3(0.6));

		gameobjects["sevenfive"] = std::make_shared<GameObject>(
			aStar["sevenfive"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sevenfive"]->setScale(glm::vec3(0.6));

		gameobjects["sevensix"] = std::make_shared<GameObject>(
			aStar["sevensix"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sevensix"]->setScale(glm::vec3(0.6));

		gameobjects["sevenseven"] = std::make_shared<GameObject>(
			aStar["sevenseven"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["sevenseven"]->setScale(glm::vec3(0.6));

		gameobjects["seveneight"] = std::make_shared<GameObject>(
			aStar["seveneight"]->pos, barrelMesh, defaultMaterial, barrelTexMap);
		gameobjects["seveneight"]->setScale(glm::vec3(0.6));
	}
	*/

	///////////////////////////////////////////////////////////////////////////
	////////////////	Players

	players["bombot1"] = std::make_shared<Player>(
		glm::vec3(-8.0f, 39.5f, 9.0f), bombotMesh, defaultMaterial, bombot1TexMap, 0, &soundTemplates);
	

	players["bombot2"] = std::make_shared<Player>(
		glm::vec3(50.0f, 39.5f, 5.0f), bombotMesh2, defaultMaterial, bombot2TexMap, 1, &soundTemplates);
	
	players["bombot3"] = std::make_shared<Player>(
		glm::vec3(0.0f, 40.0f, 0.0f), bombotMesh3, defaultMaterial, bombot3TexMap, 2, &soundTemplates);


	players["bombot4"] = std::make_shared<Player>(
		glm::vec3(0.0f, 40.0f, 0.0f), bombotMesh4, defaultMaterial, bombot4TexMap, 3, &soundTemplates);


	// Report gameObject init times
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << " success, " << duration << "ms taken" << std::endl;
	

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
	std::string cannonBodyPath = "assets\\bullet\\cannon.btdata";
	std::string boatBodyPath = "assets\\bullet\\scaledboat.btdata";
	std::string sketchBodyPath = "assets\\bullet\\sketch.btdata";
	std::string trumpBodyPath = "assets\\bullet\\Trumps_wall.btdata";
	std::string booksBodyPath = "assets\\bullet\\books.btdata";
	std::string botwallBodyPath = "assets\\bullet\\botwall.btdata";
	std::string botleftBodyPath = "assets\\bullet\\botwall.btdata";
	std::string rightwallBodyPath = "assets\\bullet\\botwall.btdata";
	std::string topwallBodyPath = "assets\\bullet\\topwall.btdata";
	std::string lampBodyPath = "assets\\bullet\\lamp.btdata";
	std::string ringBodyPath = "assets\\bullet\\readyUp.btdata";

	// Create rigidbodies
	std::unique_ptr<RigidBody> tableBody;
	std::unique_ptr<RigidBody> bombot1Body;
	std::unique_ptr<RigidBody> bombot2Body;
	std::unique_ptr<RigidBody> bombot3Body;
	std::unique_ptr<RigidBody> bombot4Body;
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
	std::unique_ptr<RigidBody> rightwallBody;
	std::unique_ptr<RigidBody> botleftBody;
	std::unique_ptr<RigidBody> topwallBody;
	std::unique_ptr<RigidBody> lampBody;
	std::unique_ptr<RigidBody> ring1Body;
	std::unique_ptr<RigidBody> ring2Body;
	std::unique_ptr<RigidBody> ring3Body;
	std::unique_ptr<RigidBody> ring4Body;

	tableBody = std::make_unique<RigidBody>();
	bombot1Body = std::make_unique<RigidBody>(btBroadphaseProxy::CharacterFilter);
	bombot2Body = std::make_unique<RigidBody>(btBroadphaseProxy::CharacterFilter);
	bombot3Body = std::make_unique<RigidBody>(btBroadphaseProxy::CharacterFilter);
	bombot4Body = std::make_unique<RigidBody>(btBroadphaseProxy::CharacterFilter);
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
	botwallBody = std::make_unique<RigidBody>(btBroadphaseProxy::DebrisFilter, btBroadphaseProxy::CharacterFilter);
	rightwallBody	= std::make_unique<RigidBody>(btBroadphaseProxy::DebrisFilter, btBroadphaseProxy::CharacterFilter);
	botleftBody		= std::make_unique<RigidBody>(btBroadphaseProxy::DebrisFilter, btBroadphaseProxy::CharacterFilter);
	topwallBody = std::make_unique<RigidBody>();
	lampBody = std::make_unique<RigidBody>();
	ring1Body = std::make_unique<RigidBody>(btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::CharacterFilter);
	ring2Body = std::make_unique<RigidBody>(btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::CharacterFilter);
	ring3Body = std::make_unique<RigidBody>(btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::CharacterFilter);
	ring4Body = std::make_unique<RigidBody>(btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::CharacterFilter);


	// Test gameobject load times
	std::cout << "loading rigidBodies...";
	t1 = std::chrono::high_resolution_clock::now();

	// Load rigidbodies
	tableBody->load(tableBodyPath);
	bombot1Body->load(bombotBodyPath);
	bombot2Body->load(bombotBodyPath);
	bombot3Body->load(bombotBodyPath);
	bombot4Body->load(bombotBodyPath);
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
	rightwallBody->load(botwallBodyPath);
	botleftBody->load(botleftBodyPath);
	topwallBody->load(topwallBodyPath);
	lampBody->load(lampBodyPath);
	ring1Body->load(ringBodyPath, btCollisionObject::CF_NO_CONTACT_RESPONSE | btCollisionObject::CF_KINEMATIC_OBJECT);
	ring2Body->load(ringBodyPath, btCollisionObject::CF_NO_CONTACT_RESPONSE | btCollisionObject::CF_KINEMATIC_OBJECT);
	ring3Body->load(ringBodyPath, btCollisionObject::CF_NO_CONTACT_RESPONSE | btCollisionObject::CF_KINEMATIC_OBJECT);
	ring4Body->load(ringBodyPath, btCollisionObject::CF_NO_CONTACT_RESPONSE | btCollisionObject::CF_KINEMATIC_OBJECT);

	// Report rigidbody load times
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << " success, " << duration << "ms taken" << std::endl << std::endl;

	// Attach rigidbodies
	players["bombot1"]->attachRigidBody(bombot1Body);
	players["bombot2"]->attachRigidBody(bombot2Body);
	players["bombot3"]->attachRigidBody(bombot3Body);
	players["bombot4"]->attachRigidBody(bombot4Body);

	gameobjects["table"]->attachRigidBody(tableBody);
	gameobjects["sphere"]->attachRigidBody(sphereBody);
	gameobjects["barrelTR"]->attachRigidBody(barrelBody);
	gameobjects["barrel1"]->attachRigidBody(barrel1Body);
	gameobjects["boulder"]->attachRigidBody(boulderBody);
	gameobjects["boulder2"]->attachRigidBody(boulder2Body);
	gameobjects["crate"]->attachRigidBody(crateBody);
	gameobjects["crate2"]->attachRigidBody(crate2Body);
	gameobjects["cannonbox"]->attachRigidBody(cannonBody);
	gameobjects["boat"]->attachRigidBody(boatBody);
	gameobjects["sketch"]->attachRigidBody(sketchBody);
	gameobjects["trumps_wall"]->attachRigidBody(trumpBody);
	gameobjects["booksbody"]->attachRigidBody(booksBody);
	gameobjects["botwall"]->attachRigidBody(botwallBody);
	gameobjects["botleftwall"]->attachRigidBody(botleftBody);
	gameobjects["rightwall"]->attachRigidBody(rightwallBody);
	gameobjects["topwall"]->attachRigidBody(topwallBody);
	gameobjects["lamp"]->attachRigidBody(lampBody);
	gameobjects["readyBlue"]->attachRigidBody(ring1Body);
	gameobjects["readyRed"]->attachRigidBody(ring2Body);
	gameobjects["readyGreen"]->attachRigidBody(ring3Body);
	gameobjects["readyYellow"]->attachRigidBody(ring4Body);

	///////////////////////////////////////////////////////////////////////////
	////////////////////////	PROPERTIES		///////////////////////////////

	// Set up the bomb manager
	bombManager = std::make_shared<BombManager>();
	bombManager->init(bombMesh,
		bombTexMap1,		// Player1 bomb texture
		bombTexMap2,		// Player2 bomb texture
		bombTexMap3,		// Player3 bomb texture
		bombTexMap4,		// Player4 bomb texture
		sphereMesh,		// Explosion mesh
		explosionTexMap1,	// Explosion texture 1
		explosionTexMap2,	// Explosion texture 2
		explosionTexMap3,	// Explosion texture 3
		explosionTexMap4,	// Explosion texture 4
		sphereBodyPath,	// Explosion rigidbody
		defaultMaterial,
		bombBodyPath,
		&soundTemplates);

	players["bombot1"]->attachBombManager(bombManager);
	players["bombot2"]->attachBombManager(bombManager);
	players["bombot3"]->attachBombManager(bombManager);
	players["bombot4"]->attachBombManager(bombManager);

	// Set the outline colors
	players["bombot1"]->setOutlineColour(glm::vec4(0.39f, 0.72f, 1.0f, 1.0f));
	players["bombot2"]->setOutlineColour(glm::vec4(1.0f, 0.41f, 0.37f, 1.0f));
	players["bombot3"]->setOutlineColour(glm::vec4(0.31f, 0.93f, 0.32f, 1.0f));
	players["bombot4"]->setOutlineColour(glm::vec4(0.88f, 0.87f, 0.33f, 1.0f));

	players["bombot1"]->initParticleSmoke(materials["particles"], smokeTexMap);
	players["bombot2"]->initParticleSmoke(materials["particles"], smokeTexMap);
	players["bombot3"]->initParticleSmoke(materials["particles"], smokeTexMap);
	players["bombot4"]->initParticleSmoke(materials["particles"], smokeTexMap);

	players["bombot1"]->initParticleSpark(materials["particles"], sparkTexMap);
	players["bombot2"]->initParticleSpark(materials["particles"], sparkTexMap);
	players["bombot3"]->initParticleSpark(materials["particles"], sparkTexMap);
	players["bombot4"]->initParticleSpark(materials["particles"], sparkTexMap);

	bombManager->initParticles(materials["particles"], explosionTexMap);

	// Set up the bullet callbacks
	RigidBody::getDispatcher()->setNearCallback((btNearCallback)bulletNearCallback);

	// Set menu properties
	// find this 



	// Set default camera properties (WIP)



}

void initializeStates()
{
	// Test atlas load times
	std::cout << "Loading atlases..." << std::endl;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	//load textures
	char startTex[] = "Assets/img/menMain_atlas.png";
	std::shared_ptr<Texture> startTexMap = std::make_shared<Texture>(startTex, startTex, 1.0f);

	char pauseTex[] = "Assets/img/menPause_atlas(4000x4000).png";
	std::shared_ptr<Texture> pauseTexMap = std::make_shared<Texture>(pauseTex, pauseTex, 1.0f);

	std::string countdownTex = "Assets/img/321(scaled).png";
	std::shared_ptr<Texture> countdownTexMap = std::make_shared<Texture>(countdownTex, countdownTex, 1.0f);

	// Report atlas load times
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << "success, total " << duration << "ms taken" << std::endl << std::endl;

	//save them
	textures["start"] = startTexMap;
	textures["pause"] = pauseTexMap;

	//load up menu's
	startMenu = std::make_shared<Menu>(startTexMap, 4, 7);
	startMenu->setMaterial(materials["menu"]);


	pauseMenu = std::make_shared<Menu>(pauseTexMap, 4, 7);
	pauseMenu->setMaterial(materials["menu"]);

	std::shared_ptr<Menu> countdown = std::make_shared<Menu>(countdownTexMap, 2, 2);
	countdown->setMaterial(materials["menu"]);

	std::cout << "Loading states...";
	t1 = std::chrono::high_resolution_clock::now();

	//init states.
	mainMenu = new MainMenu(startMenu, players.at("bombot1")->getController(), &soundTemplates);
	mainMenu->setPaused(false);

	pause = new Pause(pauseMenu);
	pause->setPaused(true);


	game = new Game(&gameobjects, &players, &materials, &textures, &obstacles, &readyUpRings, bombManager, countdown, &soundTemplates, pause, score, &playerCamera);
	game->setPaused(true);

	states.addGameState("game", game);
	states.addGameState("MainMenu", mainMenu);
	states.addGameState("pause", pause);

	// Report state load times
	t2 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << " success, total " << duration << "ms taken" << std::endl;
}

void bulletNearCallback(btBroadphasePair& collisionPair,
	btCollisionDispatcher& dispatcher, btDispatcherInfo& dispatchInfo)
{
	// Do your collision logic here
	// Only dispatch the Bullet collision information if you want the physics to continue
	// From Bullet user manual

	/*if (collisionPair.m_pProxy0->m_collisionFilterGroup == btBroadphaseProxy::SensorTrigger &&
		collisionPair.m_pProxy1->m_collisionFilterGroup == btBroadphaseProxy::DebrisFilter ||
		collisionPair.m_pProxy0->m_collisionFilterGroup == btBroadphaseProxy::DebrisFilter &&
		collisionPair.m_pProxy1->m_collisionFilterGroup == btBroadphaseProxy::SensorTrigger)
	{
		return;
	}*/

	// Tell the dispatcher to do the collision information
	dispatcher.defaultNearCallback(collisionPair, dispatcher, dispatchInfo);
}

void collideWithCorrectType(Player* player, GameObject* object)
{
	switch (object->getColliderType())
	{
	case GameObject::COLLIDER_DEFAULT:
		break;
	case GameObject::PLAYER:
		player->checkCollisionWith((Player*)object);
		break;
	case GameObject::BOMB_BASE:
		player->checkCollisionWith((Bomb*)object, game->getCurrentState() == Game::GAME_STATE::READYUP);
		break;
	case GameObject::BOMB_EXPLOSION:
		player->checkCollisionWith((Explosion*)object, game->getCurrentState() == Game::GAME_STATE::READYUP);
		break;
	case GameObject::READYUP:
		player->checkCollisionWith((readyUpRing*)object);
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	////Shadow Map
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
	switch (key) 
	{
	case 'g':
		//aStar.findPath(players["bombot1"]);
		//aStar.newTraverse = true;
		//aStar.traverse = true;
		
		//players["bombot1"]->setPosition(players["bombot1"]->checkNodes(aStarPointer)->pos);
		break;
	}
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
	//if (aStar.traverse)
		//aStar.traversePath(players["bombot1"], deltaTime);

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
	states.getGameState("game")->windowReshapeCallbackFunction(w, h);
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
	for (auto it : soundTemplates)
	{
		it.second.release();
	}
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
	//glutFullScreen();

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
	//ilutInit();
	//ilutRenderer(ILUT_OPENGL);

	// Init GL
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Load time monitoring
	std::chrono::high_resolution_clock::time_point totalStart, totalFinish;
	totalStart = std::chrono::high_resolution_clock::now();

	// Initialize scene
	initializeShaders();
	initializeScene();

	initializeStates();

	totalFinish = std::chrono::high_resolution_clock::now();
	auto durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(totalFinish - totalStart).count();
	auto durationS = std::chrono::duration_cast<std::chrono::seconds>(totalFinish - totalStart).count();
	std::cout << "Total time to initialize: " << durationMS << " ms or " << durationS << " seconds" << std::endl;

	srand(time(0));
	/* Start Game Loop */
	deltaTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime /= 1000.0f;


	glutMainLoop();
	return 0;
}
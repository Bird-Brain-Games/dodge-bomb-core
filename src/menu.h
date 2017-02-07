#pragma once
#include "GL\glew.h"
#include "Shader.h"
#include "loadObject.h"
class Menu
{
public:
	// the texture we are displaying
	Menu(Texture*);
	void updateTex(Texture*);
	Texture* tex;
	void draw();
private:
	// our shader. we use this to draw our texture to the screen.
	Shader menu;
	GLuint vaoD;
	GLuint vboD;

	
};
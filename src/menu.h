#pragma once
#include "GL\glew.h"
#include "Shader.h"
#include "loadObject.h"
class Menu
{
public:
	// the texture we are displaying
	Menu(Texture*);
	Menu(Texture* _tex, int _width, int _height);
	void updateTex(Texture*);
	//sets the spot to the given value.
	void setSpot(glm::vec2);
	void setSpot(int, int);
	//Incriments the row spot and if it goes past the row size it resets row to zero and increments coloumn
	void incSpot();

	void draw();

private:
	//How many textures are in the width and height of the texture atlas
	glm::vec2 dim;

	//where we are on the texture in x and y
	glm::vec2 spot;

	// our shader. we use this to draw our texture to the screen.
	Shader menu;
	GLuint vaoD;
	GLuint vboD;
	Texture* tex;

	
};
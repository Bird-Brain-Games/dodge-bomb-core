#pragma once
#include "GL\glew.h"
#include "Texture.h"
#include "material.h"
#include <memory>

class Menu
{
public:
	// the texture we are displaying
	Menu(std::shared_ptr<Texture>);
	Menu(std::shared_ptr<Texture> _tex, int _width, int _height);
	void updateTex(std::shared_ptr<Texture>);
	//sets the spot to the given value.
	void setSpot(glm::vec2);
	//Incriments the row spot and if it goes past the row size it resets row to zero and increments coloumn
	void incSpot();

	void setMaterial(std::shared_ptr<Material> newMaterial) { menuMaterial = newMaterial; }

	void draw();

private:
	//How many textures are in the width of the texture atlas
	glm::vec2 dim;

	//where we are on the texture
	glm::vec2 spot;

	// our shader. we use this to draw our texture to the screen.
	std::shared_ptr<Material> menuMaterial;
	GLuint vaoD;
	GLuint vboD;
	std::shared_ptr<Texture> tex;
};
#include "menu.h"

Menu::Menu(Texture* _tex)
{
	tex = _tex;
	menu.load("shaders//null.vert", "shaders//shader_texture.frag", "shaders//menu.geom");
	glGenVertexArrays(1, &vaoD);
	glBindVertexArray(vaoD);
	glGenBuffers(1, &vboD);
}

void Menu::draw()
{
	glBindVertexArray(vaoD);
	menu.bind();
	tex->bind(&menu);
	glDrawArrays(GL_POINTS, 0, 1);
	menu.unbind();
	glBindVertexArray(0);
}

void Menu::updateTex(Texture* _tex)
{
	tex = _tex;
}
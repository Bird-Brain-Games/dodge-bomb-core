#include "menu.h"

Menu::Menu(Texture* _tex)
{
	tex = _tex;
	menu.load("shaders//null.vert", "shaders//shader_texture.frag", "shaders//menu.geom");
	glGenVertexArrays(1, &vaoD);
	glBindVertexArray(vaoD);
	glGenBuffers(1, &vboD);
	dim = glm::vec2(1);
	spot = glm::vec2(0);
}

Menu::Menu(Texture* _tex, int _width, int _height)
{
	tex = _tex;
	menu.load("shaders//null.vert", "shaders//shader_texture.frag", "shaders//menu.geom");
	glGenVertexArrays(1, &vaoD);
	glBindVertexArray(vaoD);
	glGenBuffers(1, &vboD);
	dim = glm::vec2(_width, _height);
	spot = glm::vec2(0);
}

void Menu::setSpot(glm::vec2 _spot)
{
	spot = _spot;
	if (spot.x >= dim.x)
		spot.x = dim.x - 1;
	if (spot.y >= dim.y)
		spot.y = dim.y - 1;
}

void Menu::incSpot()
{
	spot.x++;
	if (spot.x >= dim.x)
	{
		spot.x = 0;
		spot.y++;
		if (spot.y >= dim.y)
		{
			spot.y = 0;
		}
	}

}

void Menu::draw()
{
	glBindVertexArray(vaoD);
	menu.bind();
	tex->bind(&menu);
	menu.uniformVec2("_dim", dim);
	menu.uniformVec2("_spot", spot);
	glDrawArrays(GL_POINTS, 0, 1);
	menu.unbind();
	glBindVertexArray(0);
}

void Menu::updateTex(Texture* _tex)
{
	tex = _tex;
}
#include "menu.h"

Menu::Menu(std::shared_ptr<Texture> _tex)
{
	tex = _tex;
	menuMaterial = nullptr;
	glGenVertexArrays(1, &vaoD);
	glBindVertexArray(vaoD);
	glGenBuffers(1, &vboD);
	dim = glm::vec2(1);
	spot = glm::vec2(0);
}

Menu::Menu(std::shared_ptr<Texture> _tex, int _width, int _height)
{
	tex = _tex;
	menuMaterial = nullptr;
	glGenVertexArrays(1, &vaoD);
	glBindVertexArray(vaoD);
	glGenBuffers(1, &vboD);
	dim = glm::vec2(_width, _height);
	spot = glm::vec2(0, 1);
}

void Menu::setSpot(glm::vec2 _spot)
{
	spot = _spot;
	if (spot.x >= dim.x)
		spot.x = dim.x - 1;
	if (spot.y >= dim.y)
		spot.y = dim.y - 1;
}

void Menu::setSpot(int x, int y)
{
	spot = glm::vec2(x, y);
	if (spot.x >= dim.x)
		spot.x = dim.x - 1;
	if (spot.y >= dim.y)
		spot.y = dim.y - 1;
}

void Menu::incSpotR()
{
	spot.x++;
	if (spot.x >= dim.x)
	{
		spot.x = 0;
	}
}

void Menu::incSpotM()
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
	menuMaterial->shader->bind();
	tex->bind(GL_TEXTURE31, GL_TEXTURE30);

	//menuMaterial->
	menuMaterial->shader->sendUniformInt("diffuseTex", 31);
	menuMaterial->shader->sendUniformVec2("_dim", dim);
	menuMaterial->shader->sendUniformVec2("_spot", spot);
	glDrawArrays(GL_POINTS, 0, 1);
	//menu.unbind();
	glBindVertexArray(0);
	menuMaterial->shader->unbind();
	tex->unbind(GL_TEXTURE31, GL_TEXTURE30);
}

void Menu::updateTex(std::shared_ptr<Texture> _tex)
{
	tex = _tex;
}

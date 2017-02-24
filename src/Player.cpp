#include "Player.h"

Player::Player(glm::vec3 position,
	std::shared_ptr<Loader> _mesh,
	std::shared_ptr<Material> _material,
	std::shared_ptr<Texture> _texture,
	int playerNum)
	:
	GameObject(position, _mesh, _material, _texture), 
	con(playerNum)
{
	//bomb = _bomb;
	//bomb->setTransform(glm::vec3(0.0, -15.0, 0.0));

	//timer = 0;
	//duration = 3;
	//thrown = false;
}

Player::~Player()
{

}

void Player::draw(Camera _camera)
{
	//bomb->draw(_camera);
	GameObject::draw(_camera);
	//this->getRigidBody()->getBody()->setAngularFactor(btVector3(0, 1, 0));
}

void Player::update(float _dt)
{

	/*if (thrown == true)
	{
		timer += _dt;
		if (timer > duration)
		{
			timer = 0;
			thrown = false;
			bomb->setTransform(glm::vec3(0.0f, -50.0f, 0.0f));
		}
	}

	controls();*/
	GameObject::update(_dt);
}

const float degToRad = 3.14159f / 180.0f;
void Player::controls()
{
	//GameObject* player = this;
	//RigidBody * rigid = player->getRigidBody();

	//Coords stick = con.getLeftStick();
	//glm::vec3 pos = glm::vec3(0.0f);
	////std::cout << stick.x << " y: " << stick.y << std::endl;
	//bool motion = false;
	//if (stick.x < -0.1 || stick.x > 0.1)
	//{
	//	pos.x = stick.x / 2;
	//	motion = true;
	//}
	//if (stick.y < -0.1 || stick.y > 0.1)
	//{
	//	pos.z = -stick.y / 2;
	//	motion = true;
	//}
	//stick = con.getRightStick();
	//angle = atan2(-stick.y, stick.x) + 270 * degToRad;


	//glm::vec3 temp = player->getRigidBody()->getWorldTransform()[3];

	//player->setTransform(temp, glm::vec4(0.0f, angle, 0.0f, 1.f));

	//if (motion == true)
	//{

	//	glm::mat4 transform = rigid->getWorldTransform();
	//	glm::mat4 translate = glm::translate(pos);
	//	transform += translate;
	//	player->setTransform(transform);
	//	//player->getRigidBody()->getBody()->applyCentralImpulse(btVector3(-stick.y, 0, -stick.x));

	//}

	//if (con.conButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && thrown == false)
	//{
	//	bomb->getRigidBody()->getBody()->setLinearVelocity(btVector3(0, 0, 0));
	//	bomb->getRigidBody()->getBody()->clearForces();//clears force not impulse?

	//	glm::vec3 temp = player->getRigidBody()->getWorldTransform()[3];

	//	glm::vec2 normalized = glm::vec2(0);

	//	if (stick.y > 0.1 || stick.y < -0.1 || stick.x > 0.1 || stick.x < -0.1)
	//		normalized = glm::normalize(glm::vec2(stick.x, stick.y));


	//	bomb->setTransform(temp, glm::vec4(0.0f, 0.0, 0.0f, 1.f));

	//	std::cout << "x: " << normalized.x << "y: " << normalized.y << std::endl;

	//	bomb->getRigidBody()->getBody()->applyCentralImpulse(btVector3(normalized.x * 150, 75.0f, normalized.y * 150));

	//	thrown = true;

	//}

}

void Player::checkCollisionWith(GameObject* other)
{
	//std::cout << "bomb collided with player" << std::endl;
}
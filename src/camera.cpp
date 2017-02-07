/*Campbell Hamilton 100582048
january 2017
*/

#include "camera.h"
#include <iostream>
glm::mat4x4 Camera::getView()
{
	return viewMatrix;
}
glm::mat4x4 Camera::getProj()
{
	return projectionMatrix;
}

//sets camera at origin looking into z axis.
Camera::Camera()
{


	pos = glm::vec3(0.35f, 2.7f, 16.6f);

	angle = glm::vec2(3.14159f, 6.0f);

	direction = glm::vec3(
		cos(angle.y) * sin(angle.x),
		sin(angle.y),
		cos(angle.y) * cos(angle.x)
	);
	//1.57f is pi/2
	right = glm::vec3(
		sin(angle.x - 1.57f),
		0,
		cos(angle.x - 1.57f)
	);
	up = glm::cross(right, direction);

	//1080 by 720
	windowRatio = 1.5;

	FOV = 45;
	minRange = 0.1f;
	maxRange = 10000;
	speed = 0.001f;


	update();
}

void Camera::setAngle(float x, float y)
{
	angle = glm::vec2(x, y);
}

void Camera::setProperties(float _FOV, float _windowRatio, float _minRange, float _maxRange, float _speed)
{
	//projection variables
	FOV = _FOV;
	windowRatio = _windowRatio;
	minRange = _minRange;
	maxRange = _maxRange;
	speed = _speed;


	//camera variables
	angle = glm::vec2(3.14159f, 6.28318f);

	direction = glm::vec3(
		cos(angle.y) * sin(angle.x),
		sin(angle.y),
		cos(angle.y) * cos(angle.x)
	);
	//1.57f is pi/2
	right = glm::vec3(
		sin(angle.x - 1.57f),
		0,
		cos(angle.x - 1.57f)
	);
	up = glm::cross(right, direction);
}

void Camera::setPosition(glm::vec3 _pos)
{
	pos = _pos;
}

void Camera::update()
{
	viewMatrix = glm::lookAt(pos, pos + direction, up);
	projectionMatrix = glm::perspective(FOV, windowRatio, minRange, maxRange);
}

void Camera::mouseMotion(int x, int y, int preX, int preY)
{
	float xDif = (x - preX);
	float yDif = (y - preY);

	if (abs(xDif) >= 100.0f || abs(yDif) >= 100.0f)
		return;

	angle.y += yDif * speed;
	angle.x += xDif * speed;

	direction = glm::vec3(
		cos(angle.y) * sin(angle.x),
		sin(angle.y),
		cos(angle.y) * cos(angle.x)
	);
	//1.57f is pi/2
	right = glm::vec3(
		sin(angle.x - 1.57f),
		0,
		cos(angle.x - 1.57f)
	);
	up = glm::cross(right, direction);
}

void Camera::moveUp()
{
	pos += up *  (speed * 100);
}
void Camera::moveDown()
{
	pos += up *  (speed * 100);
}
void Camera::moveForward()
{
	pos += direction *  (speed * 100);
}
void Camera::moveBackward()
{
	pos -= direction *  (speed * 100);
}
void Camera::moveLeft()
{
	pos += right * (speed * 100);
}
void Camera::moveRight()
{
	pos -= right *  (speed * 100);
}
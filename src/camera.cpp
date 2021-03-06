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
	setPosition(glm::vec3(15.0f));
	setAngle(0.0f, 0.0f);

	setProperties(45, 1920.0f / 1080.0f, 0.1f, 100000.0f, 0.5f);
	forward = glm::vec3(0.0f, -10.0f, -10.0f);

	shakeRadius = 0.0f;
	shakeDegrade = 0.05f;
	update();
}

void Camera::setAngle(float x, float y)
{
	angle = glm::vec2(x, y);

	forward = glm::vec3(
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
	up = glm::cross(right, forward);
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

	forward = glm::vec3(
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
	up = glm::cross(right, forward);
}

void Camera::setPosition(glm::vec3 _pos)
{
	pos = _pos;
}

void Camera::setForward(glm::vec3 _for)
{
	forward = _for;
}

void Camera::update()
{
	if (shakeRadius > 0.0f)
	{
		distribution = std::uniform_real_distribution<float>(-shakeRadius, shakeRadius);
		float offsetX = distribution(generator);
		float offsetY = distribution(generator);
		shakeOffset = glm::vec3(offsetX, offsetY, 0.0f);

		shakeRadius -= shakeDegrade;
		if (shakeRadius < 0.0f)
			shakeRadius = 0.0f;
	}
	else
		shakeOffset = glm::vec3(0.0f);

	viewMatrix = glm::lookAt(pos, pos + forward, up) * glm::translate(shakeOffset);
	projectionMatrix = glm::perspective(FOV, windowRatio, minRange, maxRange);
	viewProjMatrix = projectionMatrix * viewMatrix;
}

void Camera::shadowCam(glm::vec3 _pos, glm::vec3 _forward, glm::vec3 _up, float _minRange, float _maxRange)
{
	viewMatrix = glm::lookAt(_pos, _pos + _forward, _up);
	projectionMatrix = glm::ortho(-70.0f, 60.0f, -100.0f, 80.0f, _minRange, _maxRange);

	viewProjMatrix = projectionMatrix * viewMatrix;
}

void Camera::mouseMotion(int x, int y, int preX, int preY)
{
	float xDif = (x - preX);
	float yDif = (y - preY);

	if (abs(xDif) >= 100.0f || abs(yDif) >= 100.0f)
		return;

	angle.y += yDif * speed;
	angle.x += xDif * speed;

	forward = glm::vec3(
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
	up = glm::cross(right, forward);
}

void Camera::moveUp()
{
	pos += up *  (speed);
}
void Camera::moveDown()
{
	pos += up *  (speed);
}
void Camera::moveForward()
{
	pos += forward *  (speed);
}
void Camera::moveBackward()
{
	pos -= forward *  (speed);
}
void Camera::moveLeft()
{
	pos += right * (speed);
}
void Camera::moveRight()
{
	pos -= right *  (speed);
}

void Camera::shakeScreen(float radius)
{
	shakeRadius = radius;
}
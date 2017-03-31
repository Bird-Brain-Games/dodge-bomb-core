/*Campbell Hamilton 100582048
january 2017
*/

#pragma once
#include <glm/vec3.hpp>
#include <GLEW/glew.h>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <random>

// our camera
class Camera
{
public:
	Camera();
	void setProperties(float _FOV, float _ratio, float _minRange, float _maxRange, float _speed);
	// Set the camera window ratio
	void setRatio(float _ratio) { windowRatio = _ratio; }
	void setRatio(float windowHeight, float windowWidth) { windowRatio = windowWidth / windowHeight; }

	//sets the camera's position
	void setPosition(glm::vec3 _pos);
	void setForward(glm::vec3 _for);
	void setAngle(float, float);
	glm::vec3 getForward() { return forward; }

	//returns view matrix
	glm::mat4x4 getView();

	//returns projection matrix
	glm::mat4x4 getProj();

	// returns view projection matrix
	glm::mat4x4 getViewProj() { return viewProjMatrix; }

	//recalculates the view and projection matrix
	void update();
	void shadowCam(glm::vec3 _pos, glm::vec3 _forward, glm::vec3 _up, float _minRange, float _maxRange);
	//rotates camera based on values given.
	void mouseMotion(int x, int y, int preY, int preX);

	//moves the camera
	void moveUp();
	void moveDown();
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();

	void shakeScreen(float radius);

private:

	//the camera's matricies 
	glm::mat4x4 projectionMatrix;
	glm::mat4x4 viewMatrix;
	glm::mat4x4 viewProjMatrix;

	//transformation data
	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 forward;
	glm::vec3 right;

	// yaw and pitch
	glm::vec2 angle; 

	//The camera's variables. used to setup the projection matricies 
	float windowRatio;
	float FOV;
	float minRange;
	float maxRange;
	float speed;

	// Variables used for screen shake
	float shakeRadius;
	float shakeDegrade;
	glm::vec3 shakeOffset;
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
};
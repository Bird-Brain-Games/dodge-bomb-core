#include "DebugState.h"
#include "InputManager.h"
#include "RigidBody.h"

Debug::Debug(Camera* _camera)
{
	camera = _camera;
}

void Debug::update(float dt)
{
	if (KEYBOARD_INPUT->CheckPressEvent('e') || KEYBOARD_INPUT->CheckPressEvent('E'))
	{
		RigidBody::setDebugDraw(!RigidBody::isDrawingDebug());
	}
}

void Debug::draw()
{
	if (RigidBody::isDrawingDebug())
		RigidBody::drawDebug(camera->getView(), camera->getProj());
}
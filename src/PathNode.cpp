#include "PathNode.h"

PathNode::PathNode(int _distanceToTarget)
{
	dummy = true;
	distanceToTarget = _distanceToTarget;
}

PathNode::PathNode(glm::vec3 _pos, int _distanceToTarget, bool _dummy)
{
	pos = _pos;
	dummy = _dummy;
	distanceToTarget = _distanceToTarget;
}

PathNode::PathNode(glm::vec3 _pos, int _distanceToTarget)
{
	pos = _pos;
	dummy = false;
	distanceToTarget = _distanceToTarget;
}

PathNode::~PathNode()
{
}

void PathNode::addConnection(PathNode * connectionUp, PathNode * connectionRight, PathNode * connectionDown, PathNode * connectionLeft)
{
	connections["connectionUp"] = connectionUp;
	connections["connectionRight"] = connectionRight;
	connections["connectionDown"] = connectionDown;
	connections["connectionLeft"] = connectionLeft;

}

// Various types of classes and structures needed for representing joints for kinematic hierarchies
#pragma once

#include <GLM/glm.hpp>

enum RotationOrder
{
	// The two we care about
	XYZ = 0,
	ZYX,


	XZY,
	YXZ,
	YZX

	// ... 
};

struct JointDescriptor
{
	JointDescriptor()
	{
		boneLength = 0;
		numFrames = 0;
	}

	std::string jointName;			// Name of joint
	std::string jointParent;		// Name of joint's parent
	glm::vec3	jointBasePosition;	// Position of joint at frame 0
	glm::quat jointBaseRotation;	// Rotation of joint at frame 0
	float	boneLength;

	std::vector<glm::vec3>	jointPositions;		// Joint point for each frame (should all be zero except for root)
	std::vector<glm::quat> jointRotations;	// Joint rotations for each frame 
	std::vector<float> jointScales;

	unsigned int numFrames;
};

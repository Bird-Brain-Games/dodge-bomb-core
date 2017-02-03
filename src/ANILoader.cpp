#define _CRT_SECURE_NO_WARNINGS
#include "ANILoader.h"
#include <iostream>


#define MAX_LINE_LENGTH 128

ANILoader::ANILoader()
{
	rootNode = nullptr;
}

bool ANILoader::loadHTR(std::string fileName)
{
	FILE* file;

	file = fopen(fileName.c_str(), "r");

	char* loc;
	char buffer[MAX_LINE_LENGTH];
	int bufferSize = 0;

	if (file)
	{
		// Find line with header
		// fgets() will read to new line OR MAX_LINE_LENGTH, which ever happens first
		// Returns pointer to buffer OR null pointer for end of file
		while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL)
		{
			// Get location of "Header" in file
			loc = strstr(buffer, "Header");

			// Process headers
			if (loc)
			{
				// CODE HERE A //////////////////////////////////////////////////////////////////////////
				if (!processHeaderSection(file, loc))
					break;

				if (!processSegementNameSection(file, loc))
					break;

				if (!processBasePositionSection(file, loc))
					break;

				if (!processAnimationSection(file, loc))
					break;

				if (!processVBO(file, loc))
					break;
			}
		}

		fclose(file);	// Remember to close file

		return true;
	}
	else
	{
		std::cout << "LOAD HTR ERROR: " << fileName << " not found or cannot be opened." << std::endl;
		return false;
	}
}

bool ANILoader::processHeaderSection(FILE* fp, char* loc)
{
	// CODE HERE B //////////////////////////////////////////////////////////////////////////
	char buffer[MAX_LINE_LENGTH];

	goToNextValidLineInFile(fp, buffer);

	do
	{
		char idBuffer[64];
		char valueBuffer[64];

		sscanf(buffer, "%s %s", idBuffer, valueBuffer);

		processHeader(std::string(idBuffer), std::string(valueBuffer));

		goToNextValidLineInFile(fp, buffer);
		loc = strstr(buffer, "SegmentNames");
	} while (loc == nullptr);

	return true;
}

bool ANILoader::processHeader(std::string identifier, std::string value)
{
	if (identifier == "FileType")
	{
		fileType = value;
		return true;
	}
	else if (identifier == "DataType")
	{
		dataType = value;
		return true;
	}
	else if (identifier == "FileVersion")
	{
		// remember fileVersion is an int
		// atoi converts a string to an int
		fileVersion = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "NumSegments")
	{
		numSegments = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "NumFrames")
	{
		numFrames = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "DataFrameRate")
	{
		fps = std::atoi(value.c_str());
		return true;
	}
	else if (identifier == "EulerRotationOrder")
	{
		if (value == "XYZ")
			rotationOrder = RotationOrder::XYZ;
		else if (value == "ZYX")
			rotationOrder = RotationOrder::ZYX;
		else
		{
			std::cout << "HTR Parse Warning: File has unsupported rotation order of: " << value << std::endl;
		}

		// put conditions for the other rotation orders here...
	}
	else if (identifier == "CalibrationUnits")
	{

	}
	else if (identifier == "RotationUnits")
	{
		rotationUnits = value;
	}
	else if (identifier == "GlobalAxisofGravity")
	{

	}
	else if (identifier == "BoneLengthAxis")
	{

	}
	else if (identifier == "ScaleFactor")
	{

	}
	else
	{
		std::cout << "HTR Parse Error: Could not identify HEADER: " << identifier << " with value: " << value << std::endl;
		return false;
	}
	return false;
}

bool ANILoader::processSegementNameSection(FILE * fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process segment names
	// File currently on line with "SegmentNames"
	goToNextValidLineInFile(fp, buffer);
	do
	{
		// Allocate memory for values
		char childBuff[64];
		char parentBuff[64];

		// Parse values from current line 
		sscanf(buffer, "%s %s", childBuff, parentBuff);

		// Create new joint descriptor
		JointDescriptor jd;

		// Set joint descriptor values
		jd.jointName = childBuff;
		jd.jointParent = parentBuff;

		// Store the new joint
		jointDescriptors.push_back(jd);

		// Increment to next valid line
		goToNextValidLineInFile(fp, buffer);

		// Check if current line is the start of the next section
		// Remember strstr returns null if second param isn't found in first
		loc = strstr(buffer, "BasePosition");

	} while (loc == NULL);

	return true;
}

bool ANILoader::processBasePositionSection(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process base positions
	// File currently on line with "BasePosition"
	// increment to next line with actual data
	goToNextValidLineInFile(fp, buffer);
	do
	{
		char jointNameBuff[64];
		float tx, ty, tz;
		float rx, ry, rz;
		float boneLength;

		sscanf(buffer, "%s %f %f %f %f %f %f %f", jointNameBuff, &tx, &ty, &tz, &rx, &ry, &rz, &boneLength);

		// Find the joint descriptor by name
		JointDescriptor* jd = getJointDescriptorByName(jointNameBuff);

		// Make sure we got it
		if (!jd)
		{
			std::cout << "HTR Parse ERROR: Could not find JointDescriptor with name: " << std::string(jointNameBuff) << std::endl;
			return false;
		}

		// Store values
		jd->jointBasePosition = glm::vec3(tx, ty, tz);
		jd->jointBaseRotation = createQuaternion(rx, ry, rz);
		jd->boneLength = boneLength;
		// Increment to next valid line
		goToNextValidLineInFile(fp, buffer);

		// Check if current line is the start of the next section
		// Remember strstr returns null if second param isn't found in first
		loc = strstr(buffer, jointDescriptors[0].jointName.c_str());
	} while (loc == NULL);
	return true;
}

bool ANILoader::processAnimationSection(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process each joint's poses at each frame
	for (int i = 0; i < jointDescriptors.size(); i++)
	{
		goToNextValidLineInFile(fp, buffer);

		do
		{
			numFrames++;
			int frame;
			float tx, ty, tz;
			float rx, ry, rz;
			float scale;

			sscanf(buffer, "%d %f %f %f %f %f %f %f", &frame, &tx, &ty, &tz, &rx, &ry, &rz, &scale);

			// CODE HERE C //////////////////////////////////////////////////////////////////////////

			// Increment to next valid line
			goToNextValidLineInFile(fp, buffer);

			tx /= 10;
			ty /= 10;
			tz /= 10;

			glm::vec3 newJointPos(tx, ty, tz);
			glm::quat newJointRot = createQuaternion(rx, ry, rz);

			jointDescriptors[i].jointPositions.push_back(newJointPos);
			jointDescriptors[i].jointRotations.push_back(newJointRot);
			jointDescriptors[i].jointScales.push_back(scale);
			jointDescriptors[i].numFrames++;

			// Check if current line is the start of the next section
			// Remember strstr returns null if second param isn't found in first
			int nextJointIndex = i + 1;

			if (nextJointIndex < jointDescriptors.size())
				loc = strstr(buffer, jointDescriptors[nextJointIndex].jointName.c_str());
			else
			{
				loc = strstr(buffer, "EndOfFile");
			}
		} while (loc == NULL);
	}

	return true;
}

bool ANILoader::processVBO(FILE* fp, char* loc)
{
	char buffer[MAX_LINE_LENGTH];

	// Process each joint's poses at each frame
	char type;
	goToNextValidLineInFile(fp, buffer);
	int size;
	sscanf(buffer, "%i", &size);
	float x, y, z, w;
	char start = fgetc(fp);
	while (start != EOF)
	{
		goToNextValidLineInFile(fp, buffer);
		if (start == 'v')
		{
			sscanf(buffer, "%f %f %f", &x, &y, &z);
			vertexs.push_back(glm::vec3(x, y, z));
		}
		else if (start == 'u')
		{
			sscanf(buffer, "%f %f", &x, &y);
			UV.push_back(glm::vec2(x, y));
		}
		else if (start == 'n')
		{
			sscanf(buffer, "%f %f %f", &x, &y, &z);
			normals.push_back(glm::vec3(x, y, z));
		}
		else if (start == 'w')
		{
			sscanf(buffer, "%f %f %f %f", &x, &y, &z, &w);
			weights.push_back(glm::vec4(x, y, z, w));
		}
		else if (start == 'j')
		{
			sscanf(buffer, "%f %f %f %f", &x, &y, &z, &w);
			joints.push_back(glm::vec4(x, y, z, w));
		}
		else if (start == 'f')
		{
			//std::cout << strcmp(line, "f") << std::endl;
			unsigned int vertexIndex[3], UVindex[3], normalIndex[3], jointsIndex[3], weightsIndex[3];

			sscanf(buffer, "%d/%d/%d/%d/%d %d/%d/%d/%d/%d %d/%d/%d/%d/%d\n",
				&vertexIndex[0], &normalIndex[0], &UVindex[0], &weightsIndex[0], &jointsIndex[0],
				&vertexIndex[1], &normalIndex[1], &UVindex[1], &weightsIndex[1], &jointsIndex[1],
				&vertexIndex[2], &normalIndex[2], &UVindex[2], &weightsIndex[2], &jointsIndex[2]);

			vertexsOrder.push_back(vertexs[vertexIndex[0]]);
			vertexsOrder.push_back(vertexs[vertexIndex[1]]);
			vertexsOrder.push_back(vertexs[vertexIndex[2]]);

			UVOrder.push_back(UV[UVindex[0]]);
			UVOrder.push_back(UV[UVindex[1]]);
			UVOrder.push_back(UV[UVindex[2]]);

			normalsOrder.push_back(normals[normalIndex[0]]);
			normalsOrder.push_back(normals[normalIndex[1]]);
			normalsOrder.push_back(normals[normalIndex[2]]);

			weightsOrder.push_back(weights[weightsIndex[0]]);
			weightsOrder.push_back(weights[weightsIndex[1]]);
			weightsOrder.push_back(weights[weightsIndex[2]]);

			jointsOrder.push_back(joints[jointsIndex[0]]);
			jointsOrder.push_back(joints[jointsIndex[1]]);
			jointsOrder.push_back(joints[jointsIndex[2]]);
		}
		start = fgetc(fp);

	}

	return true;

}

void ANILoader::createNodes()
{
	// CODE HERE D //////////////////////////////////////////////////////////////////////////
	// HTR data loaded into memory
	// want game objects
	// want to update and draw.

	//poop

	for (int i = 0; i < jointDescriptors.size(); ++i)
	{
		Node newJoint;
		newJoint.name = jointDescriptors[i].jointName;
		newJoint.jointAnimation = &jointDescriptors[i];

		jointNodes.push_back(newJoint);
	}

	// Set up the heirarchy
	for (int i = 0; i < jointDescriptors.size(); ++i)
	{
		JointDescriptor *jd = &jointDescriptors[i];
		Node *child = getNodeByName(jd->jointName);
		Node *parent = getNodeByName(jd->jointParent);

		if (child == nullptr || parent == nullptr)
			continue;

		parent->addChild(child);
	}
}

Node* ANILoader::getRootNode()
{
	if (rootNode == nullptr)
	{
		for (int i = 0; i < jointNodes.size(); i++)
		{
			if (jointNodes[i].jointAnimation->jointParent == "GLOBAL")
				rootNode = &jointNodes[i];
		}
	}

	return rootNode;
}


void ANILoader::goToNextValidLineInFile(FILE* fp, char* buffer)
{
	fgets(buffer, MAX_LINE_LENGTH, fp);

	// Move stream to next valid line
	// Remember: fgets will split on tabs
	// Remember: '#' is comment symbol
	while (buffer[0] == '\t' || buffer[0] == '#')
		fgets(buffer, MAX_LINE_LENGTH, fp);
}

glm::quat ANILoader::createQuaternion(float rx, float ry, float rz)
{
	// If rotation units is degrees, convert to radians
	if (rotationUnits == "Degrees")
	{
		rx = glm::radians(rx);
		ry = glm::radians(ry);
		rz = glm::radians(rz);
	}

	// Construct unit quaternions for each axis
	glm::quat qX = glm::angleAxis(rx, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qY = glm::angleAxis(ry, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qZ = glm::angleAxis(rz, glm::vec3(0.0f, 0.0f, 1.0f));

	// Check for rotation order
	if (rotationOrder == RotationOrder::XYZ)
		return qX * qY * qZ;
	else if (rotationOrder == RotationOrder::ZYX)
		return qZ * qY * qX;
	// ... put other rotation orders here ...
	else
	{
		std::cout << "HTR Parse Warning: File has unsupported rotation order" << std::endl;
	}

	// Return XYZ by default
	return qZ * qY * qX;
}

JointDescriptor* ANILoader::getJointDescriptorByName(std::string jointName)
{
	int numJoints = jointDescriptors.size();

	for (int i = 0; i < numJoints; i++)
	{
		if (jointName == jointDescriptors[i].jointName)
			return &jointDescriptors[i];
	}

	return nullptr;
}

Node* ANILoader::getNodeByName(std::string jointName)
{
	int numJoints = jointNodes.size();

	for (int i = 0; i < numJoints; i++)
	{
		if (jointName == jointNodes[i].name)
			return &jointNodes[i];
	}

	return nullptr;
}

std::vector<glm::vec3> ANILoader::getVertexs() { return vertexsOrder; }
std::vector<glm::vec2> ANILoader::getUV() { return UVOrder; }
std::vector<glm::vec3> ANILoader::getNormals() { return normalsOrder; }
std::vector<glm::vec4> ANILoader::getWeights() { return weightsOrder; }//the weights for each vertex
std::vector<glm::vec4> ANILoader::getJoints() { return jointsOrder; }//the joint for each vertex


Holder::Holder(Node* child, std::shared_ptr<ANILoader> data)
{
	m_pChild = child;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	bones = 0;
	child->createBase(multipliedMatricies, matricies, bones);


	data->getVertexs();
	data->getUV();
	data->getNormals();
	data->getWeights();//the weights for each vertex
	data->getJoints();//the joint for each vertex

	numtris = data->getVertexs().size();

	// verts tho
	glGenBuffers(1, &vertbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*numtris, data->getVertexs().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(4); // position/vertices
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// textures tho(poopybutt)
	glGenBuffers(1, &texbo);
	glBindBuffer(GL_ARRAY_BUFFER, texbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*numtris, data->getUV().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(5); // texcoords/uv
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normals tho
	glGenBuffers(1, &normbo);
	glBindBuffer(GL_ARRAY_BUFFER, normbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*numtris, data->getNormals().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(6); //  normals
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normals tho
	glGenBuffers(1, &weightbo);
	glBindBuffer(GL_ARRAY_BUFFER, weightbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*numtris, data->getWeights().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(9); //  colors
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &bonebo);
	glBindBuffer(GL_ARRAY_BUFFER, bonebo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*numtris, data->getJoints().data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(8); //  colors
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Holder::draw(std::shared_ptr<Shader> s)
{
	s->uniformMat4x4("BoneMatrixArray", &matricies[0], bones);
	s->uniformInt("boneCount", bones);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, numtris);
	glBindVertexArray(0);
}

void Holder::update(float dt)
{
	int count = 0;
	m_pChild->update(dt);
	m_pChild->getMatrixStack(matricies, multipliedMatricies, count);
}
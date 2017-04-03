#pragma once
#include <memory>
#include <string>
#include "Node.h"
#include "JointTypes.h"
#include "loader.h"
#include <map>
class ANILoader
{
public:
	ANILoader();

	// Loads the specified HTR file. 
	// Returns true if successful
	bool loadHTR(std::string fileName);

	// Loop through each joint descriptor until the input jointName is found
	// If not found, will return a nullptr
	JointDescriptor* getJointDescriptorByName(std::string jointName);

	// Returns pointer to specific joint
	Node* getNodeByName(std::string jointName);

	// Turns the HTR file into usable game objects
	void createNodes();

	// Returns root node (usually the hip if a humanoid skeleton)
	Node* getRootNode();

	//used to get some values.
	std::vector<glm::vec3> getVertexs();
	std::vector<glm::vec2> getUV();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec4> getWeights();//the weights for each vertex
	std::vector<glm::vec4> getJoints();//the joint for each vertex

	//used to get data.
	glm::vec3* ANILoader::getVertexsData();
	glm::vec2* ANILoader::getUVData();
	glm::vec3* ANILoader::getNormalsData();
	glm::vec4* ANILoader::getWeightsData();
	glm::vec4* ANILoader::getJointsData();

	int getSegments();

private:

	// Functions used in parsing process
	bool processHeaderSection(FILE* fp, char* loc);
	bool processHeader(std::string header, std::string value);
	bool processSegementNameSection(FILE* fp, char* loc);
	bool processBasePositionSection(FILE* fp, char* loc);
	bool processAnimationSection(FILE* fp, char* loc);
	bool processVBO(FILE* fp, char* loc);

	// Increments the file pointer to the next line in the file
	// and copies it into buffer
	void goToNextValidLineInFile(FILE* fp, char* buffer);

	// Takes in 3 euler angles and returns a quaternion
	glm::quat createQuaternion(float rx, float ry, float rz);

	// Describes the data in the file

	std::string fileType;		// File extension
	std::string dataType;		// What kind of data is stored in file. 
								// Ie. HTRS means Hierarchical translations followed by rotations and scale

	int			fileVersion;	// Useful if you are generating binary object files on load, can check the version of an existing binary file, 
								// check version of text, if text version > binary version then reparse, otherwise just use binary

	int			numSegments;	// Number of bones in skeleton
	int			numFrames;		// number of frames in the animation
	int			fps;			// FPS of the animation

	RotationOrder rotationOrder;// Order to apply rotations
	std::string	calibrationUnits;// 
	char		upAxis;			// X, Y, Z (usually Y)
	std::string rotationUnits;	// "Degrees" or "Radians"
	char		boneLengthAxis;	// Axis that is aligned with bone

	// Actual animation data
	std::vector<JointDescriptor> jointDescriptors;	// Stores each joint and its parent (could be an array since we know number of segments)

	std::vector<Node> jointNodes;

	std::vector<glm::vec3> vertexsOrder;
	std::vector<glm::vec2> UVOrder;
	std::vector<glm::vec3> normalsOrder;
	std::vector<glm::vec4> weightsOrder;//the weights for each vertex
	std::vector<glm::vec4> jointsOrder;//the joint for each vertex

	std::vector<glm::vec3> vertexs;
	std::vector<glm::vec2> UV;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> weights;//the weights for each vertex
	std::vector<glm::vec4> joints;//the joint for each vertex

	Node* rootNode;
};

class Holder : public Loader
{
private:

	Node* basePose;
	std::map<std::string, Node*> animations;
	Node* currentTop;
	Node* currentBot;
	unsigned int numtris; // count number of vertices for data creation
	int bones;

	// the base pose matricies
	std::vector<glm::mat4> matricies;
	// the current transformation matrix
	std::vector<glm::mat4> multipliedMatricies;
	void createVAO();

	float angle;
public:
	//
	bool winAnimation;
	Holder() {};
	Holder(std::shared_ptr<Holder>);
	void reset();
	void setAnim(std::string);
	void overWrite(std::string);
	bool baseLoad(std::string);
	bool AniLoad(std::string, std::string);
	void draw(std::shared_ptr<ShaderProgram> s);
	void update(float, float, float);
	//used to update a portion of the skeleton.
	//void update(int frame, int max);
};

void loadAnimations(std::shared_ptr<Holder> _temp);
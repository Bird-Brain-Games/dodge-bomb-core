#include "Shader.h"
#include <GLM\gtc\type_ptr.hpp>

// Forward definition of a utility function
static char* readTextFromFile(const char *fileName);

Shader::Shader(const char *vsFile, const char *fsFile, const char* gsFile) {
	init(vsFile, fsFile, gsFile);
}

Shader::Shader()
{

}

void Shader::load(const char *vsFile, const char *fsFile, const char* gsFile) {
	init(vsFile, fsFile, gsFile);
}

Shader::~Shader(void) {
	glDetachShader(programID, vertShader);
	glDetachShader(programID, fragShader);
	if (geomShader != -1)
	{
		glDetachShader(programID, geomShader);
		glDeleteShader(geomShader);
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	glDeleteProgram(programID);
}

void Shader::init(const char *vsFile, const char *fsFile, const char* gsFile) {

	GLint result = 0;
	GLchar error[1024] = { 0 };

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	const char *vsText = readTextFromFile(vsFile);
	glShaderSource(vertShader, 1, &vsText, 0);
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(vertShader, sizeof(error), NULL, error);
		std::cerr << "Error Compiling Shader: " << error << std::endl;
	}
	else printf("Compiled!\n");

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fsText = readTextFromFile(fsFile);
	glShaderSource(fragShader, 1, &fsText, 0);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(fragShader, sizeof(error), NULL, error);
		std::cerr << "Error Compiling Shader: " << error << std::endl;
	}
	else printf("Compiled!\n");

	if (gsFile != NULL)
	{
		geomShader = glCreateShader(GL_GEOMETRY_SHADER);
		const char *fsText = readTextFromFile(gsFile);
		glShaderSource(geomShader, 1, &fsText, 0);
		glCompileShader(geomShader);
		glGetShaderiv(geomShader, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			glGetShaderInfoLog(geomShader, sizeof(error), NULL, error);
			std::cerr << "Error Compiling Shader: " << error << std::endl;
		}
		else printf("Compiled!\n");

	}
	else
		geomShader = -1;

	programID = glCreateProgram();
	// attaching the vertex and fragment shaders to the program id
	glAttachShader(programID, vertShader);
	glAttachShader(programID, fragShader);
	if (geomShader != -1)
		glAttachShader(programID, geomShader);

	// linking the programs
	glLinkProgram(programID);

	int linkStatus;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);

	if (linkStatus)
	{
		std::cout << "Shader linked successfully" << std::endl;
		return;
	}
	char infoLog[512];

	GLint infoLen;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLen);
	glGetProgramInfoLog(programID, sizeof(char) * 512, &infoLen, &infoLog[0]); 
	std::cout << infoLog << "\n";

	std::cout << "Shader program failed to link: handle not set" << std::endl;

}

void Shader::bind() {
	glUseProgram(programID);
}

void Shader::unbind() {
	glUseProgram(0);
}

unsigned int Shader::getID() {
	return programID;
}

static char* readTextFromFile(const char *fileName) {

	std::cout << "\t" << fileName << "... \t";

	static char* text;

	if (fileName != NULL) {
		FILE *file;
		fopen_s(&file, fileName, "rt");

		if (file != NULL) {
			fseek(file, 0, SEEK_END);
			int count = ftell(file);
			rewind(file);
			if (count > 0) {
				text = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(text, sizeof(char), count, file);
				text[count] = '\0';
			}
			fclose(file);
		}
		else {
			printf("ERROR: Could not find file. %s", fileName);
		}
	}

	return text;
}


void Shader::uniformVec3(const std::string& varName, float *data) {
	GLint loc = getUnifrom(varName);
	glUniform3fv(loc, 1, data);
}

void Shader::uniformVec3(const std::string& varName, float x, float y, float z) {
	float temp[3] = { x, y, z };
	GLint loc = getUnifrom(varName);
	glUniform3fv(loc, 1, temp);
}

void Shader::uniformVec3(const std::string& varName, glm::vec3* v) {
	float temp[3] = { v->x, v->y, v->z };
	GLint loc = getUnifrom(varName);
	glUniform3fv(loc, 1, temp);
}

void Shader::uniformVec2(const std::string& varName, glm::vec2& vec) {
	GLint loc = getUnifrom(varName);
	glUniform2fv(loc, 1, &vec[0]);
}

void Shader::uniformVec3(const std::string& varName, glm::vec4& vec) {
	GLint loc = getUnifrom(varName);
	glUniform3fv(loc, 1, &vec[0]);
}

void Shader::uniformVec4(const std::string& varName, glm::vec4& vec) {
	GLint loc = getUnifrom(varName);
	glUniform3fv(loc, 1, &vec[0]);
}


void Shader::uniformFloat(const std::string& varName, float data) {
	GLint loc = getUnifrom(varName);
	glUniform1f(loc, data);
}

void Shader::uniformInt(const std::string& varName, int data) {
	GLint loc = getUnifrom(varName);
	glUniform1i(loc, data);
}

void Shader::uniformMat4x4(const std::string& matName, glm::mat4x4* m, unsigned int size) {
	GLint loc = getUnifrom(matName);
	glUniformMatrix4fv(loc, size, GL_FALSE, glm::value_ptr(*m));
}

int Shader::getUnifrom(const std::string& uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}

void Shader::uniformTex(const std::string& varName, GLuint data, unsigned short activeTexture) {

	switch (activeTexture)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	default:
		glActiveTexture(GL_TEXTURE0);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, data);
	GLuint loc = getUnifrom(varName);
	glUniform1i(loc, activeTexture);
}

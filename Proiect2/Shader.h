#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/glm/gtc/type_ptr.hpp"

class Shader
{
public:
	unsigned int ID;

	Shader() {};
	Shader(const char* vertexPath, const char* fragmentPath);

	Shader& use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void SetVector3f(const char* name, float x, float y, float z);
	void SetVector3f(const char* name, const glm::vec3& value);
	void SetMatrix4(const char* name, const glm::mat4& matrix);
};

#endif
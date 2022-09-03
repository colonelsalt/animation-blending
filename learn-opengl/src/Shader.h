#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void Bind();

	void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void SetMat4(const std::string& name, const glm::mat4& value);
	void SetVec3(const std::string& name, const glm::vec3& value);

	uint32_t GetId() const { return m_RendererId; }

private:
	std::string ParseShader(const char* fileName);
	uint32_t CompileShader(const char* source, GLenum type);

	uint32_t m_RendererId;
	std::unordered_map <std::string, int> m_ShaderLocationCache;
};
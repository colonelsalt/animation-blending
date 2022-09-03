#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

std::string Shader::ParseShader(const char* fileName)
{
	std::stringstream buffer;
	std::ifstream fileStream(fileName);
	buffer << fileStream.rdbuf();
	return buffer.str();
}

uint32_t Shader::CompileShader(const char* source, GLenum type)
{
	uint32_t shaderId = glCreateShader(type);

	glShaderSource(shaderId, 1, &source, nullptr);
	glCompileShader(shaderId);

	int compileSuccess;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileSuccess);
	if (!compileSuccess)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
		
		const char* shaderTypeName = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
		std::cout << shaderTypeName << " shader failed to compile:\n" << infoLog;
		exit(1);
	}
	return shaderId;
}


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexSource = ParseShader(vertexPath);
	std::string fragmentSource = ParseShader(fragmentPath);

	uint32_t vertId = CompileShader(vertexSource.c_str(), GL_VERTEX_SHADER);
	uint32_t fragId = CompileShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

	m_RendererId = glCreateProgram();

	glAttachShader(m_RendererId, vertId);
	glAttachShader(m_RendererId, fragId);
	glLinkProgram(m_RendererId);

	int linkSuccess;
	glGetProgramiv(m_RendererId, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess)
	{
		char infoLog[512];
		glGetProgramInfoLog(m_RendererId, sizeof(infoLog), nullptr, infoLog);

		std::cout << "Shader program failed to link:\n" << infoLog;
		exit(1);
	}

	glDeleteShader(vertId);
	glDeleteShader(fragId);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererId);
}

void Shader::Bind()
{
	glUseProgram(m_RendererId);
}

void Shader::SetBool(const std::string& name, bool value)
{
	SetInt(name, (int)value);
}

void Shader::SetInt(const std::string& name, int value)
{
	if (m_ShaderLocationCache.find(name) != m_ShaderLocationCache.end())
	{
		glUniform1i(m_ShaderLocationCache.at(name), value);
		return;
	}
	int location = glGetUniformLocation(m_RendererId, name.c_str());
	m_ShaderLocationCache[name] = location;
	glUniform1i(location, value);
}

void Shader::SetFloat(const std::string& name, float value)
{
	if (m_ShaderLocationCache.find(name) != m_ShaderLocationCache.end())
	{
		glUniform1f(m_ShaderLocationCache.at(name), value);
		return;
	}
	int location = glGetUniformLocation(m_RendererId, name.c_str());
	m_ShaderLocationCache[name] = location;
	glUniform1i(location, value);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
	if (m_ShaderLocationCache.find(name) != m_ShaderLocationCache.end())
	{
		glUniformMatrix4fv(m_ShaderLocationCache.at(name), 1, GL_FALSE, glm::value_ptr(value));
		return;
	}
	int location = glGetUniformLocation(m_RendererId, name.c_str());
	m_ShaderLocationCache[name] = location;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value)
{
	if (m_ShaderLocationCache.find(name) != m_ShaderLocationCache.end())
	{
		glUniform3fv(m_ShaderLocationCache.at(name), 1, glm::value_ptr(value));
		return;
	}
	int location = glGetUniformLocation(m_RendererId, name.c_str());
	m_ShaderLocationCache[name] = location;
	glUniform3fv(location, 1, glm::value_ptr(value));
}



#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Shader.h"

enum class TextureType
{
	Diffuse, Specular
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	uint32_t Id;
	TextureType Type;
	std::string FileName;
};

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture>& textures);

	void Draw(Shader& shader);
private:
	void SetUpMesh();
private:
	uint32_t m_VertexArrayId, m_VertexBufferId, m_IndexBufferId;

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	std::vector<Texture> m_Textures;
};
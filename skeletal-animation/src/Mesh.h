#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Shader.h"

enum class TextureType
{
	Diffuse, Specular
};

static constexpr int MAX_JOINTS_PER_VERTEX = 4;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;

	// TODO: Need to work out what these things are
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	// ID of each bone whose transformation influences the pos/rot of this vertex
	int JointIds[MAX_JOINTS_PER_VERTEX];

	// How much each of those bones affect the pos/rot (S: range [0, 1]??)
	float Weights[MAX_JOINTS_PER_VERTEX];

	void ResetJointData()
	{
		for (uint32_t i = 0; i < MAX_JOINTS_PER_VERTEX; i++)
		{
			JointIds[i] = -1;
			Weights[i] = 0.0f;
		}
	}

	void SetJointData(int jointId, float weight)
	{
		for (uint32_t i = 0; i < MAX_JOINTS_PER_VERTEX; i++)
		{
			if (JointIds[i] == -1)
			{
				JointIds[i] = jointId;
				Weights[i] = weight;
				break;
			}
		}
	}
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
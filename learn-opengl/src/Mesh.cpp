#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture>& textures)
	: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
	SetUpMesh();
}

void Mesh::Draw(Shader& shader)
{
	uint32_t numDiffuseTextures = 0;
	uint32_t numSpecularTextures = 0;
	for (uint32_t i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		if (m_Textures[i].Type == TextureType::Diffuse)
		{
			numDiffuseTextures++;
			shader.SetInt("u_Material.TextureDiffuse" + std::to_string(numDiffuseTextures), i);
		}
		else if (m_Textures[i].Type == TextureType::Specular)
		{
			numSpecularTextures++;
			shader.SetInt("u_Material.TextureSpecular" + std::to_string(numSpecularTextures), i);
		}
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].Id);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(m_VertexArrayId);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

void Mesh::SetUpMesh()
{
	// Generate and bind vertex array
	glGenVertexArrays(1, &m_VertexArrayId);
	glBindVertexArray(m_VertexArrayId);

	// Generate and bind vertex buffer
	glGenBuffers(1, &m_VertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);

	// Generate and bind index buffer
	glGenBuffers(1, &m_IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);


	// Copy data into vertex buffer
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

	// Copy data into index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), m_Indices.data(), GL_STATIC_DRAW);


	// Set up vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIds));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

	for (const Vertex& vertex : m_Vertices)
	{
		for (int boneId : vertex.BoneIds)
		{
			if (boneId >= 100)
				__debugbreak();
		}
	}
}

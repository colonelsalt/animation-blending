#pragma once

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model
{
public:
	Model(const char* path);

	void Draw(Shader& shader);
private:
	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, TextureType type);
private:
	std::vector<Mesh> m_Meshes;
	std::string m_DirectoryPath;

	std::unordered_map<std::string, Texture> m_TexturesLoaded;
};


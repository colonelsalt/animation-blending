#pragma once

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Animation/JointDirectory.h"

class Model
{
public:
	Model(const char* path, const std::shared_ptr<JointDirectory>& jointDirectory);

	void Draw(Shader& shader);
private:
	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, TextureType type, const aiScene* scene);
	void ExtractJoints(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

private:
	std::vector<Mesh> m_Meshes;
	std::string m_DirectoryPath;

	std::unordered_map<std::string, Texture> m_TexturesLoaded;

	std::shared_ptr<JointDirectory> m_JointDirectory;
};


#pragma once

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

struct BoneInfo
{
	int Id;

	// Transforms from local bone space to model space
	glm::mat4 InverseBindPose;
};

class Model
{
public:
	Model(const char* path);

	void Draw(Shader& shader);
	int AppendBone(const std::string& name, const glm::mat4& inverseBindPose);
	bool ContainsBone(const std::string& boneName) const
	{
		return m_BonesLoaded.find(boneName) != m_BonesLoaded.end();
	}
	BoneInfo& GetBone(const std::string& boneName)
	{
		if (!ContainsBone(boneName))
			__debugbreak();
		//assert(ContainsBone(boneName));
		return m_BonesLoaded[boneName];
	}
private:
	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, TextureType type);
	void ExtractMeshBoneData(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

private:
	std::vector<Mesh> m_Meshes;
	std::string m_DirectoryPath;

	std::unordered_map<std::string, Texture> m_TexturesLoaded;
	
	//! The same bone may be described multiple times in different meshes belonging to the same model.
	//! We keep track of which bones we've already loaded (keyed by bone name) to ensure we can refer
	//! to a previously loaded bone if we encounter one in a different mesh.
	std::unordered_map<std::string, BoneInfo> m_BonesLoaded;

	//! Used to generate the internal ID of a bone
	int m_NumBonesLoaded;

};


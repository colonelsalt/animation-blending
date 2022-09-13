#pragma once

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

struct Joint
{
	int Id;

	//! The inverse of the joint's model space position when in the bind pose.
	//! Another way of looking at it: The position of the model space origin seen from joint space.
	//! Applying first the model space transform of this joint and then this matrix results in
	//! an *offset* matrix describing the shift in relative position the joint has gone through.
	glm::mat4 InverseBindPose;
};

class Model
{
public:
	Model(const char* path);

	void Draw(Shader& shader);
	bool ContainsJoint(const std::string& boneName) const
	{
		return m_JointIndex.find(boneName) != m_JointIndex.end();
	}
	const Joint& GetJoint(const std::string& boneName)
	{
		return m_JointIndex.at(boneName);
	}
private:
	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, TextureType type, const aiScene* scene);
	void ExtractJoints(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	int AppendJoint(const std::string& name, const glm::mat4& inverseBindPose);

private:
	std::vector<Mesh> m_Meshes;
	std::string m_DirectoryPath;

	std::unordered_map<std::string, Texture> m_TexturesLoaded;
	
	//! The same joint may be described multiple times in different meshes belonging to the same model.
	//! We keep track of which joints we've already loaded (keyed by joint name) to ensure we can refer
	//! to a previously loaded joint if we encounter one in a different mesh.
	std::unordered_map<std::string, Joint> m_JointIndex;

	//! Used to generate the internal ID of a joint
	int m_NumJointsLoaded;

};


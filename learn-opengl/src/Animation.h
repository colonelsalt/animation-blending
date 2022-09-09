#pragma once

#include "Model.h"
#include "Bone.h"

struct BoneNode
{
	//! Relative to parent bone
	glm::mat4 Transform;
	std::string Name;
	std::vector<BoneNode> Children;
	BoneInfo* Info;
	bool IsUnknown = false;
};


class Animation
{
public:
	Animation(const std::string& filePath, Model* model);

	const BoneNode& GetRootBone() const { return m_RootNode; }
	int GetTicksPerSecond() const { return m_TicksPerSecond; }
	float GetDuration() const { return m_Duration; }
	Bone* GetBoneAnimation(const std::string& boneName)
	{
		if (m_Bones.find(boneName) == m_Bones.end())
			return nullptr;
		return &m_Bones[boneName];
	}
private:
	void ReadNode(BoneNode& dstNode, const aiNode* srcNode, Model& model);
	void ExtractMissingBones(const aiAnimation* animation, Model& model);
private:
	float m_Duration;
	int m_TicksPerSecond;
	std::unordered_map<std::string, Bone> m_Bones;
	BoneNode m_RootNode;

	uint32_t m_NumBoneNodes = 0;
};


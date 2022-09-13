#pragma once

#include "Model.h"
#include "JointClip.h"

struct SkeletonNode
{
	//! Relative to parent joint
	glm::mat4 Transform;
	std::string Name;
	std::vector<SkeletonNode> Children;

	//! If this is a joint (i.e. is bound to one/more vertices), contains data about the joint; otherwise nullptr.
	const Joint* JointInfo;
};


class Animation
{
public:
	Animation(const std::string& filePath, Model* model, bool shouldFreezeTranslation = false);

	const SkeletonNode& GetRootNode() const { return m_RootNode; }
	int GetTicksPerSecond() const { return m_TicksPerSecond; }
	float GetDuration() const { return m_Duration; }
	JointClip* GetJointClip(const std::string& jointName)
	{
		if (m_JointClips.find(jointName) == m_JointClips.end())
			return nullptr;
		return &m_JointClips.at(jointName);
	}
	const std::string& GetName() const { return m_Name; }
	bool IsPlaying() const { return m_IsPlaying; }
	float ToLocalTime(float normalisedTime) const { return normalisedTime * m_Duration; }

private:
	void ReadNode(SkeletonNode& dstNode, const aiNode* srcNode, Model& model);
	void CreateJointClips(const aiAnimation* animation);
private:
	float m_Duration;
	float m_IsPlaying = false;

	int m_TicksPerSecond;
	bool m_ShouldFreezeTranslation;
	std::unordered_map<std::string, JointClip> m_JointClips;
	SkeletonNode m_RootNode;
	std::string m_Name;

	uint32_t m_NumBoneNodes = 0;
};


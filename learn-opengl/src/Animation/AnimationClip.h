#pragma once

#include "AnimationNode.h"
#include "JointClip.h"

#include "../Model.h"

class AnimationClip : public AnimationNode
{
public:
	AnimationClip(const std::string& filePath, const std::shared_ptr<JointDirectory>& jointDirectory, bool shouldFreezeTranslation = false);

	void UpdateLocalPoses(float animationTime) override;
	const std::unordered_map<std::string, LocalPose>& GetLocalPoses() const override { return m_LocalPoses; }

	float GetTicksPerSecond() const override { return m_LocalTicksPerSecond; }
	float GetDuration() const override { return m_LocalDuration; }
private:
	void CreateJointClips(const aiAnimation* animation);
private:
	std::string m_Name;
	bool m_ShouldFreezeTranslation;

	std::unordered_map<std::string, JointClip> m_JointClips;
	std::unordered_map<std::string, LocalPose> m_LocalPoses;
	std::shared_ptr<JointDirectory> m_JointDirectory;

	//! The duration (in "ticks") the animation clip has been authored for
	float m_LocalDuration;

	//! How many "ticks" this animation clip is intended to proceed per second
	float m_LocalTicksPerSecond;
};
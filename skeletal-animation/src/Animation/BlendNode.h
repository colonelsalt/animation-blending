#pragma once

#include "AnimationNode.h"

class BlendNode : public AnimationNode
{
public:
	BlendNode(AnimationNode* sourceNode, AnimationNode* targetNode);

	void SetTargetWeight(float targetWeight);

	void UpdateLocalPoses(float animationTime) override;
	const std::unordered_map<std::string, LocalPose>& GetLocalPoses() const override { return m_LocalPoses; };

	float GetTicksPerSecond() const override { return m_TicksPerSecond; }
	float GetDuration() const override { return m_Duration; }
private:
	AnimationNode* m_SourceNode;
	AnimationNode* m_TargetNode;
	std::unordered_map<std::string, LocalPose> m_LocalPoses;

	float m_TargetWeight;
	float m_Duration;
	float m_TicksPerSecond;

	//! How much to scale the source clip's TPS by so it's duration matches the
	//! target duration at t = 1.
	float m_SourceTpsScale;
	
	float m_TimePassed = 0.0f;
	float m_CurrentTime;
};
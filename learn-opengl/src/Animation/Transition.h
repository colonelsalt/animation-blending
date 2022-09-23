#pragma once

#include <unordered_map>
#include <string>

#include "AnimationNode.h"

class AnimationState;

class Transition
{
public:
	Transition(AnimationState* sourceState, AnimationState* targetState, float duration);
	void Update(float deltaTime);

	const std::unordered_map<std::string, LocalPose>& GetLocalPoses() const { return m_LocalPoses; }

	AnimationState* GetSourceState() const { return m_SourceState; }
	AnimationState* GetTargetState() const { return m_TargetState; }
private:
	AnimationState* m_SourceState;
	AnimationState* m_TargetState;

	std::unordered_map<std::string, LocalPose> m_LocalPoses;

	float m_Duration;
	float m_TimePassed = 0.0f;
};
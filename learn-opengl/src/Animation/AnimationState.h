#pragma once

#include <unordered_map>
#include <memory>

#include "AnimationNode.h"
#include "Transition.h"

template <typename T>
struct AnimationVar
{
	T Value;
	T MinValue;
	T MaxValue;
};

class AnimationState
{
public:
	AnimationState(std::string&& name, AnimationNode* animation, bool shouldLoop = false);

	void Update(float deltaTime);

	template <typename T>
	void AddVar(const std::string& name, AnimationVar<T>&& var);

	const std::unordered_map<std::string, LocalPose>& GetLocalPoses() const { return m_Animation->GetLocalPoses(); }
	void SetOnCompleteTransition(Transition* transition) { m_OnCompleteTransition = transition; }
	void AddTriggerTransition(std::string&& triggerName, Transition* transition);

	void SetTrigger(const std::string& name);
	
	template <typename T>
	void SetVar(const std::string& name, T value);

private:
	std::string m_Name;

	float m_AnimationTime = 0.0f;
	bool m_ShouldLoop;

	std::unordered_map<std::string, AnimationVar<float>> m_FloatVars;

	AnimationNode* m_Animation;

	Transition* m_OnCompleteTransition = nullptr;
	std::unordered_map<std::string, Transition*> m_OnTriggerTransitions;
};
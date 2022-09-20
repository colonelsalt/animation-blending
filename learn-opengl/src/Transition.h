#pragma once

#include "AnimationState.h"

class Transition
{
public:
	enum class TriggerCondition
	{
		OnComplete, OnTrigger
	};

	Transition(const AnimationState* sourceState, const AnimationState* targetState, float duration, TriggerCondition condition);

private:
	AnimationState* m_SourceState;
	AnimationState* m_TargetState;

	float m_Duration;
	TriggerCondition m_Condition;
};
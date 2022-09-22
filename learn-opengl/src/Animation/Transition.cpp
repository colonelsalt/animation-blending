#include "Transition.h"

#include "BlendHelper.h"
#include "Animator.h"

Transition::Transition(AnimationState* sourceState, AnimationState* targetState, float duration)
	: m_SourceState(sourceState), m_TargetState(targetState), m_Duration(duration)
{

}

void Transition::Update(float deltaTime)
{
	m_TimePassed += deltaTime;

	if (m_TimePassed >= m_Duration)
	{
		Animator::GetInstance()->OnTransitionFinished(this);
		// Reset
		m_TimePassed = 0.0f;
		return;
	}

	m_SourceState->Update(deltaTime);
	m_TargetState->Update(deltaTime);

	BlendHelper::BlendPoses(m_LocalPoses, m_SourceState->GetLocalPoses(), m_TargetState->GetLocalPoses(), m_TimePassed / m_Duration);
}
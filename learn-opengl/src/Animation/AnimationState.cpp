#include "AnimationState.h"

#include "Animator.h"
#include "BlendNode.h"

AnimationState::AnimationState(std::string&& name, AnimationNode* animation, bool shouldLoop)
	: m_Name(std::move(name)), m_Animation(animation), m_ShouldLoop(shouldLoop)
{
}

void AnimationState::AddTriggerTransition(std::string&& triggerName, Transition* transition)
{
	S_ASSERT(transition);
	m_OnTriggerTransitions[triggerName] = transition;
}

void AnimationState::Update(float deltaTime)
{
	m_AnimationTime += deltaTime * m_Animation->GetTicksPerSecond();
	if (m_ShouldLoop)
	{
		m_AnimationTime = fmod(m_AnimationTime, m_Animation->GetDuration());
	}
	else
	{
		m_AnimationTime = glm::max(m_AnimationTime, m_Animation->GetDuration());
		if (m_OnCompleteTransition)
		{
			Animator::GetInstance()->OnStateFinished(this, m_OnCompleteTransition);
			m_AnimationTime = 0.0f;
			return;
		}
	}

	m_Animation->UpdateLocalPoses(m_AnimationTime);
}

void AnimationState::SetTrigger(const std::string& name)
{
	if (m_OnTriggerTransitions.find(name) != m_OnTriggerTransitions.end())
	{
		Animator::GetInstance()->OnStateFinished(this, m_OnTriggerTransitions.at(name));
	}
}

template <>
void AnimationState::AddVar<float>(const std::string& name, AnimationVar<float>&& var)
{
	m_FloatVars[name] = std::move(var);
}

template <>
void AnimationState::SetVar<float>(const std::string& name, float value)
{
	if (m_FloatVars.find(name) != m_FloatVars.end())
	{
		AnimationVar<float>& var = m_FloatVars.at(name);
		BlendNode* blendNode = dynamic_cast<BlendNode*>(m_Animation);
		if (blendNode)
		{
			float t = (value - var.MinValue) / var.MaxValue;
			blendNode->SetTargetWeight(t);
		}
	}

}
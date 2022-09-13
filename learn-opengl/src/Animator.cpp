#include "Animator.h"


Animator::Animator(Animation* animation, bool shouldLoop)
	: m_CurrentAnimation(animation), m_CurrentTime(0.0f), m_ShouldLoop(shouldLoop)
{
	m_SkinningMatrices.reserve(MAX_TOTAL_BONES);
	for (uint32_t i = 0; i < MAX_TOTAL_BONES; i++)
	{
		m_SkinningMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::Update(float deltaTime)
{
	if (!m_CurrentAnimation || !m_IsRunning)
		return;

	m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * deltaTime;
	if (m_ShouldLoop)
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
	else
		m_CurrentTime = glm::max(m_CurrentTime, m_CurrentAnimation->GetDuration());

	UpdateJointTransforms(m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
}

void Animator::UpdateJointTransforms(const SkeletonNode& node, const glm::mat4& parentTransform)
{
	// Default value if this node is not animated
	glm::mat4 jointTransform = node.Transform;

	JointClip* jointClip = m_CurrentAnimation->GetJointClip(node.Name);
	if (jointClip)
	{
		jointClip->Update(m_CurrentTime);
		jointTransform = jointClip->GetLocalPose();
	}

	glm::mat4 modelSpaceTransform = parentTransform * jointTransform;
	
	// If this node is a joint (i.e. is bound to a vertex), update its skinning matrix
	if (node.JointInfo)
	{
		int jointIndex = node.JointInfo->Id;
		const glm::mat4& inverseBindPose = node.JointInfo->InverseBindPose;

		m_SkinningMatrices[jointIndex] = modelSpaceTransform * inverseBindPose;
	}

	for (uint32_t i = 0; i < node.Children.size(); i++)
	{
		UpdateJointTransforms(node.Children[i], modelSpaceTransform);
	}

}

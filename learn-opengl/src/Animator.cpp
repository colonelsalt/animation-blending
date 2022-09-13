#include "Animator.h"

#include "Core.h"

Animator::Animator(bool shouldLoop)
	: m_ShouldLoop(shouldLoop)
{
	m_SkinningMatrices.reserve(MAX_TOTAL_BONES);
	for (uint32_t i = 0; i < MAX_TOTAL_BONES; i++)
	{
		m_SkinningMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::Update(float deltaTime)
{
	if (!m_IsRunning)
		return;

	m_CurrentTime += m_TicksPerSecond * deltaTime;
	if (m_ShouldLoop)
		m_CurrentTime = fmod(m_CurrentTime, 1.0f);
	else
		m_CurrentTime = glm::max(m_CurrentTime, 1.0f);

	UpdateJointTransforms(m_Animations[0].first.GetRootNode(), glm::mat4(1.0f));
}

void Animator::AddAnimation(Animation& animation, float weight)
{
	m_Animations.push_back({ animation, weight });
}

void Animator::SetWeight(const std::string& animationName, float weight)
{
	for (uint32_t i = 0; i < m_Animations.size(); i++)
	{
		if (animationName == m_Animations[i].first.GetName())
		{
			m_Animations[i].second = weight;
			return;
		}
	}
	S_ASSERT(false);
}

void Animator::UpdateJointTransforms(const SkeletonNode& node, const glm::mat4& parentTransform)
{
	// Default value if this node is not animated
	glm::mat4 jointTransform = node.Transform;
	
	glm::vec3 translation = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(0.0f);
	bool isAnimated = false;

	std::vector<glm::vec3> scales;
	for (auto& [animation, weight] : m_Animations)
	{
		JointClip* jointClip = animation.GetJointClip(node.Name);
		if (jointClip)
		{
			isAnimated = true;

			jointClip->Update(animation.ToLocalTime(m_CurrentTime));
			jointTransform = jointClip->GetLocalPose();

			translation += weight * jointClip->GetTranslation();
			rotation = rotation + weight * jointClip->GetRotation();
			scale += weight * jointClip->GetScale();
		}
	}
	if (isAnimated)
	{
		jointTransform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::normalize(rotation))
			* glm::scale(glm::mat4(1.0f), scale);
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

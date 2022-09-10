#include "Animator.h"

static void PrintBoneTransforms(const std::vector<glm::mat4>& boneTransforms)
{
	int boneIndex = 0;
	for (const glm::mat4& transform : boneTransforms)
	{
		std::cout << "Bone " << boneIndex << ": " << std::endl;
		
		std::cout << transform[0][0] << ", ";
		std::cout << transform[1][0] << ", ";
		std::cout << transform[2][0] << ", ";
		std::cout << transform[3][0] << ", " << std::endl;
		
		std::cout << transform[0][1] << ", ";
		std::cout << transform[1][1] << ", ";
		std::cout << transform[2][1] << ", ";
		std::cout << transform[3][1] << ", " << std::endl;

		std::cout << transform[0][2] << ", ";
		std::cout << transform[1][2] << ", ";
		std::cout << transform[2][2] << ", ";
		std::cout << transform[3][2] << ", " << std::endl;

		std::cout << transform[0][3] << ", ";
		std::cout << transform[1][3] << ", ";
		std::cout << transform[2][3] << ", ";
		std::cout << transform[3][3] << ", " << std::endl;

		boneIndex++;
	}

}

Animator::Animator(Animation* animation, bool shouldLoop)
	: m_CurrentAnimation(animation), m_CurrentTime(0.0f), m_ShouldLoop(shouldLoop)
{
	m_BoneTransformations.reserve(MAX_TOTAL_BONES);
	for (uint32_t i = 0; i < MAX_TOTAL_BONES; i++)
	{
		m_BoneTransformations.push_back(glm::mat4(1.0f));
	}
}

void Animator::Update(float deltaTime)
{
	if (!m_CurrentAnimation)
		return;

	m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * deltaTime;
	if (m_ShouldLoop)
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
	else
		m_CurrentTime = glm::max(m_CurrentTime, m_CurrentAnimation->GetDuration());

	UpdateBoneTransform(m_CurrentAnimation->GetRootBone(), glm::mat4(1.0f));

	//PrintBoneTransforms(m_BoneTransformations);
}

void Animator::UpdateBoneTransform(const BoneNode& bone, const glm::mat4& parentTransform)
{
	glm::mat4 boneTransform = bone.Transform;

	Bone* boneAnimation = m_CurrentAnimation->GetBoneAnimation(bone.Name);
	if (boneAnimation)
	{
		boneAnimation->Update(m_CurrentTime);
		boneTransform = boneAnimation->GetLocalPose();
	}

	glm::mat4 modelSpaceTransform = parentTransform * boneTransform;
	if (bone.Info)
	{
		int boneIndex = bone.Info->Id;
		const glm::mat4& inverseBindPose = bone.Info->InverseBindPose;

		// Final matrix describes the bone's offset from its bind pose
		m_BoneTransformations[boneIndex] = modelSpaceTransform * inverseBindPose;
	}

	for (uint32_t i = 0; i < bone.Children.size(); i++)
	{
		UpdateBoneTransform(bone.Children[i], modelSpaceTransform);
	}

}

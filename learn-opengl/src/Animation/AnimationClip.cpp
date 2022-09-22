#include "AnimationClip.h"

#include "../Core.h"
#include "../AssimpHelper.h"

AnimationClip::AnimationClip(const std::string& filePath, const std::shared_ptr<JointDirectory>& jointDirectory, bool shouldFreezeTranslation)
	: m_ShouldFreezeTranslation(shouldFreezeTranslation), m_JointDirectory(jointDirectory)
{
	m_Name = filePath.substr(filePath.find_last_of('/') + 1);
	std::cout << "Loaded animation: " << m_Name << std::endl;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
	S_ASSERT(scene && scene->mRootNode);

	// TODO: Let's not hardcode just grabbing the first animation
	aiAnimation* animation = scene->mAnimations[0];
	m_LocalDuration = animation->mDuration;
	m_LocalTicksPerSecond = animation->mTicksPerSecond;

	m_JointDirectory->ParseRootNode(scene->mRootNode);
	CreateJointClips(animation);
}

void AnimationClip::UpdateLocalPoses(float animationTime)
{
	S_ASSERT(animationTime >= 0 && animationTime <= 1);

	float localTime = animationTime * m_LocalDuration;
	for (auto& [name, jointClip] : m_JointClips)
	{
		jointClip.Update(localTime);
		m_LocalPoses[name] = jointClip.GetLocalPose();
	}
}

void AnimationClip::CreateJointClips(const aiAnimation* animation)
{
	// Note: There may be joints here that were not present when we parsed the mesh
	// i.e. joints that are not bound to vertices, but which still move and affect
	// the transforms of their child joints
	for (uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];

		std::string jointName = std::string(channel->mNodeName.C_Str());

		JointClip jointClip(jointName, channel, m_ShouldFreezeTranslation);
		m_JointClips.emplace(jointName, jointClip);
	}
}
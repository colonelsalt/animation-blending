#include "Animation.h"

#include "AssimpHelper.h"

#include "Core.h"

Animation::Animation(const std::string& filePath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
	S_ASSERT(scene && scene->mRootNode);

	// TODO: Let's not hardcode just grabbing the first animation
	aiAnimation* animation = scene->mAnimations[0];
	m_Duration = animation->mDuration;
	m_TicksPerSecond = animation->mTicksPerSecond;

	ReadNode(m_RootNode, scene->mRootNode, *model);
	CreateJointClips(animation);
}

void Animation::ReadNode(SkeletonNode& dstNode, const aiNode* srcNode, Model& model)
{
	S_ASSERT(srcNode);

	dstNode.Name = std::string(srcNode->mName.C_Str());
	dstNode.Transform = AssimpHelper::AssimpToGlmMatrix(srcNode->mTransformation);

	if (!model.ContainsJoint(dstNode.Name))
	{
		// Node is not a joint (is not bound to any vertex)
		dstNode.JointInfo = nullptr;
	}
	else
	{
		dstNode.JointInfo = &model.GetJoint(dstNode.Name);
	}

	for (uint32_t i = 0; i < srcNode->mNumChildren; i++)
	{
		SkeletonNode childNode;
		ReadNode(childNode, srcNode->mChildren[i], model);
		dstNode.Children.push_back(childNode);

	}
	m_NumBoneNodes++;
}

void Animation::CreateJointClips(const aiAnimation* animation)
{
	// Note: There may be joints here that were not present when we parsed the mesh
	// i.e. joints that are not bound to vertices, but which still move and affect
	// the transforms of their child joints
	for (uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];

		std::string jointName = std::string(channel->mNodeName.C_Str());

		JointClip jointClip(jointName, channel);
		m_JointClips.emplace(jointName, jointClip);
	}
}

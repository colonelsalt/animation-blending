#include "Animation.h"

#include "AssimpHelper.h"

Animation::Animation(const std::string& filePath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);

	// TODO: Let's not hardcode just grabbing the first animation
	aiAnimation* animation = scene->mAnimations[0];
	m_Duration = animation->mDuration;
	m_TicksPerSecond = animation->mTicksPerSecond;

	ReadNode(m_RootNode, scene->mRootNode, *model);
	ExtractMissingBones(animation, *model);

	std::cout << "We've got " + std::to_string(m_Bones.size()) + " animation bones and "
		+ std::to_string(m_NumBoneNodes) + " bone nodes..." << std::endl;
}

void Animation::ReadNode(BoneNode& dstNode, const aiNode* srcNode, Model& model)
{
	assert(srcNode);

	dstNode.Name = std::string(srcNode->mName.C_Str());
	dstNode.Transform = AssimpHelper::AssimpToGlmMatrix(srcNode->mTransformation); //glm::mat4(1.0f);

	if (!model.ContainsBone(dstNode.Name))
	{
		//__debugbreak();
		std::cout << "WARNING: Found unknown bone: " << dstNode.Name << std::endl;
		dstNode.IsUnknown = true;
		dstNode.Info = nullptr;
	}
	else
	{
		dstNode.Info = &model.GetBone(dstNode.Name);
	}

	for (uint32_t i = 0; i < srcNode->mNumChildren; i++)
	{
		BoneNode childNode;
		ReadNode(childNode, srcNode->mChildren[i], model);
		dstNode.Children.push_back(childNode);

	}
	m_NumBoneNodes++;
}

void Animation::ExtractMissingBones(const aiAnimation* animation, Model& model)
{
	for (uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* animNode = animation->mChannels[i];

		std::string boneName = std::string(animNode->mNodeName.C_Str());
		if (!model.ContainsBone(boneName))
		{
			std::cout << "Found unknown bone in animation not present in model: " << boneName << std::endl;
			//__debugbreak();
		}
		const BoneInfo& boneInfo = model.GetBone(boneName);

		Bone bone(boneName, boneInfo.Id, animNode);
		m_Bones.emplace(boneName, bone);
	}
}

#include "JointDirectory.h"

#include "../AssimpHelper.h"

void JointDirectory::ParseRootNode(const aiNode* rootNode)
{
	// TODO: Verify that it's indeed safe to only parse the skeleton once for all animations of the same model
	if (m_RootNode.Children.empty())
	{
		ReadNode(m_RootNode, rootNode);
	}
}

void JointDirectory::ReadNode(SkeletonNode& dstNode, const aiNode* srcNode)
{
	S_ASSERT(srcNode);

	dstNode.Name = std::string(srcNode->mName.C_Str());
	dstNode.Transform = AssimpHelper::AssimpToGlmMatrix(srcNode->mTransformation);

	for (uint32_t i = 0; i < srcNode->mNumChildren; i++)
	{
		SkeletonNode childNode;
		ReadNode(childNode, srcNode->mChildren[i]);
		dstNode.Children.push_back(childNode);
	}
}

int JointDirectory::AppendJoint(const std::string& name, const glm::mat4& inverseBindPose)
{
	// Joint may have been seen before in a different mesh in the same model
	if (!ContainsJoint(name))
	{
		Joint joint;
		joint.Id = m_NumJointsLoaded;
		joint.InverseBindPose = inverseBindPose;

		m_Directory[name] = joint;
		m_NumJointsLoaded++;
	}
	return m_Directory.at(name).Id;
}
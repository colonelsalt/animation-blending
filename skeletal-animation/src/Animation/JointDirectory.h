#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Core.h"

struct Joint
{
	int Id;

	//! The inverse of the joint's model space position when in the bind pose.
	//! Another way of looking at it: The position of the model space origin seen from joint space.
	//! Applying first the model space transform of this joint and then this matrix results in
	//! an *offset* matrix describing the shift in relative position the joint has gone through.
	glm::mat4 InverseBindPose;
};

//! Not all SkeletonNodes may be joints (i.e. bound to vertices) - some might be there just to
//! contribute a parent transform to a child joint's final pose.
struct SkeletonNode
{
	//! Relative to parent joint
	glm::mat4 Transform;
	std::string Name;
	std::vector<SkeletonNode> Children;
};

//! The same joint may be described multiple times in different meshes belonging to the same model.
//! We keep track of which joints we've already loaded (keyed by joint name) to ensure we can refer
//! to a previously loaded joint if we encounter one in a different mesh.
class JointDirectory
{
public:
	bool ContainsJoint(const std::string& jointName) const { return m_Directory.find(jointName) != m_Directory.end(); }
	const Joint& GetJoint(const std::string& name) const { S_ASSERT(ContainsJoint(name)); return m_Directory.at(name); }
	int AppendJoint(const std::string& name, const glm::mat4& inverseBindPose);
	void ParseRootNode(const aiNode* rootNode);
	const SkeletonNode& GetRootNode() const { S_ASSERT(!m_RootNode.Children.empty()) return m_RootNode; }
private:
	void ReadNode(SkeletonNode& dstNode, const aiNode* srcNode);
private:
	std::unordered_map<std::string, Joint> m_Directory;

	SkeletonNode m_RootNode;

	//! Used to generate the internal ID of a joint
	int m_NumJointsLoaded = 0;
};
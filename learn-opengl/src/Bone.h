#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

struct PositionKeyFrame
{
	glm::vec3 Position;
	float Timestamp; // time in animation at which this bone should be at `Position`
};

struct RotationKeyFrame
{
	glm::quat Rotation;
	float Timestamp;
};

struct ScaleKeyFrame
{
	glm::vec3 Scale;
	float Timestamp;
};


// Represents the state of a bone in a single animation
class Bone
{
public:
	Bone();
	Bone(const std::string& name, int id, const aiNodeAnim* channel);
	
	//! Interpolates local pose of bone between key frames of animation according to animation time
	void Update(float animationTime);

	const glm::mat4& GetLocalPose() const { return m_LocalPose; }
	const std::string& GetName() const { return m_Name; }
	int GetId() const { return m_Id; }

private:
	glm::mat4 InterpolatePosition(float animationTime) const;
	glm::mat4 InterpolateRotation(float animationTime) const;
	glm::mat4 InterpolateScale(float animationTime) const;

	int GetPositionIndex(float animationTime) const;
	int GetRotationIndex(float animationTime) const;
	int GetScaleIndex(float animationTime) const;

	static float GetLerpParam(float prevKeyTime, float nextKeyTime, float currentTime);

private:
	std::vector<PositionKeyFrame> m_PositionKeys;
	std::vector<RotationKeyFrame> m_RotationKeys;
	std::vector<ScaleKeyFrame> m_ScaleKeys;

	glm::mat4 m_LocalPose; // transformation of bone relative to its parent
	std::string m_Name;
	int m_Id;
};
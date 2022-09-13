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
	float Timestamp; // time in animation at which this joint should be at `Position`
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


// Represents the state of a joint in a single animation
class JointClip
{
public:
	JointClip(const std::string& name, const aiNodeAnim* channel);
	
	//! Interpolates local pose of joint between key frames of animation according to animation time
	void Update(float animationTime);

	const glm::mat4& GetLocalPose() const { return m_LocalPose; }
	const std::string& GetName() const { return m_Name; }

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

	glm::mat4 m_LocalPose; // transform of joint relative to its parent
	std::string m_Name;
};
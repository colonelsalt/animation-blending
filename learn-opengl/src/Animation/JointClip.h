#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AnimationNode.h"

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
	JointClip(const std::string& name, const aiNodeAnim* channel, bool shouldFreezeTranslation = false);
	
	//! Interpolates local pose of joint between key frames of animation according to animation time
	void Update(float animationTime);

	const LocalPose& GetLocalPose() const { return m_LocalPose; }
	const std::string& GetName() const { return m_Name; }

private:
	glm::vec3 InterpolatePosition(float animationTime) const;
	glm::quat InterpolateRotation(float animationTime) const;
	glm::vec3 InterpolateScale(float animationTime) const;

	int GetPositionIndex(float animationTime) const;
	int GetRotationIndex(float animationTime) const;
	int GetScaleIndex(float animationTime) const;

	static float GetLerpParam(float prevKeyTime, float nextKeyTime, float currentTime);

private:
	bool m_ShouldFreezeTranslation;

	std::vector<PositionKeyFrame> m_PositionKeys;
	std::vector<RotationKeyFrame> m_RotationKeys;
	std::vector<ScaleKeyFrame> m_ScaleKeys;

	LocalPose m_LocalPose; // transform of joint relative to its parent
	std::string m_Name;
};
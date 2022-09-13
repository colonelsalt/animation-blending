#include "JointClip.h"

JointClip::JointClip(const std::string& name, const aiNodeAnim* channel)
	: m_LocalPose(1.0f), m_Name(name)
{
	m_PositionKeys.reserve(channel->mNumPositionKeys);
	for (uint32_t i = 0; i < channel->mNumPositionKeys; i++)
	{
		aiVector3D& pos = channel->mPositionKeys[i].mValue;
		float timestamp = channel->mPositionKeys[i].mTime;

		PositionKeyFrame posKey = { glm::vec3(pos.x, pos.y, pos.z), timestamp};
		m_PositionKeys.push_back(posKey);
	}

	m_RotationKeys.reserve(channel->mNumRotationKeys);
	for (uint32_t i = 0; i < channel->mNumRotationKeys; i++)
	{
		aiQuaternion& rot = channel->mRotationKeys[i].mValue;
		float timestamp = channel->mRotationKeys[i].mTime;

		RotationKeyFrame rotKey = { glm::quat(rot.w, rot.x, rot.y, rot.z), timestamp };
		m_RotationKeys.push_back(rotKey);
	}

	m_ScaleKeys.reserve(channel->mNumScalingKeys);
	for (uint32_t i = 0; i < channel->mNumScalingKeys; i++)
	{
		aiVector3D& scale = channel->mScalingKeys[i].mValue;
		float timestamp = channel->mScalingKeys[i].mTime;

		ScaleKeyFrame scaleKey = { glm::vec3(scale.x, scale.y, scale.z), timestamp };
		m_ScaleKeys.push_back(scaleKey);
	}
}

void JointClip::Update(float animationTime)
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	glm::mat4 scale = InterpolateScale(animationTime);

	// S: Why this multiplication order?? Would any other do??
	m_LocalPose = translation * rotation * scale;
}

glm::mat4 JointClip::InterpolatePosition(float animationTime) const
{
	if (m_PositionKeys.size() == 1)
		return glm::translate(glm::mat4(1.0f), m_PositionKeys[0].Position);

	int currentKeyIndex = GetPositionIndex(animationTime);

	const PositionKeyFrame& currentKey = m_PositionKeys[currentKeyIndex];
	const PositionKeyFrame& nextKey = m_PositionKeys[currentKeyIndex + 1];

	float t = GetLerpParam(currentKey.Timestamp, nextKey.Timestamp, animationTime);
	glm::vec3 interpolatedPos = glm::mix(currentKey.Position, nextKey.Position, t);

	return glm::translate(glm::mat4(1.0f), interpolatedPos);
}

glm::mat4 JointClip::InterpolateRotation(float animationTime) const
{
	if (m_RotationKeys.size() == 1)
	{
		const glm::quat& rotation = m_RotationKeys[0].Rotation;
		return glm::toMat4(glm::normalize(rotation));
	}

	int currentKeyIndex = GetRotationIndex(animationTime);

	const RotationKeyFrame& currentKey = m_RotationKeys[currentKeyIndex];
	const RotationKeyFrame& nextKey = m_RotationKeys[currentKeyIndex + 1];

	float t = GetLerpParam(currentKey.Timestamp, nextKey.Timestamp, animationTime);
	glm::quat interpolatedRot = glm::slerp(currentKey.Rotation, nextKey.Rotation, t);

	// S: Why does a quaternion have to be normalised??
	return glm::toMat4(glm::normalize(interpolatedRot));
}

glm::mat4 JointClip::InterpolateScale(float animationTime) const
{
	if (m_ScaleKeys.size() == 1)
		return glm::scale(glm::mat4(1.0f), m_ScaleKeys[0].Scale);

	int currentKeyIndex = GetScaleIndex(animationTime);

	const ScaleKeyFrame& currentKey = m_ScaleKeys[currentKeyIndex];
	const ScaleKeyFrame& nextKey = m_ScaleKeys[currentKeyIndex + 1];

	float t = GetLerpParam(currentKey.Timestamp, nextKey.Timestamp, animationTime);
	glm::vec3 interpolatedScale = glm::mix(currentKey.Scale, nextKey.Scale, t);

	return glm::scale(glm::mat4(1.0f), interpolatedScale);
}

int JointClip::GetPositionIndex(float animationTime) const
{
	if (m_PositionKeys.size() == 0)
		__debugbreak();
	for (uint32_t i = 0; i < m_PositionKeys.size() - 1; i++)
	{
		// Assuming of course that key frames are sorted by ascending timestamps
		if (animationTime < m_PositionKeys[i + 1].Timestamp)
			return i;
	}
	return m_PositionKeys.size() - 2;
}

int JointClip::GetRotationIndex(float animationTime) const
{
	for (uint32_t i = 0; i < m_RotationKeys.size() - 1; i++)
	{
		if (animationTime < m_RotationKeys[i + 1].Timestamp)
			return i;
	}
	return m_RotationKeys.size() - 2;
}

int JointClip::GetScaleIndex(float animationTime) const
{
	for (uint32_t i = 0; i < m_ScaleKeys.size() - 1; i++)
	{
		if (animationTime < m_ScaleKeys[i + 1].Timestamp)
			return i;
	}
	return m_ScaleKeys.size() - 2;
}

float JointClip::GetLerpParam(float prevKeyTime, float nextKeyTime, float currentTime)
{
	return (currentTime - prevKeyTime) / (nextKeyTime - prevKeyTime);
}

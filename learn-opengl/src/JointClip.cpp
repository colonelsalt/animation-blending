#include "JointClip.h"

JointClip::JointClip(const std::string& name, const aiNodeAnim* channel, bool shouldFreezeTranslation)
	: m_LocalPose(1.0f), m_Name(name), m_ShouldFreezeTranslation(shouldFreezeTranslation)
{
	uint32_t numPositions = shouldFreezeTranslation ? 1 : channel->mNumPositionKeys;
	m_PositionKeys.reserve(numPositions);
	for (uint32_t i = 0; i < numPositions; i++)
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
	m_Translation = InterpolatePosition(animationTime);
	m_Rotation = InterpolateRotation(animationTime);
	m_Scale = InterpolateScale(animationTime);

	// S: Why this multiplication order?? Would any other do??
	m_LocalPose = glm::translate(glm::mat4(1.0f), m_Translation)
		* glm::toMat4(m_Rotation)
		* glm::scale(glm::mat4(1.0f), m_Scale);
}

glm::vec3 JointClip::InterpolatePosition(float animationTime) const
{
	if (m_PositionKeys.size() == 1)
		return m_PositionKeys[0].Position;

	int currentKeyIndex = GetPositionIndex(animationTime);

	const PositionKeyFrame& currentKey = m_PositionKeys[currentKeyIndex];
	const PositionKeyFrame& nextKey = m_PositionKeys[currentKeyIndex + 1];

	float t = GetLerpParam(currentKey.Timestamp, nextKey.Timestamp, animationTime);
	return glm::mix(currentKey.Position, nextKey.Position, t);
}

glm::quat JointClip::InterpolateRotation(float animationTime) const
{
	if (m_RotationKeys.size() == 1)
	{
		const glm::quat& rotation = m_RotationKeys[0].Rotation;
		return glm::normalize(rotation);
	}

	int currentKeyIndex = GetRotationIndex(animationTime);

	const RotationKeyFrame& currentKey = m_RotationKeys[currentKeyIndex];
	const RotationKeyFrame& nextKey = m_RotationKeys[currentKeyIndex + 1];

	float t = GetLerpParam(currentKey.Timestamp, nextKey.Timestamp, animationTime);
	glm::quat interpolatedRot = glm::slerp(currentKey.Rotation, nextKey.Rotation, t);

	// S: Why does a quaternion have to be normalised??
	return glm::normalize(interpolatedRot);
}

glm::vec3 JointClip::InterpolateScale(float animationTime) const
{
	if (m_ScaleKeys.size() == 1)
		return m_ScaleKeys[0].Scale;

	int currentKeyIndex = GetScaleIndex(animationTime);

	const ScaleKeyFrame& currentKey = m_ScaleKeys[currentKeyIndex];
	const ScaleKeyFrame& nextKey = m_ScaleKeys[currentKeyIndex + 1];

	float t = GetLerpParam(currentKey.Timestamp, nextKey.Timestamp, animationTime);
	return glm::mix(currentKey.Scale, nextKey.Scale, t);
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

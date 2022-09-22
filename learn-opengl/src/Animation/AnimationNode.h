#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>
#include <string>

struct LocalPose
{
	std::string JointName;

	glm::vec3 Translation;
	glm::quat Rotation;
	glm::vec3 Scale;
};

class AnimationNode
{
public:
	virtual void UpdateLocalPoses(float animationTime) = 0;
	virtual const std::unordered_map<std::string, LocalPose>& GetLocalPoses() const = 0;

	virtual float GetTicksPerSecond() const = 0;
	virtual float GetDuration() const = 0;
};
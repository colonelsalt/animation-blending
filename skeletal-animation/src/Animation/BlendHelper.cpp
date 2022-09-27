#include "BlendHelper.h"

#include "../Core.h"
#include <glm/gtc/quaternion.hpp>

namespace BlendHelper
{
	void BlendPoses(std::unordered_map<std::string, LocalPose>& blendedPoses,
					const std::unordered_map<std::string, LocalPose>& sourcePoses,
					const std::unordered_map<std::string, LocalPose>& targetPoses,
					float t)
	{
		S_ASSERT(sourcePoses.size() == targetPoses.size());

		for (auto& [name, sourcePose] : sourcePoses)
		{
			const LocalPose& targetPose = targetPoses.at(name);

			glm::vec3 translation = glm::mix(sourcePose.Translation, targetPose.Translation, t);
			glm::quat rotation = glm::slerp(sourcePose.Rotation, targetPose.Rotation, t);
			glm::vec3 scale = glm::mix(sourcePose.Scale, targetPose.Scale, t);

			blendedPoses[name] = { sourcePose.JointName, translation, rotation, scale };
		}
	}
}
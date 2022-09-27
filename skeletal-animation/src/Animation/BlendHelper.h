#pragma once

#include "AnimationNode.h"

namespace BlendHelper
{
	void BlendPoses(std::unordered_map<std::string, LocalPose>& blendedPoses,
					const std::unordered_map<std::string, LocalPose>& sourcePoses,
					const std::unordered_map<std::string, LocalPose>& targetPoses,
					float t);
}
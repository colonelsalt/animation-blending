#pragma once

#include "Animation.h"

class Animator
{
public:
	Animator(Animation* animation, bool shouldLoop = false);

	void Update(float deltaTime);

	//! Describes each bone's offset from its bind pose
	const std::vector<glm::mat4>& GetBoneTransforms() const { return m_BoneTransformations; }

private:
	void UpdateBoneTransform(const BoneNode& bone, const glm::mat4& parentTransform);
private:
	static constexpr int MAX_TOTAL_BONES = 100;

	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	bool m_ShouldLoop;

	std::vector<glm::mat4> m_BoneTransformations;
};
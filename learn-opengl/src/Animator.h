#pragma once

#include "Animation.h"

class Animator
{
public:
	Animator(Animation* animation, bool shouldLoop = false);

	void Update(float deltaTime);

	//! Describes each joint's offset from its bind pose
	const std::vector<glm::mat4>& GetSkinningMatrices() const { return m_SkinningMatrices; }

	void Pause() { m_IsRunning = false; }
	void Play() { m_IsRunning = true; }

private:
	void UpdateJointTransforms(const SkeletonNode& bone, const glm::mat4& parentTransform);
private:
	static constexpr int MAX_TOTAL_BONES = 100;

	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	bool m_ShouldLoop;

	bool m_IsRunning = false;

	//! Final matrix describes each joint's offset from its bind pose
	std::vector<glm::mat4> m_SkinningMatrices;
};
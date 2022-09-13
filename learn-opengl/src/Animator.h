#pragma once

#include "Animation.h"

class Animator
{
public:
	Animator(bool shouldLoop = false);

	void Update(float deltaTime);

	//! Describes each joint's offset from its bind pose
	const std::vector<glm::mat4>& GetSkinningMatrices() const { return m_SkinningMatrices; }

	void Pause() { m_IsRunning = false; }
	void Play() { m_IsRunning = true; }

	void AddAnimation(Animation& animation, float weight);
	void SetWeight(const std::string& animationName, float weight);
	void SetTicksPerSecond(float tps) { m_TicksPerSecond = tps; }

private:
	void UpdateJointTransforms(const SkeletonNode& bone, const glm::mat4& parentTransform);
private:
	static constexpr int MAX_TOTAL_BONES = 100;

	float m_CurrentTime = 0.0f;
	float m_TicksPerSecond = 1.0f;
	bool m_ShouldLoop;

	std::vector<std::pair<Animation, float>> m_Animations;

	bool m_IsRunning = false;

	//! Final matrix describes each joint's offset from its bind pose
	std::vector<glm::mat4> m_SkinningMatrices;
};
#pragma once

#include "AnimationState.h"
#include "AnimationClip.h"

#include "../Core.h"

class Animator
{
protected:
	Animator();
public:
	~Animator();

	void SetDirectory(const std::shared_ptr<JointDirectory>& jointDirectory) { m_JointDirectory = jointDirectory; }
	void SetState(AnimationState* state) { S_ASSERT(!m_CurrentState); m_CurrentState = state; }

	void Update(float deltaTime);

	void SetTrigger(const std::string& name);
	void SetFloat(const std::string& name, float value);

	void OnStateFinished(const AnimationState* state, Transition* nextTransition);
	void OnTransitionFinished(const Transition* transition);

	bool IsTransitioning() const { return m_CurrentTransition; }

	//! Describes each joint's offset from its bind pose
	const std::vector<glm::mat4>& GetSkinningMatrices() const { return m_SkinningMatrices; }
	
	static Animator* GetInstance() { S_ASSERT(s_Instance); return s_Instance; }
private:
	void UpdateSkinningMatrices(const SkeletonNode& node, const glm::mat4& parentTransform);
	const std::unordered_map<std::string, LocalPose>& GetLocalPoses() const;
private:
	static constexpr int MAX_TOTAL_JOINTS = 100;
	static Animator* s_Instance;

	std::shared_ptr<JointDirectory> m_JointDirectory;

	AnimationState* m_CurrentState;
	Transition* m_CurrentTransition;

	//! Final matrix describes each joint's offset from its bind pose
	std::vector<glm::mat4> m_SkinningMatrices;
};
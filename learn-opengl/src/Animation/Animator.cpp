#include "Animator.h"

#include <glm/gtx/quaternion.hpp>

Animator* Animator::s_Instance = new Animator();

Animator::Animator()
{
	m_SkinningMatrices.reserve(MAX_TOTAL_BONES);
	for (uint32_t i = 0; i < MAX_TOTAL_BONES; i++)
	{
		m_SkinningMatrices.push_back(glm::mat4(1.0f));
	}
}

Animator::~Animator()
{
	if (s_Instance)
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}

void Animator::Update(float deltaTime)
{
	if (m_CurrentTransition)
		m_CurrentTransition->Update(deltaTime);
	if (m_CurrentState)
		m_CurrentState->Update(deltaTime);

	UpdateSkinningMatrices(m_JointDirectory->GetRootNode(), glm::mat4(1.0f));
}

void Animator::SetTrigger(const std::string& name)
{
	if (m_CurrentState)
		m_CurrentState->SetTrigger(name);
}

void Animator::SetFloat(const std::string& name, float value)
{
	if (m_CurrentState)
		m_CurrentState->SetVar<float>(name, value);
}

void Animator::OnStateFinished(const AnimationState* state, Transition* nextTransition)
{
	std::cout << "State " << state->GetName() << " completed" << std::endl;

	S_ASSERT(state == m_CurrentState);
	S_ASSERT(!m_CurrentTransition);

	std::cout << "Transitioning to state " << nextTransition->GetTargetState()->GetName() << std::endl;
	//nextTransition->GetTargetState()->Reset();
	/*if (nextTransition->GetTargetState()->GetName() == "Halting")
		__debugbreak();*/

	m_CurrentState = nullptr;
	m_CurrentTransition = nextTransition;
}

void Animator::OnTransitionFinished(const Transition* transition)
{
	S_ASSERT(transition == m_CurrentTransition);
	S_ASSERT(!m_CurrentState);

	std::cout << "Transitioned to state " << transition->GetTargetState()->GetName() << std::endl;

	m_CurrentTransition = nullptr;
	transition->GetSourceState()->Reset();
	m_CurrentState = transition->GetTargetState();
}

void Animator::UpdateSkinningMatrices(const SkeletonNode& node, const glm::mat4& parentTransform)
{
	// Default value if this node is not animated
	glm::mat4 localTransform = node.Transform;

	const std::unordered_map<std::string, LocalPose>& localPoses = GetLocalPoses();
	if (localPoses.find(node.Name) != localPoses.end())
	{
		// This joint is animated, so use its animated pose instead
		const LocalPose& localPose = localPoses.at(node.Name);

		localTransform = glm::translate(glm::mat4(1.0f), localPose.Translation)
			* glm::toMat4(localPose.Rotation)
			* glm::scale(glm::mat4(1.0f), localPose.Scale);
	}

	glm::mat4 modelSpaceTransform = parentTransform * localTransform;

	// If this is a joint (i.e. is bound to a vertex), update its skinning matrix
	if (m_JointDirectory->ContainsJoint(node.Name))
	{
		const Joint& joint = m_JointDirectory->GetJoint(node.Name);
		m_SkinningMatrices[joint.Id] = modelSpaceTransform * joint.InverseBindPose;
	}

	// Recursively update skinning matrices of node's children
	for (uint32_t i = 0; i < node.Children.size(); i++)
		UpdateSkinningMatrices(node.Children[i], modelSpaceTransform);
}

const std::unordered_map<std::string, LocalPose>& Animator::GetLocalPoses() const
{
	if (m_CurrentTransition)
		return m_CurrentTransition->GetLocalPoses();
	else if (m_CurrentState)
		return m_CurrentState->GetLocalPoses();
	
	S_ASSERT(false); // Animator has neither state nor transition set
	return std::unordered_map<std::string, LocalPose>();
}
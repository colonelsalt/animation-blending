#include "BlendNode.h"

#include "BlendHelper.h"
#include "../Core.h"

BlendNode::BlendNode(AnimationNode* sourceNode, AnimationNode* targetNode)
	: m_SourceNode(sourceNode), m_TargetNode(targetNode)
{
	m_Duration = 1.0f;
	m_SourceTpsScale = m_TargetNode->GetDuration() / m_SourceNode->GetDuration();
}

void BlendNode::SetTargetWeight(float targetWeight)
{
	S_ASSERT(targetWeight >= 0 && targetWeight <= 1);
	m_TargetWeight = targetWeight;

	float finalTps = m_SourceTpsScale * m_SourceNode->GetDuration();
	m_TicksPerSecond = glm::mix(30.0f / m_SourceNode->GetDuration(), 30.0f / m_TargetNode->GetDuration(), targetWeight);
}

void BlendNode::UpdateLocalPoses(float animationTime)
{
	m_SourceNode->UpdateLocalPoses(animationTime);
	m_TargetNode->UpdateLocalPoses(animationTime);

	BlendHelper::BlendPoses(m_LocalPoses, m_SourceNode->GetLocalPoses(), m_TargetNode->GetLocalPoses(), m_TargetWeight);
}
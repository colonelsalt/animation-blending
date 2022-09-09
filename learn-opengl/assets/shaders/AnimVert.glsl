#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_BiTangent;
layout (location = 5) in ivec4 a_BoneIds; // IDs of all bones whose position influence this vertex's position
layout (location = 6) in vec4 a_BoneWeights; // weight of each bone's contribution to moving this vertex

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

const int MAX_TOTAL_BONES = 100;
const int MAX_BONES_PER_VERTEX = 4; // no more than 4 bones can influence the position of a single vertex

// bone's offset from its bind pose, in model space
uniform mat4 u_BoneTransformations[MAX_TOTAL_BONES];

out vec2 v_TexCoords;

void main()
{
	vec4 finalPosition = vec4(0.0);

	// Go through all bones attached to this vertex, and sum up their contribution to the final pos/rot of the vertex
	for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
	{
		if (a_BoneIds[i] == -1)
		{
			continue;
		}
		if (a_BoneIds[i] >= MAX_TOTAL_BONES)
		{
			// Do not consider bones with IDs above 100 (S: how can this happen?)
			finalPosition = vec4(a_Position, 1.0);
			break;
		}

		vec4 bonePosition = u_BoneTransformations[i] * vec4(a_Position, 1.0);
		finalPosition += bonePosition * a_BoneWeights[i];

	}

	gl_Position = u_Projection * u_View * u_Model * finalPosition;
	v_TexCoords = a_TexCoords;
}

#pragma once

#include <glm/glm.hpp>
#include <assimp/matrix4x4.h>

namespace AssimpHelper
{
	static glm::mat4 AssimpToGlmMatrix(const aiMatrix4x4& assimpMatrix)
	{
		glm::mat4 out;

		for (uint32_t i = 0; i < 4; i++)
		{
			for (uint32_t j = 0; j < 4; j++)
			{
				out[i][j] = assimpMatrix[j][i];
			}
		}
		return out;
	}
}
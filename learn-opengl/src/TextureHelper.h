#pragma once

#include <stdint.h>
#include <string>

#include <stb_image.h>

#include <assimp/scene.h>

namespace TextureHelper
{
	uint32_t LoadTexture(const char* fileName, const std::string& directoryPath);

	uint32_t LoadTextureEmbedded(const aiTexture* texture);
}
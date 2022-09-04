#pragma once

#include <stdint.h>
#include <string>

#include <stb_image.h>

namespace TextureHelper
{
	uint32_t LoadTexture(const char* fileName, const std::string& directoryPath);
}
#include "TextureHelper.h"

#include <glad/glad.h>
#include <iostream>

uint32_t TextureHelper::LoadTexture(const char* fileName, const std::string& directoryPath)
{
	std::string path = directoryPath + '/' + fileName;

	stbi_set_flip_vertically_on_load(true);
	int width, height, numChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture." << std::endl;
		exit(1);
	}

	uint32_t textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	GLenum imageFormat;
	if (numChannels == 3)
		imageFormat = GL_RGB;
	else if (numChannels == 4)
		imageFormat = GL_RGBA;
	else
	{
		std::cout << "Unknown image format in texture: " << path << std::endl;
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return textureId;
}

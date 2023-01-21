#include "Texture.h"
#include "IMG_lib.h"
Texture::Texture(const std::string& path)
	:m_rendererID(0),m_filepath(path),m_localbuffer(nullptr),m_width(0),m_height(0),m_bpp(0)
{
	stbi_set_flip_vertically_on_load(1);
	m_localbuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_bpp, 4);
	glGenTextures(1, &m_rendererID);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);

	//textures parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localbuffer);
	glBindTexture(GL_TEXTURE_2D,0);

	if (m_localbuffer)
		stbi_image_free(m_localbuffer);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_rendererID);
}

void Texture::bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0); 
}

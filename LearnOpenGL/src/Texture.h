#pragma once
#include "Renderer.h"
class Texture
{
private:
	unsigned int m_rendererID;
	std::string m_filepath;
	unsigned char* m_localbuffer;
	int m_width, m_height, m_bpp;  //bits per pixel(bpp)

public:
	Texture(const std::string& path);
	~Texture();

	void bind(unsigned int slot = 0) const;
	void unbind() const;

	inline int getwidth() const { return m_width; }
	inline int getheight() const { return m_height; }
};


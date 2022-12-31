#pragma once
#include "vertexbuffer.h"
#include "vertexbufferlayout.h"
class vertexarray
{
private:
	unsigned int m_rendererID;
public:
	vertexarray();
	~vertexarray();

	void addbuffer(vertexbuffer &vb,vertexbufferlayout& layout);

	void bind();
	void unbind();
};
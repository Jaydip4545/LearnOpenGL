#include "vertexbuffer.h"
#include "Renderer.h"
vertexbuffer::vertexbuffer(const void* data, unsigned int size)
{
	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

vertexbuffer::~vertexbuffer()
{
	glDeleteBuffers(1, &m_rendererID);
}

void vertexbuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void vertexbuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

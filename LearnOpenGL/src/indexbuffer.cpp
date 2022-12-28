#include "indexbuffer.h"
#include "Renderer.h"

indexbuffer::indexbuffer(const unsigned int* data, unsigned int count):m_count(count)
{
	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

indexbuffer::~indexbuffer()
{
	glDeleteBuffers(1, &m_rendererID);
}

void indexbuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void indexbuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

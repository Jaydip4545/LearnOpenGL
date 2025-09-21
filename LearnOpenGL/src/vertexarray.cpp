#include "vertexarray.h"
#include "Renderer.h"
vertexarray::vertexarray()
{
	glGenVertexArrays(1, &m_rendererID);//create 1 vertex buffer and store it's id to m_rendererID
}

vertexarray::~vertexarray()
{
	glDeleteVertexArrays(1,&m_rendererID);
}

void vertexarray::addbuffer(vertexbuffer& vb, vertexbufferlayout& layout)
{
	bind();
	vb.bind();
	const auto& elements = layout.getelements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(attribCount);
		glVertexAttribPointer(attribCount, element.count,element.type,GL_FALSE, layout.getstride(), (void*)offset);
		offset += element.count * vertexbufferelement::getsizeoftype(element.type);
		attribCount++;
	}
	
}

void vertexarray::bind()
{
	glBindVertexArray(m_rendererID);
}

void vertexarray::unbind()
{
	glBindVertexArray(0);
}

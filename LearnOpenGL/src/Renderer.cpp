#include "Renderer.h"

void Renderer::draw(vertexarray& va, indexbuffer& ib,shader& sdr)
{
	sdr.bind();
	va.bind();
	ib.bind();
	glDrawElements(GL_TRIANGLES,ib.getcount(), GL_UNSIGNED_INT, nullptr);
	
}
void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

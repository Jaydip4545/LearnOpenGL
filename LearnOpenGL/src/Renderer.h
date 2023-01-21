#pragma once
#include "glew.h"
#include "vertexarray.h"
#include "indexbuffer.h"
#include "shader.h"

class Renderer {
private:
public:
	void draw(vertexarray& va, indexbuffer& ib,shader& sdr);
	void clear();

};


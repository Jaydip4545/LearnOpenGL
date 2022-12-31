#pragma once
#include<vector>
#include "glew.h"
struct vertexbufferelement
{

	unsigned int type;
	unsigned int count;
	bool normalized;

	static unsigned int getsizeoftype(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT:
				return 4;
			case GL_UNSIGNED_INT:
				return 4;
			case GL_UNSIGNED_BYTE:
				return 1;
		}
		return 0;
	}
};

class vertexbufferlayout
{
private:
	std::vector<vertexbufferelement> m_elements;
	unsigned int m_stride;
public:
	vertexbufferlayout():m_stride(0){};
	template<typename T>
	void push(unsigned int count)
	{
		
	}

	template<>
	void push<float>(unsigned int count)
	{
		m_elements.push_back({ GL_FLOAT,count,false });
		m_stride += count * vertexbufferelement::getsizeoftype(GL_FLOAT);
	}

	template<>
	void push<unsigned int>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_INT,count,false });
		m_stride += count * vertexbufferelement::getsizeoftype(GL_UNSIGNED_INT);
	}

	template<>
	void push<unsigned char>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_BYTE,count,true });
		m_stride += count * vertexbufferelement::getsizeoftype(GL_UNSIGNED_BYTE);
	}
	inline const std::vector<vertexbufferelement> getelements() {
		return m_elements;
	}
	inline unsigned int getstride() { return m_stride; }
};
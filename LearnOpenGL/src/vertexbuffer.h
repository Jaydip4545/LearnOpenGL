#pragma once
class vertexbuffer
{
	private:
		unsigned int m_rendererID;
	public:
		vertexbuffer(const void* data, unsigned int size);
		~vertexbuffer();

		void bind();
		void unbind();
};


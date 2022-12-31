#pragma once
#include<string>
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;

};
class shader
{
private:
	unsigned int m_rendererID;
	std::string m_filepath;
	//caching for uniforms
public:
	shader(const std::string filepath);
	~shader();

	void bind();
	void unbind();

	//set uniforms
	void setuniform4f(const std::string name, float v0, float v1, float v2, float v3);
	unsigned int createshader(unsigned int type, const std::string& source);
	unsigned int createshaderprogram(std::string& vertexshader, std::string& fragmentshader);
	ShaderProgramSource ParseShader(const std::string& filepath);
	private:
		unsigned int getuniformlocation(const std::string& name);

};


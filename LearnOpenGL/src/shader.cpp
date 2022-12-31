#include "shader.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include "Renderer.h"
shader::shader(const std::string filepath): m_filepath(filepath),m_rendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_rendererID = createshaderprogram(source.VertexSource, source.FragmentSource);
}

void shader::setuniform4f(const std::string name, float v0, float v1, float v2, float v3)
{
    glUniform4f(getuniformlocation(name), v0, v1, v2, v3);
}

unsigned int shader::createshader(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

unsigned int shader::createshaderprogram(std::string& vertexshader, std::string& fragmentshader)
{
    int program = glCreateProgram();
    unsigned int vs = createshader(GL_VERTEX_SHADER, vertexshader);
    unsigned int fs = createshader(GL_FRAGMENT_SHADER, fragmentshader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;

}

ShaderProgramSource shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class Shadertype
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };


    std::string line;
    std::stringstream ss[2];

    Shadertype type = Shadertype::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = Shadertype::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = Shadertype::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}


shader::~shader()
{
    glDeleteProgram(m_rendererID);
}


void shader::bind()
{
    glUseProgram(m_rendererID);
}

void shader::unbind()
{
    glUseProgram(0);
}



unsigned int shader::getuniformlocation(const std::string& name)
{
    int location = glGetUniformLocation(m_rendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning : Uniform " << name << " doesn't exist!!" << std::endl;
    return location;
}

#include "shader.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include "Renderer.h"

shader::shader(const std::string filepath): m_filepath(filepath),m_rendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_rendererID = createshaderprogram(source.VertexSource, source.FragmentSource, source.GeometrySource);
}

void shader::setuniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(getuniformlocation(name), v0, v1, v2, v3);
}

void shader::setuniform1f(const std::string& name, int value)
{
    glUniform1f(getuniformlocation(name), value);
}

void shader::setuniformMat4f(const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(getuniformlocation(name), 1, GL_FALSE,&value[0][0]);
    
}

void shader::setuniformvec3(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(getuniformlocation(name), 1, &value[0]);
}

void shader::setuniformvec4(const std::string& name, const glm::vec4& value)
{
    glUniform4fv(getuniformlocation(name), 1, &value[0]);
}

void shader::setuniform1i(const std::string& name, int value)
{
    glUniform1i(getuniformlocation(name), value);
}

unsigned int shader::createshader(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

unsigned int shader::createshaderprogram(std::string& vertexshader, std::string& fragmentshader ,std::string& geometryshader)
{
    int program = glCreateProgram();
    unsigned int vs = createshader(GL_VERTEX_SHADER, vertexshader);
    unsigned int fs = createshader(GL_FRAGMENT_SHADER, fragmentshader);
    unsigned int gs = createshader(GL_GEOMETRY_SHADER, geometryshader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glAttachShader(program, gs);
    glLinkProgram(program);
    int success = 0;
    char infoLog[1024];
    // After linking the shader program...
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> infoLog(logLength);
        glGetProgramInfoLog(program, logLength, NULL, infoLog.data());

        std::cerr << "Shader Program Linking Failed:\n"
            << infoLog.data() << std::endl;
    }
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(gs);
    return program;

}

ShaderProgramSource shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class Shadertype
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1 , GEOMETRY = 2
    };


    std::string line;
    std::stringstream ss[3];

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
            else if (line.find("geometry") != std::string::npos)
            {
                type = Shadertype::GEOMETRY;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str(),ss[2].str()};
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
        //std::cout << "Warning : Uniform " << name << " doesn't exist!!" << std::endl;
    return location;
}

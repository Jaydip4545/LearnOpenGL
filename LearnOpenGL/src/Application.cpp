#include "glew.h"
#include<iostream>
#include "glfw3.h"
#include<fstream>
#include<string>
#include<sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	
};
static ShaderProgramSource ParseShader(const std::string& filepath)
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
			if(line.find("vertex")!=std::string::npos)
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
unsigned int createshader(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
	const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}
unsigned int createshaderprogram(std::string& vertexshader, std::string& fragmentshader)
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
void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{  
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
		glfwSetWindowShouldClose(window, true);
	}
}
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
	
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Application", NULL, NULL);
   
    //fix frame rate
   
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    //Positions of triangle points
	float vertices[] = {
     0.5f,  0.5f,  // top right
     0.5f, -0.5f, // bottom right
    -0.5f, -0.5f,  // bottom left
	-0.5f,  0.5f  // top left
	};
    unsigned int indices[] = {
         0, 1, 3,   // first triangle
         1, 2, 3
    };
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();
    glViewport(0, 0, 800, 600);
    glfwSwapInterval(1);
    
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    //vertex buffer object
    unsigned int VBO;
    glGenBuffers(1, &VBO); 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  
    //vertex index array (indexes of vertices)
    unsigned int ibo;
    glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    
    
    
    
    ShaderProgramSource source = ParseShader("Resources/Shaders/basics.shader");
   
    
    unsigned int program = createshaderprogram(source.VertexSource,source.FragmentSource);
    glLinkProgram(program);
    glUseProgram(program);
	int u_loc =glGetUniformLocation(program, "in_color");
    float r = 0.0f;
	float increment = 0.05f;
    glProgramUniform4f(program,u_loc, r, 0.3f, 0.8f, 1.0f);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        if (r > 1.0f)
        {
            increment = -0.05f;
        }
        else if (r < 0.0f)
        {
            increment = 0.05f;
        }
        /* Render here */
        ProcessInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glProgramUniform4f(program, u_loc, r, 0.3f, 0.8f, 1.0f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		r += increment;
        
       
   
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
		
        
		
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

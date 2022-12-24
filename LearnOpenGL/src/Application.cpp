#include "glew.h"
#include<iostream>
#include "glfw3.h"

unsigned int createshader(unsigned int type, const char* &source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}
unsigned int createshaderprogram(const char*& vertexshader, const char*& fragmentshader)
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
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    //Positions of triangle points
	float vertices[6] = {
		-0.5f, -0.5f, // left  
		 0.5f, -0.5f, // right 
		 0.0f,  0.5f  // top   
	};
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();
    glViewport(0, 0, 800, 600);
    unsigned int VBO;
    glGenBuffers(1, &VBO); 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    //vertex shader code
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec4 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = aPos;\n"
        "}\0";
    
	//fragment shader code
    
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";

    unsigned int program = createshaderprogram(vertexShaderSource, fragmentShaderSource);
    glLinkProgram(program);
    glUseProgram(program);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        ProcessInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        
		glDrawArrays(GL_TRIANGLES, 0, 3);
        
		
       
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
		
        
		
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
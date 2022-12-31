#include "glew.h"
#include "glfw3.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include "indexbuffer.h"
#include "vertexbuffer.h"
#include "vertexarray.h"
#include "shader.h"

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{  
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
    -0.5f, -0.5f, // bottom left
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
    
    
    

    vertexarray va;
    vertexbuffer VBO(vertices,8*sizeof(float));
    vertexbufferlayout layout;
    layout.push<float>(2);
    va.addbuffer(VBO, layout);

    //vertex index array (indexes of vertices)
    indexbuffer ibo(indices, 6);

    //shader constructure takes const reference to a string as input
    shader first_shader("Resources/Shaders/basics.shader");

    first_shader.bind();
    first_shader.setuniform4f("in_color", 1.0f, 0.0f, 0.0f, 1.0f);
    va.unbind();
    VBO.unbind();
    ibo.unbind();
    first_shader.unbind();
    float r = 0.0f;
	float increment = 0.05f;
    
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
       
        first_shader.bind();
        first_shader.setuniform4f("in_color", r, 0.0f, 0.4f, 1.0f);
        va.bind();
        ibo.bind();
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

#include "glew.h"
#include<iostream>
#include "glfw3.h"


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
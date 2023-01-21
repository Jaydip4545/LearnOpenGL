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
#include "Renderer.h"
#include "Texture.h"
#include <cmath>
#include "particle.h"
#include "algorithm"
#include "physics.h"
float r = 0.7971;

void update(particle* p, int n, float& DeltaTime)
{
    //Earth_Gravity(p, n, DeltaTime);
    collision(p, n, DeltaTime);
   
}

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
    window = glfwCreateWindow(1200, 1200, "Application", NULL, NULL);
   
    //fix frame rate
   
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    //Positions of triangle points
    int number_of_objects = 40;
    particle prtcls[3000];
    float c_x = -0.84;
    float c_y = 0.84;
    for (int i = 0; i < number_of_objects; i++)
    {
        float v_x = (rand() - RAND_MAX / 2.0) / (500*RAND_MAX);
        float v_y = (rand() - RAND_MAX / 2.0) / (500*RAND_MAX);
        float r = (rand() - RAND_MAX / 2.0) / (5*RAND_MAX);

        prtcls[i].set_center(c_x, c_y);
        prtcls[i].set_valocity_x(v_x);
        prtcls[i].set_valocity_y(v_y);
        prtcls[i].set_radius(abs(0.05));
        prtcls[i].set_mass(abs(r * 10));
        if (c_x + 0.15 >= 0.84)
        {
            c_x = -0.96;
            c_y -= 0.15;
        }
        else
        {
            c_x += 0.25;
        }
       
       
        //std::cout << r * 10 << std::endl;
    }
    float* circle_points_p[3000];
	float vertices[] = {
     -0.5f,  -0.5f,0.0f,  0.0f,  // top right
     0.5f, -0.5f,1.0f, 0.0f, // bottom right
     0.5f, 0.5f,1.0f, 1.0f, // bottom left
    -0.5f,  0.5f,0.0f, 1.0f // top left
	};
    unsigned int indices[] = {
         0, 1, 2,   // first triangle
         2, 3, 0
    };
    
    //
    float points[10000];
   
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();
    glViewport(0, 0, 1200, 1200);
    //glfwSwapInterval(1);
    
    
    //blending

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    vertexarray va;
    vertexbuffer VBO(vertices,16*sizeof(float));
    vertexbufferlayout layout;
    layout.push<float>(2);
    layout.push<float>(2);
    //layout.push<float>(1);
    va.addbuffer(VBO, layout);
   
    //vertex index array (indexes of vertices)
    indexbuffer ibo(indices, 6);

    //texture
    
    Texture texture("Resources/Textures/image.png");
    texture.bind(); 
    //shader constructure takes const reference to a string as input
    shader first_shader("Resources/Shaders/basics.shader");

    first_shader.bind();
    first_shader.setuniform4f("in_color", 1.0f, 0.0f, 0.0f, 1.0f);

    //texture
    first_shader.setuniform1i("u_Texture", 0);

    va.unbind();
    VBO.unbind();
    ibo.unbind();
    first_shader.unbind();
    float r = 0.0f;
	float increment = 0.05f;
    Renderer renderer;

    float theta = 0;
    
    //
    float *final_points=new float[number_of_objects *72];
    for (int i = 0; i < number_of_objects; i++)
    {
        circle_points_p[i] = prtcls[i].draw();
        for (int j = 0; j < 72; j++)
        { 
            final_points[i * 72 + j] = circle_points_p[i][j];
        }
    }
    vertexarray vaa;
    vertexbuffer vb(final_points, 72* number_of_objects * sizeof(float));
    vertexbufferlayout l;
    l.push<float>(2);
    vaa.addbuffer(vb, l);
    float m = 0.01;
    shader Draw_points("Resources/Shaders/Draw_points.shader");
    Draw_points.bind();
  
    //Draw_points.setuniform4f("in_color", 1.0f, 0.0f, 0.0f, 1.0f);
    /* Loop until the user closes the window */
    double previousTime = glfwGetTime();
    int frameCount = 0;
    float frames_per_sec = 60;
    float DeltaTime = frames_per_sec/2000;

    while (!glfwWindowShouldClose(window))
    {
      
        double currentTime = glfwGetTime();
        frameCount++;
       
        // If a second has passed.
        if (currentTime - previousTime >= 1.0)
        {
            // Display the frame count here any way you want.
            std::cout << frameCount << std::endl;
            DeltaTime = frames_per_sec / frameCount;
            frameCount = 0;
            previousTime = currentTime;
        }
       
        for (int i = 0; i < number_of_objects; i++)
        {
            circle_points_p[i] = prtcls[i].draw();
            for (int j = 0; j < 72; j++)
            {
                final_points[i * 72 + j] = circle_points_p[i][j];
            }
        }
        
       
       vertexbuffer vb(final_points, 72* number_of_objects * sizeof(float));
       vaa.addbuffer(vb, l);
      
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
        renderer.clear();
        //first_shader.setuniform4f("in_color", r, 0.0f, 0.4f, 1.0f);
        //renderer.draw(va, ibo, first_shader);
       
        m += 0.0001;
       // std::cout << m << std::endl;
        glDrawArrays(GL_POINTS, 0, 72 * number_of_objects);
		r += increment;
        
      
      
      
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
		
        
		
        /* Poll for and process events */
        glfwPollEvents();
       
        update(prtcls, number_of_objects,DeltaTime);
        theta += 0.00001;
    }

    glfwTerminate();
    return 0;
}

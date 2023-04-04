#include "glew.h"//functions for modern opengl , always include glew or glad or any other
//libraries that contains opengl before glfw because glfw requires opengl to be included first
#include "glfw3.h" //used to create window
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include "indexbuffer.h" // index buffer class
#include "vertexbuffer.h" // vertex buffer class
#include "vertexarray.h" //use to define vertex attributes
#include "shader.h" //shader class to compile and bind different shaders
#include "Renderer.h"
#include "Texture.h" 
#include <cmath>
#include "algorithm"
#include "glm/glm.hpp" // maths library specific for opengl maths
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"
int screen_wigth  = 2500;
int screen_height = 1400;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);

//camera 
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screen_wigth / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;



int main(void)
{
    GLFWwindow* window;
   
    /* Initialize the library */
    if (!glfwInit())
        return -1;
	
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screen_wigth, screen_height, "Application", NULL, NULL);
    glfwSetWindowPos(window, 50, 50);//glfwwindow will be rendered on the give coordinates 
    //fix frame rate
   
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    //Positions of triangle points
  
    float cube_coordinates[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    glm::vec3 lightPos(1.2f, 4.5f, 4.0f);
    glm::vec3 objectPos(1.0f, 1.3f, 1.2f);
   
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();
    glViewport(0, 0, screen_wigth, screen_height);
    //glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //blending

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, lightPos);
   // model = glm::scale(model, glm::vec3(0.2));
    //glm::mat4 view;
    view = camera.GetViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)screen_wigth / (float)screen_height, 0.1f, 100.0f);
   
   
   
    //layout.push<float>(1);
    
    //vertex index array (indexes of vertices)
    vertexbuffer vbo(cube_coordinates, 8 * 36 * sizeof(float));
    vertexarray va_object;
    vertexbufferlayout object_layout;
    object_layout.push<float>(3);
    object_layout.push<float>(3);
    object_layout.push<float>(2);
    va_object.addbuffer(vbo, object_layout);

   
    

    //texture
    
      Texture texture("Resources/Textures/brick_wall.png");
      Texture texture2("Resources/Textures/brick_wall_outline.png");
      texture.bind(); 
     //shader constructure takes const reference to a string as input
  
   
    //texture
      
    
    
    

   //vao_cubes.unbind();
   //VBO.unbind();
   //ibo.unbind();
   //object_shader.unbind();
    Renderer renderer;

  
    
    //Draw_points.setuniform4f("in_color", 1.0f, 0.0f, 0.0f, 1.0f);
    /* Loop until the user closes the window */
    double previousTime = glfwGetTime();
    int frameCount = 0;
    float frames_per_sec = 60;
    float DeltaTime = frames_per_sec/2000;
   
   
    //light and object colors
    glm::vec3 lightcolor(1.0, 1.0, 1.0);
    glm::vec3  objectcolor(1.0f, 0.5f, 0.31f);
    //multiple cubes 
    shader light_shader("Resources/Shaders/light.shader");
    shader object_shader("Resources/Shaders/basics.shader");

        object_shader.setuniform1i("material.diffuse", 0);
        //texture.unbind();
        texture2.bind(1);
        object_shader.setuniform1i("material.specular", 1);
        light_shader.bind();
        //bind shader first then only you can set uniforms for the shaders 
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model,glm::vec3(0.5,0.5,0.5));
        light_shader.setuniformMat4f("u_model", model);
        light_shader.setuniformMat4f("u_view", view);
        light_shader.setuniformMat4f("u_projection", projection);
       
        light_shader.unbind();

        model = glm::mat4(1.0f);
        model = glm::translate(model, objectPos);

        object_shader.bind();
        object_shader.setuniformMat4f("u_model", model);
        object_shader.setuniformMat4f("u_view", view);
        object_shader.setuniformMat4f("u_projection", projection);
        object_shader.setuniformvec3("objectColor",objectcolor);
        object_shader.setuniformvec3("lightColor",lightcolor);
        object_shader.setuniformvec3("lightpos", lightPos);
        object_shader.setuniformvec3("viewPos", camera.Position);
        object_shader.setuniformvec3("material.ambient", glm::vec3(0.19f,0.19f,0.19f));
        object_shader.setuniformvec3("material.diffuse", glm::vec3(0.50754f,0.50754f,0.50754f));
        object_shader.setuniformvec3("material.specular", glm::vec3(0.508273f, 0.508273f, 0.508273f));
        object_shader.setuniformvec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        object_shader.setuniformvec3("light.diffuse", glm::vec3(0.9f, 0.9f, 0.9f)); // darken diffuse light a bit
        object_shader.setuniformvec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        object_shader.setuniform1f("material.shininess",52.0f);
        object_shader.unbind();
        
   
    while (!glfwWindowShouldClose(window))
    {
        

        
        renderer.clear();//clears depth and color buffers

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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
       // vbo.bind();
        //va_object.addbuffer(vbo, object_layout);

        view = camera.GetViewMatrix();
        
       

        va_object.bind();
        light_shader.bind();
        light_shader.setuniformMat4f("u_view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
       // model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
        light_shader.setuniformMat4f("u_model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        light_shader.unbind();
        lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        va_object.bind();
        object_shader.bind();
        object_shader.setuniformMat4f("u_view", view);
        object_shader.setuniformvec3("lightColor", lightcolor);
        object_shader.setuniformvec3("viewPos", camera.Position);
        object_shader.setuniformvec3("lightpos", lightPos);
       //object_shader.setuniformvec3("light.ambient", ambientColor);
       //object_shader.setuniformvec3("light.diffuse", diffuseColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        object_shader.unbind();
        /* Render here */
       
        
        ProcessInput(window);
        //renderer.clear();
        //renderer.draw(va, ibo, first_shader);
       

        // camera/view transformation
       
      
		
     
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
		
        
		
        /* Poll for and process events */
        glfwPollEvents();
       
    }

    glfwTerminate();
    return 0;
}
void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
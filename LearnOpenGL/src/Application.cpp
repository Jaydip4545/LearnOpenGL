#include "glew.h"
#include "glfw3.h" //used to create window
#include<iostream>
#include<fstream>
#include<string>
#include"Mesh.h"
#include"MeshSlicer.h"
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

int screen_wigth = 2500;
int screen_height = 1400;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


//camera 
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screen_wigth / 2.0f;
float lastY = screen_height / 2.0f;
int firstMouse = true;

//state Variables
bool CPUSlicer = true;
int rotateConst = 10;
bool phongEnabled = 0;
int numPlanes = 0;



int main(void)
{

	//Setup

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(screen_wigth, screen_height, "Application", NULL, NULL);
	glfwSetWindowPos(window, 50, 50);//glfwwindow will be rendered on the give coordinates 
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
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



	//delta time 

	double previousTime = glfwGetTime();
	int frameCount = 0;
	float frames_per_sec = 120;
	float DeltaTime = frames_per_sec / 2000;

	Mesh mesh;
	mesh.loadOFF("Resources/Models/1GRM_1.off");
	glm::vec3 objectPos(0.0f, 0.0f, -40.0f);
	glm::vec3  objectcolor(1.0f, 0.5f, 1.0f);
	glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(45.0f), (float)screen_wigth / (float)screen_height, 0.1f, 100.0f);
	vertexbuffer mesh_vbo(mesh.getVertices(), 3 * mesh.getNumVertices() * sizeof(float));
	vertexarray mesh_vao;
	vertexbufferlayout object_layout;
	object_layout.push<float>(3);
	vertexbuffer mesh_vboNormals(mesh.getNormals(), 3 * mesh.getNumVertices() * sizeof(float));
	indexbuffer mesh_ibo(mesh.getIndices(), mesh.getNumIndices());
	mesh_vao.addbuffer(mesh_vbo, object_layout);
	mesh_vao.addbuffer(mesh_vboNormals, object_layout);
	Renderer renderer;
	shader slicer_shader("Resources/Shaders/Slicer.shader");
	shader basic_shader("Resources/Shaders/basic.shader");
	model = glm::mat4(1.0f);
	model = glm::translate(model, objectPos);
	float scl = 1.0;
	model = glm::scale(model, glm::vec3(scl, scl, scl));
	model = glm::mat4(1.0f);
	model = glm::translate(model, objectPos);
	glm::vec4 u_plane = glm::vec4(1.0f, 0.0f, 0.0f, -0.2f);
	float u_sliceOffset = 1.0f;

	slicer_shader.bind();
	slicer_shader.setuniformMat4f("u_model", model);
	slicer_shader.setuniformvec4("u_plane", u_plane);
	slicer_shader.setuniform1f("u_sliceOffset", u_sliceOffset);
	slicer_shader.setuniformMat4f("u_view", view);
	slicer_shader.setuniformMat4f("u_projection", projection);
	slicer_shader.setuniformvec3("u_lightPos", glm::vec3(10.0f, 80.0f, 5.00f));
	slicer_shader.setuniformvec3("u_lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	slicer_shader.setuniformvec3("viewPos", camera.Position);
	slicer_shader.setuniformvec3("u_viewPos", camera.Position);
	slicer_shader.unbind();
	float theta = 55.0;
	slicer_shader.setuniform1f("minDepth", mesh.getMinBoundingBox()[2]);
	slicer_shader.setuniform1f("maxDepth", mesh.getMaxBoundingBox()[2]);

	basic_shader.bind();
	basic_shader.setuniformMat4f("u_model", model);
	basic_shader.setuniformMat4f("u_view", view);
	basic_shader.setuniformMat4f("u_projection", projection);
	basic_shader.setuniformvec3("viewPos", camera.Position);
	basic_shader.setuniformvec3("u_lightPos", glm::vec3(10.0f, 80.0f, 5.00f));
	basic_shader.setuniformvec3("u_lightColor", glm::vec3(1.0f, 1.0f, 1.0f));


	basic_shader.setuniform1f("minDepth", mesh.getMinBoundingBox()[2]);
	basic_shader.setuniform1f("maxDepth", mesh.getMaxBoundingBox()[2]);
	basic_shader.unbind();


	GLuint slicedPartVAO;
	GLuint slicedPartVBO;
	GLuint slicedPartNormalsVBO;
	GLuint slicedPartIBO;


	glGenVertexArrays(1, &slicedPartVAO);
	glGenBuffers(1, &slicedPartVBO);
	glGenBuffers(1, &slicedPartNormalsVBO);
	glGenBuffers(1, &slicedPartIBO);


	glBindVertexArray(slicedPartVAO);


	glBindBuffer(GL_ARRAY_BUFFER, slicedPartVBO);
	glBufferData(GL_ARRAY_BUFFER, 10 * 1024 * 1024, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, slicedPartNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, 10 * 1024 * 1024, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, slicedPartIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10 * 1024 * 1024, nullptr, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);

	glfwSetKeyCallback(window, key_callback);

	std::string fpsText = "";
	
	while (!glfwWindowShouldClose(window))
	{



		renderer.clear();

		//Frame Rate
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		double currentTime = glfwGetTime();
		frameCount++;
		if (currentTime - previousTime >= 1.0)
		{
			fpsText = "FPS: " + std::to_string(frameCount);
			glfwSetWindowTitle(window, fpsText.c_str());
			DeltaTime = frames_per_sec / frameCount;
			frameCount = 0;
			previousTime = currentTime;
		}


		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		model = glm::translate(model, objectPos);
		model = glm::rotate(model, glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scl, scl, scl));
		theta += rotateConst * deltaTime;

		if (CPUSlicer)
		{
			
			CuttingPlane planes[10];
			planes[0] = CuttingPlane(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -40.0f);
			planes[1] = CuttingPlane(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -40.0f);
			planes[2] = CuttingPlane(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -40.0f);
			planes[3] = CuttingPlane(1.0f, 0.0f, 0.0f, 0.5f, 0.0f, -40.0f);
			planes[4] = CuttingPlane(0.707f, 0.707f, 0.0f, 0.0f, 0.0f, -40.0f);
			planes[5] = CuttingPlane(0.0f, 0.707f, 0.707f, 0.0f, 0.0f, -40.0f);
			planes[6] = CuttingPlane(0.577f, 0.577f, 0.577f, 0.0f, 0.0f, -40.0f);
			planes[7] = CuttingPlane(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
			planes[8] = CuttingPlane(0.0f, 1.0f, 0.0f, 0.0f, 100.0f, -40.0f);
			planes[9] = CuttingPlane(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -39.5f);

			Transform worldTransform;
			for (int col = 0; col < 4; col++) {
				for (int row = 0; row < 4; row++) {
					worldTransform.matrix[col * 4 + row] = model[col][row];
				}
			}
			// Slice the mesh
			MeshSlicer slicer;
			MeshPartArray* parts = slicer.sliceMesh(mesh, worldTransform, planes, numPlanes, 2.0f);


			basic_shader.bind();
			basic_shader.setuniformMat4f("u_view", view);
			basic_shader.setuniformvec3("u_cameraPos", camera.Position);
			basic_shader.setuniform1i("phongEnabled", phongEnabled);

			// Render each part using the dynamic buffers
			for (int i = 0; i < parts->getCount(); i++) {
				Mesh* part = parts->getMesh(i);
				
				if (part && part->getNumVertices() > 0) {
					
					float minX = mesh.getMinBoundingBox()[0];
					float minY = mesh.getMinBoundingBox()[1];
					float minZ = mesh.getMinBoundingBox()[2];
					float maxX = mesh.getMaxBoundingBox()[0];
					float maxY = mesh.getMaxBoundingBox()[1];
					float maxZ = mesh.getMaxBoundingBox()[2];
					std::vector<glm::vec3> corners = {
						{minX, minY, minZ}, {maxX, minY, minZ},
						{minX, maxY, minZ}, {maxX, maxY, minZ},
						{minX, minY, maxZ}, {maxX, minY, maxZ},
						{minX, maxY, maxZ}, {maxX, maxY, maxZ}
					};

					float minViewDepth = FLT_MAX;
					float maxViewDepth = 0.0f;

					for (const auto& corner : corners) {
						// Transform corner to world space
						glm::vec4 worldPos = model * glm::vec4(corner, 1.0f);

						// Calculate distance from camera
						float distance = glm::distance(camera.Position, glm::vec3(worldPos));

						minViewDepth = std::min(minViewDepth, distance);
						maxViewDepth = std::max(maxViewDepth, distance);
					}
					basic_shader.setuniform1f("minDepth", minViewDepth);
					basic_shader.setuniform1f("maxDepth", maxViewDepth);

					// 1. Update the Vertex Buffer with new position data
					glBindBuffer(GL_ARRAY_BUFFER, slicedPartVBO);
					glBufferSubData(GL_ARRAY_BUFFER, 0, part->getNumVertices() * 3 * sizeof(float), part->getVertices());

					// 2. Update the Normal Buffer with new normal data
					glBindBuffer(GL_ARRAY_BUFFER, slicedPartNormalsVBO);
					glBufferSubData(GL_ARRAY_BUFFER, 0, part->getNumVertices() * 3 * sizeof(float), part->getNormals());

					// 3. Update the Index Buffer with new index data
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, slicedPartIBO);
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, part->getNumIndices() * sizeof(unsigned int), part->getIndices());

					// 4. Bind the VAO that knows how to read our buffers
					glBindVertexArray(slicedPartVAO);

					// 5. Draw the part!
					glDrawElements(GL_TRIANGLES, part->getNumIndices(), GL_UNSIGNED_INT, 0);
				}
			}


			glBindVertexArray(0);
			basic_shader.unbind();

			delete parts;





		}
		else {
			mesh_vao.bind();

			slicer_shader.bind();
			slicer_shader.setuniformMat4f("u_view", view);
			slicer_shader.setuniformMat4f("u_model", model);
			slicer_shader.setuniformvec3("u_viewPos", camera.Position);
			slicer_shader.setuniform1i("phongEnabled", phongEnabled);
			slicer_shader.setuniform1i("PLANE_COUNT",numPlanes);
			// Transform bounding box corners to view space
			float minX = mesh.getMinBoundingBox()[0];
			float minY = mesh.getMinBoundingBox()[1];
			float minZ = mesh.getMinBoundingBox()[2];
			float maxX = mesh.getMaxBoundingBox()[0];
			float maxY = mesh.getMaxBoundingBox()[1];
			float maxZ = mesh.getMaxBoundingBox()[2];

			std::vector<glm::vec3> corners = {
				{minX, minY, minZ}, {maxX, minY, minZ},
				{minX, maxY, minZ}, {maxX, maxY, minZ},
				{minX, minY, maxZ}, {maxX, minY, maxZ},
				{minX, maxY, maxZ}, {maxX, maxY, maxZ}
			};

			float minViewDepth = FLT_MAX;
			float maxViewDepth = 0.0f;

			for (const auto& corner : corners) {
				glm::vec4 worldPos = model * glm::vec4(corner, 1.0f);
				float distance = glm::distance(camera.Position, glm::vec3(worldPos));

				minViewDepth = std::min(minViewDepth, distance);
				maxViewDepth = std::max(maxViewDepth, distance);
			}


			slicer_shader.setuniform1f("minDepth", minViewDepth);
			slicer_shader.setuniform1f("maxDepth", maxViewDepth);
			slicer_shader.setuniformvec3("u_cameraPos", camera.Position);
			
			
			mesh_ibo.bind();
			glDrawElements(GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, 0);
			mesh_vao.unbind();
			mesh_ibo.unbind();
			slicer_shader.unbind();
		}



		ProcessInput(window);

		glfwSwapBuffers(window);

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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// We only care about the 'B' key and only when it is first pressed.
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		
			CPUSlicer = !CPUSlicer;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		phongEnabled = !phongEnabled;
		
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		numPlanes++;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		numPlanes--;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		rotateConst+=10;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		rotateConst-=10;
	}
	
}


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <algorithm>    

#include "camera.h"
#include "Sphere.h"
#include "Cube.h"

#ifndef WHOMST
#define WHOMST
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

# define M_PI           3.14159265358979323846

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

// camera data
glm::vec3 cameraPos = glm::vec3(1.0f, 1.0f, 30.0f);
float cameraDistance, cameraAngle = 0.0f, acceleration = 1.0f;
Camera camera(cameraPos);

// used for rendering
glm::mat4 projection, view;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
GLfloat angle = 45.0f;

// time between frames
float deltaTime = 0.0f, lastFrame = 0.0f;

// sun data
glm::vec3 lightPos = glm::vec3(0.0f, 1.0f, 0.0f), lightColor = glm::vec3(0.949f, 0.960f, 0.760f);

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		acceleration = 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		acceleration = std::min(acceleration * 1.007f, 3.0f);
		cameraAngle -= fmod(0.5f * deltaTime * acceleration, 2 * M_PI);
		
		if (cameraAngle < 0.0f)
			cameraAngle = 2 * M_PI;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		acceleration = std::min(acceleration * 1.007f, 3.0f);
		cameraAngle += fmod(0.5f * deltaTime * acceleration, 2 * M_PI);

		if (cameraAngle > 2 * M_PI)
			cameraAngle = 0.0f;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Planet Destroyer", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces
	{
		"./right.jpg",
		"./left.jpg",
		"./top.jpg",
		"./bottom.jpg",
		"./front.jpg",
		"./back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	Shader shader("planet.vert", "planet.frag");
	Shader lightShader("planet.vert", "sun.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	Cube c(shader, glm::vec3(1.0f, 1.0f, 3.0f), glm::vec3(1.0f), glm::vec3(0.47f, 0.13f, 0.15f));
	Cube d(shader, glm::vec3(3.0f, 1.0f, 5.0f), glm::vec3(2.0f), glm::vec3(0.12f, 0.36f, 0.47f));
	Cube e(shader, glm::vec3(6.5f, 1.0f, 6.5f), glm::vec3(1.4f), glm::vec3(0.90f, 0.5f, 0.0f));
	Cube sun(lightShader, lightPos, glm::vec3(1.0f), lightColor);

	cameraAngle = atan(cameraPos.x / cameraPos.y);

	while (!glfwWindowShouldClose(window))
	{
		cameraDistance = glm::distance(camera.Position, sun.position);

		float camX = sin(cameraAngle) * cameraDistance;
		float camZ = cos(cameraAngle) * cameraDistance;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		view = glm::lookAt(glm::vec3(camX, 1.0, camZ), sun.position, glm::vec3(0.0, 1.0, 0.0));

		// draw the sun
		angle = fmod(angle + 0.01, 2* M_PI);

		
		sun.draw(projection, view, angle,false,1,1);

		// draw the planets
		shader.use();
		shader.SetVector3f("lightColor", sun.color);
		shader.SetVector3f("lightPos", sun.position);
		shader.SetVector3f("viewpos", camera.Position);
		c.draw(projection, view, fmod(angle + 0.06, 2 * M_PI), true, 1, 1);
		d.draw(projection, view, fmod(angle + 0.1, 2 * M_PI), true, 2, -1);
		e.draw(projection, view, fmod(angle + 0.06, 2 * M_PI), true, 2.5, 1);

		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.SetMatrix4("view", view);
		skyboxShader.SetMatrix4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}


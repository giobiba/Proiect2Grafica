#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <algorithm>    

#include "camera.h"
#include "Sphere.h"
#include "Cube.h"

# define M_PI           3.14159265358979323846

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

// camera data
glm::vec3 cameraPos = glm::vec3(1.0f, 1.0f, 10.0f);
float cameraDistance, cameraAngle = 0.0f, acceleration = 1.0f;
Camera camera(cameraPos);

// used for rendering
glm::mat4 projection, view;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	Shader shader("planet.vert", "planet.frag");
	Shader lightShader("planet.vert", "sun.frag");
	Cube c(shader, glm::vec3(1.0f, 1.0f, 3.0f), glm::vec3(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
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
		sun.draw(projection, view);

		// draw the planets

		shader.use();
		shader.SetVector3f("lightColor", sun.color);
		shader.SetVector3f("lightPos", sun.position);
		shader.SetVector3f("viewpos", camera.Position);
		c.draw(projection, view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}


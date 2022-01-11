#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm/glm.hpp"  
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include "Shader.h"

int const NR_PARR = 10, NR_MERID = 20;

class Sphere 
{
private:
	float radius;
	int nr_parr, nr_merid;

	GLuint VAO, VBO, EBO;

	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> Colors;
	std::vector<GLushort> Indices;

	Shader shader;
	glm::vec3 position;

public:
	Sphere(Shader shader, glm::vec3 position, float radius, int nr_parr = NR_PARR, int nr_merid = NR_MERID);
	~Sphere();

	void move(glm::vec3 direction, float time_delta);
	void CreateVBO();
	void draw();
};
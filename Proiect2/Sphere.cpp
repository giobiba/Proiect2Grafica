#include "Sphere.h"

float PI = 3.141592f;
float const U_MIN = -PI / 2, U_MAX = PI / 2, V_MIN = 0, V_MAX = 2 * PI;


void Sphere::CreateVBO()
{
	float step_u = (U_MAX - U_MIN) / nr_parr, step_v = (V_MAX - V_MIN) / nr_merid;

	for (int merid = 0; merid < nr_merid; merid++)
	{
		for (int parr = 0; parr < nr_parr + 1; parr++)
		{
			
			float u = U_MIN + parr * step_u;
			float v = V_MIN + merid * step_v;

			float x_vf = radius * cosf(u) * cosf(v); 
			float y_vf = radius * cosf(u) * sinf(v);
			float z_vf = radius * sinf(u);

			int index = merid * (nr_parr + 1) + parr;

			Vertices[index] = glm::vec3(x_vf, y_vf, z_vf);
			Colors[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Indices[index] = index;
			


			int index_aux = parr * (nr_merid)+merid;
			Indices[(nr_parr + 1) * nr_merid + index_aux] = index;
			if ((parr + 1) % (nr_parr + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (nr_parr + 1) * nr_merid;
				int index1 = index; // varful v considerat
				int index2 = index + (nr_parr + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == nr_merid - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (nr_parr + 1);
					index3 = index3 % (nr_parr + 1);
				}
				Indices[AUX + 4 * index] = index1; 
				Indices[AUX + 4 * index + 1] = index2;
				Indices[AUX + 4 * index + 2] = index3;
				Indices[AUX + 4 * index + 3] = index4;
			}
		}
	}


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), &Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), &Colors);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Sphere::draw()
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	this->shader.SetMatrix4("model", model);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	for (int patr = 0; patr < (this->nr_parr + 1) * this->nr_merid; patr++)
	{
		if ((patr + 1) % (this->nr_parr + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, (GLvoid*)((2 * (this->nr_parr + 1) * (this->nr_merid)+4 * patr) * sizeof(GLushort)));
	}
}

Sphere::Sphere(Shader shader, glm::vec3 position, float radius, int nr_parr, int nr_merid): shader(shader), position(position), radius(radius), nr_parr(nr_parr), nr_merid(nr_merid) {
	Vertices.resize((nr_parr + 1) * nr_merid);
	Colors.resize((nr_parr + 1) * nr_merid);
	Indices.resize(2 * (nr_parr + 1) * nr_merid + 4 * (nr_parr + 1) * nr_merid);
	CreateVBO();
}

Sphere::~Sphere() {
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}
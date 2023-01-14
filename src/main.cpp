#include <stdio.h>
#include <cmath>
#include <string>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "window.hpp"
#include "mesh.hpp"
#include "shader.hpp"

std::vector<mesh*> mesh_list;
std::vector<shader*> shader_list;

static const char* vertex_shader_file = "src/shaders/shader.vert";
static const char* fragment_shader_file = "src/shaders/shader.frag";

void create_objects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, // 0
		0.0f, -1.0f, 1.0f, // 1
		1.0f, -1.0f, 0.0f, // 2
		0.0f, 1.0f, 0.0f // 3
	};

	mesh_list.push_back(new mesh(vertices, indices, 12, 12));
	mesh_list.push_back(new mesh(vertices, indices, 12, 12));
}

void create_shaders()
{
	shader_list.push_back(make_shader_from_file(vertex_shader_file,fragment_shader_file));
}

int main()
{
	window* main_window = new window(800, 600);
	main_window->initialize();

	create_objects();
	create_shaders();

	GLuint uniform_projection = 0;
	GLuint uniform_model = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)main_window->get_buffer_width()/(GLfloat)main_window->get_buffer_height(), 0.1f, 100.0f);

	// Loop until window closed
	while (!main_window->get_should_close())
	{
		// Get + Handle user input events
		glfwPollEvents();

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader_list[0]->use();
		uniform_model = shader_list[0]->get_model_location();
		uniform_projection = shader_list[0]->get_projection_location();

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		// model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(projection));	
		mesh_list[0]->render();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		// model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		mesh_list[1]->render();

		glUseProgram(0);

		main_window->swap_buffers();
	}

	return 0;
}
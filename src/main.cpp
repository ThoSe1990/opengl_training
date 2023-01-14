#define STB_IMAGE_IMPLEMENTATION

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
#include "camera.hpp"
#include "texture.hpp"
#include "light.hpp"

const float to_radians = 3.14159265f / 180.0f;

std::vector<mesh*> mesh_list;
std::vector<shader*> shader_list;

GLfloat delta_time = 0.0f;
GLfloat last_time = 0.0f;


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
	// introducing u and s for textures
	//    x       y     z    u     s
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f, 0.5f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  0.5f, 1.0f
	};

	mesh_list.push_back(new mesh(vertices, indices, 20, 12));
	mesh_list.push_back(new mesh(vertices, indices, 20, 12));
}

void create_shaders()
{
	shader_list.push_back(make_shader_from_file(vertex_shader_file,fragment_shader_file));
}

int main()
{
	window main_window(800, 600);
	main_window.initialize();

	camera c(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 5.0f, 0.1f);

	texture brick("textures/brick.png");
	brick.load();
	texture dirt("textures/dirt.png");
	dirt.load();

	light main_light(0.0f, 1.0f, 0.0f, 0.9f);

	create_objects();
	create_shaders();

	GLuint uniform_projection = 0;
	GLuint uniform_model = 0;
	GLuint uniform_view = 0;
	GLuint uniform_ambient_color = 0;
	GLuint uniform_ambient_intensity = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)main_window.get_buffer_width()/(GLfloat)main_window.get_buffer_height(), 0.1f, 100.0f);

	// Loop until window closed
	while (!main_window.get_should_close())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerfomanceCounter();
		delta_time = now - last_time; // (now - last_time)*1000 / SDL_GetPerformanceFrequency();
		last_time = now;

		// Get + Handle user input events
		glfwPollEvents();

		c.key_control(main_window.get_keys(), delta_time);
		c.mouse_control(main_window.get_x_change(), main_window.get_y_change());

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader_list[0]->use();
		uniform_model = shader_list[0]->get_model_location();
		uniform_projection = shader_list[0]->get_projection_location();
		uniform_view = shader_list[0]->get_view_location();
		uniform_ambient_color = shader_list[0]->get_ambient_color_location();
		uniform_ambient_intensity = shader_list[0]->get_ambient_intensity_location();

		main_light.use_light(uniform_ambient_intensity, uniform_ambient_color);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.5f));
		model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(projection));	
		glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(c.calculate_view_matrix()));	
		brick.use();
		mesh_list[0]->render();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		dirt.use();
		mesh_list[1]->render();

		glUseProgram(0);

		main_window.swap_buffers();
	}

	return 0;
}
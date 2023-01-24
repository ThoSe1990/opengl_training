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

static constexpr std::size_t MAX_POINT_LIGHTS = 3;
static constexpr std::size_t MAX_SPOT_LIGHTS = 3;

#include "window.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "directional_light.hpp"
#include "point_light.hpp"
#include "spot_light.hpp"
#include "material.hpp"

#include "assimp/Importer.hpp"

#include "model.hpp"

const float to_radians = 3.14159265f / 180.0f;

std::vector<mesh*> mesh_list;
std::vector<shader*> shader_list;

GLfloat delta_time = 0.0f;
GLfloat last_time = 0.0f;


static const char* vertex_shader_file = "src/shaders/shader.vert";
static const char* fragment_shader_file = "src/shaders/shader.frag";

void calculate_average_normals(
 unsigned int* indices,
 unsigned int indice_count,
 GLfloat* vertices,
 unsigned int vertice_count,
 unsigned int vertice_length,
 unsigned int normal_offset)
{
	for (std::size_t i = 0 ; i < indice_count ; i += 3)
	{
		unsigned int in0 = indices[i] * vertice_length;
		unsigned int in1 = indices[i+1] * vertice_length;
		unsigned int in2 = indices[i+2] * vertice_length;
		glm::vec3 v1(
			vertices[in1] - vertices[in0], 
			vertices[in1+1] - vertices[in0+1],
			vertices[in1+2] - vertices[in0+2]
		);
		glm::vec3 v2(
			vertices[in2] - vertices[in0], 
			vertices[in2+1] - vertices[in0+1],
			vertices[in2+2] - vertices[in0+2]
		);
		glm::vec3 normal(glm::cross(v1, v2));
		normal = glm::normalize(normal);

		in0 += normal_offset; in1 += normal_offset; in2 += normal_offset;
		vertices[in0] += normal.x; vertices[in0+1] += normal.y; vertices[in0+2] += normal.z;
		vertices[in1] += normal.x; vertices[in1+1] += normal.y; vertices[in1+2] += normal.z;
		vertices[in2] += normal.x; vertices[in2+1] += normal.y; vertices[in2+2] += normal.z;
	}

	for (std::size_t i = 0 ; i < vertice_count/vertice_length ; i++)
	{
		unsigned int n_offset = i * vertice_length + normal_offset;
		glm::vec3 vec(vertices[n_offset], vertices[n_offset+1], vertices[n_offset+2]);
		vec = glm::normalize(vec);
		vertices[n_offset] = vec.x; vertices[n_offset+1] = vec.y; vertices[n_offset+2] = vec.z;

	}
}



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
	//    x       y     z       u     v       nx    ny    nz
		-1.0f, -1.0f, -0.6f,    0.0f, 0.0f,    0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f,    0.5f, 0.0f,    0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f,    1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,   0.5f, 1.0f,    0.0f, 0.0f, 0.0f
	};

	calculate_average_normals(indices, 12, vertices, 32, 8, 5);

	mesh_list.push_back(new mesh(vertices, indices, 32, 12));
	mesh_list.push_back(new mesh(vertices, indices, 32, 12));


	unsigned int floor_indices[] = {
		0, 2, 1,
		1, 2, 3
	};
	GLfloat floor_vertices[] = {
		-10.0f, 0.0f, -10.f, 0.0f,    0.0f, 0.0f,    -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f, 10.0f,   0.0f, 0.0f,    -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f, 0.0f,   10.0f, 0.0f,    -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f, 10.0f,   10.0f, 0.0f,    -1.0f, 0.0f
	};
	mesh_list.push_back(new mesh(floor_vertices, floor_indices, 32, 6));
}

void create_shaders()
{
	shader_list.push_back(make_shader_from_file(vertex_shader_file,fragment_shader_file));
}

int main()
{
	window main_window(1366, 768);
	main_window.initialize();

	camera c(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.2f);

	material shiny_material(4.0f, 256);
	material dull_material(0.3f, 4);

	model xwing("models/xwing/x-wing.obj");

	texture brick("textures/brick.png");
	brick.load_with_alpha();
	texture dirt("textures/dirt.png");
	dirt.load_with_alpha();
	texture plain("textures/plain.png");
	plain.load_with_alpha();

	directional_light main_light(
		1.0f, 1.0f, 1.0f, 
		0.0f, 0.0f, 
		0.0f, 0.0f, -1.0f
	);
	
	int point_light_count = 0;
	point_light point_lights[MAX_POINT_LIGHTS];

	point_lights[point_light_count++] = point_light(
		0.0f, 0.0f, 1.0f, 
		0.0f, 0.1f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f
	);
	point_lights[point_light_count++] = point_light(
		1.0f, 0.0f, 0.0f, 
		0.0f, 0.1f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f
	);

	int spot_light_count = 0;
	spot_light spot_lights[MAX_SPOT_LIGHTS];
	spot_lights[spot_light_count++] = spot_light(
		0.0f, 1.0f, 1.0f, 
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.3f, 0.2f, 0.1f,
		20.0f
	);
	spot_lights[spot_light_count++] = spot_light(
		1.0f, 1.0f, 1.0f, 
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.3f, 0.2f, 0.1f,
		20.0f
	);


	create_objects();
	create_shaders();

	GLuint uniform_projection = 0;
	GLuint uniform_model = 0;
	GLuint uniform_view = 0;
	
	GLuint uinform_eye_position = 0;
	GLuint uniform_specular_intensity = 0;
	GLuint uniform_shininess = 0; 

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
		
		uinform_eye_position = shader_list[0]->get_eye_position_location();
		uniform_specular_intensity = shader_list[0]->get_specular_intensity_location();
		uniform_shininess = shader_list[0]->get_shininess_location(); 

		glm::vec3 a_bit_lower = c.get_camera_position();
		a_bit_lower.y -= 0.3f;
		spot_lights[0].set_flash(a_bit_lower, c.get_camera_direction());

		shader_list[0]->set_directional_light(&main_light);
		shader_list[0]->set_point_lights(point_lights, point_light_count);
		shader_list[0]->set_spot_lights(spot_lights, spot_light_count);
		// main_light.use_light(uniform_ambient_intensity, uniform_ambient_color, uniform_diffuse_intensity, uniform_diffuse_direction);



		glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(projection));	
		glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(c.calculate_view_matrix()));	
		glUniform3f(uinform_eye_position, c.get_camera_position().x, c.get_camera_position().y, c.get_camera_position().z);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.5f));
		model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		// model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		brick.use();
		shiny_material.use(uniform_specular_intensity, uniform_shininess);
		mesh_list[0]->render();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
		model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		// model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		dirt.use();
		dull_material.use(uniform_specular_intensity, uniform_shininess);
		mesh_list[1]->render();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		// model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		dirt.use();
		shiny_material.use(uniform_specular_intensity, uniform_shininess);
		mesh_list[2]->render();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
		model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		shiny_material.use(uniform_specular_intensity, uniform_shininess);
		xwing.render();

		glUseProgram(0);

		main_window.swap_buffers();
	}

	return 0;
}
#include <stdio.h>
#include <cmath>
#include <string>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float to_radians = 3.14159265f / 180.0f;

GLuint VAO;
GLuint VBO;
GLuint IBO;
GLuint shader;
GLuint uniform_model;
GLuint uniform_projection;

bool direction = true;
float triangle_offset = 0.0f;
float triangle_max_offset = 0.7f;
float triangle_increment = 0.025f;

float current_angle = 0.0f;
float current_scale = 0.0f;

static const char* vertex_shader = R"*(
#version 330

layout (location = 0) in vec3 pos;

out vec4 vertex_color;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	gl_Position = projection * model * vec4(pos, 1.0);
	vertex_color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}
)*";

static const char* fragment_shader = R"*(
#version 330

in vec4 vertex_color;

out vec4 color;

void main()
{
	color = vertex_color;
}
)*";


void create_triangle()
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

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void add_shader(GLuint program, const char* shader_code, GLenum type) 
{
	GLuint current_shader = glCreateShader(type);

	const GLchar* code[1];
	code[0] = shader_code;

	GLint code_length[1];
	code_length[0] = strlen(shader_code);

	glShaderSource(current_shader, 1, code, code_length);
	glCompileShader(current_shader);

	GLint result = 0;
	GLchar log[1024] = {0};

	glGetShaderiv(current_shader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(current_shader, sizeof(log), NULL, log);
		printf("Error compiling &d  shader! %s\n", type, log);
		return;
	}

	glAttachShader(program, current_shader);
}

void compile_shaders()
{
	shader = glCreateProgram();
	if(!shader) {
		printf("Error creating shader program!\n"); 
		exit(1);
	}

	add_shader(shader, vertex_shader, GL_VERTEX_SHADER);
	add_shader(shader, fragment_shader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar log[1024] = {0};

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(log), NULL, log);
		printf("Error linking program! %s\n", log);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(log), NULL, log);
		printf("Error validating program! %s\n", log);
		return;
	}

	uniform_model = glGetUniformLocation(shader, "model");
	uniform_projection = glGetUniformLocation(shader, "projection");
}


int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW initialisation failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile = No Backwards Compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow Forward Compatbility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the window
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get Buffer Size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}


	glEnable(GL_DEPTH_TEST);

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	create_triangle();
	compile_shaders();

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth/(GLfloat)bufferHeight, 0.1f, 100.0f);

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();

		if (direction) {
			triangle_offset += triangle_increment;
			current_scale += 0.1f;
		} else {
			triangle_offset -= triangle_increment;
			current_scale -= 0.1f;
		}

		if (abs(triangle_offset) >= triangle_max_offset){
			direction = !direction;
		}

		current_angle += 1.0f;
		if (current_angle > 360) {
			current_angle -= 360;
		}

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::rotate(model, current_angle*to_radians, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));



		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}
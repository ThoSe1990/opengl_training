#include <stdio.h>
#include <cmath>
#include <string>

#include <GL\glew.h>
#include <GLFW\glfw3.h>


// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO;
GLuint VBO;
GLuint shader;

GLuint uniform_x_move;
bool direction = true;
float triangle_offset = 0.0f;
float triangle_max_offset = 0.7f;
float triangle_increment = 0.005f;

static const char* vertex_shader = R"*(
#version 330

layout (location = 0) in vec3 pos;

uniform float x_move;

void main()
{
	gl_Position = vec4(0.4*pos.x + x_move, 0.4*pos.y, 0.5*pos.z, 1.0);
}
)*";

static const char* fragment_shader = R"*(
#version 330

out vec4 color;

void main()
{
	color = vec4(0.0, 1.0, 0.0, 1.0);
}
)*";


void create_triangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, // 1. vertex x, y, z
		1.0f, -1.0f, 0.0f, // 2. vertex ...
		0.0f, 1.0f, 0.0f // etc... 
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	uniform_x_move = glGetUniformLocation(shader, "x_move");
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

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	create_triangle();
	compile_shaders();

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();

		if (direction) {
			triangle_offset += triangle_increment;
		} else {
			triangle_offset -= triangle_increment;
		}

		if (abs(triangle_offset) >= triangle_max_offset){
			direction = !direction;
		}

		// Clear window
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		glUseProgram(shader);
		
		glUniform1f(uniform_x_move, triangle_offset);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindVertexArray(0);
		
		glUseProgram(0);

		
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}
#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec4 vertex_color;
out vec2 tex_coordinate;
out vec3 normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0);
	vertex_color = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);

	tex_coordinate = tex;
	normal = mat3(transpose(inverse(model))) * norm;
}
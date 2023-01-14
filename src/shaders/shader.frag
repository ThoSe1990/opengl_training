#version 330

in vec4 vertex_color;
in vec2 tex_coordinate;

out vec4 color;

uniform sampler2D this_texture;

void main()
{
	color = texture(this_texture, tex_coordinate);
}
#version 330

in vec4 vertex_color;
in vec2 tex_coordinate;

out vec4 color;

struct directional_light 
{
	vec3 color;
	float intensity;
};

uniform sampler2D this_texture;
uniform directional_light direct_light;

void main()
{
	vec4 ambient_color = vec4(direct_light.color, 1.0f) * direct_light.intensity; 

	color = texture(this_texture, tex_coordinate) * ambient_color;
}
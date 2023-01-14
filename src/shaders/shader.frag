#version 330

in vec4 vertex_color;
in vec2 tex_coordinate;
in vec3 normal;

out vec4 color;

struct directional_light 
{
	vec3 color;
	float intensity;
	vec3 direction;
	float diffuse_intensity;
};

uniform sampler2D this_texture;
uniform directional_light direct_light;

void main()
{
	vec4 ambient_color = vec4(direct_light.color, 1.0f) * direct_light.intensity; 

	float diffuse_factor = max(dot(normalize(normal), normalize(direct_light.direction)), 0.0f);
	vec4 diffuse_color = vec4(direct_light.color, 1.0f) * direct_light.diffuse_intensity * diffuse_factor;

	color = texture(this_texture, tex_coordinate) * (ambient_color + diffuse_color);
}
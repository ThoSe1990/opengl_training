#version 330

in vec4 vertex_color;
in vec2 tex_coordinate;
in vec3 normal;
in vec3 frag_pos;

out vec4 color;

struct directional_light 
{
	vec3 color;
	float intensity;
	vec3 direction;
	float diffuse_intensity;
};

struct material
{
	float specular_intensity;
	float shininess;
};

uniform sampler2D this_texture;
uniform directional_light direct_light;
uniform material m;

uniform vec3 eye_position;

void main()
{
	vec4 ambient_color = vec4(direct_light.color, 1.0f) * direct_light.intensity; 

	float diffuse_factor = max(dot(normalize(normal), normalize(direct_light.direction)), 0.0f);
	vec4 diffuse_color = vec4(direct_light.color, 1.0f) * direct_light.diffuse_intensity * diffuse_factor;

	vec4 specular_color = vec4(0,0,0,0);
	if(diffuse_factor > 0.0f)
	{
		vec3 frag_to_eye = normalize(eye_position - frag_pos);
		vec3 reflected_vertex = normalize(reflect(direct_light.direction, normalize(normal)));
		float specular_factor = dot(frag_to_eye, reflected_vertex);
		if (specular_factor > 0.0f)
		{
			specular_factor = pow(specular_factor, m.shininess);
			specular_color = vec4(direct_light.color * m.specular_intensity * specular_factor, 1.0f);
		}
	}

	color = texture(this_texture, tex_coordinate) * (ambient_color + diffuse_color + specular_color);
}
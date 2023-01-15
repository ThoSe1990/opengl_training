#version 330

in vec4 vertex_color;
in vec2 tex_coordinate;
in vec3 normal;
in vec3 frag_pos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;

struct light
{
	vec3 color;
	float ambient_intensity;
	float diffuse_intensity;
};

struct directional_light 
{
	light base;
	vec3 direction;
};

struct point_light 
{
	light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct material
{
	float specular_intensity;
	float shininess;
};

uniform int point_light_count;

uniform directional_light direct_light;
uniform point_light point_lights[MAX_POINT_LIGHTS];

uniform sampler2D this_texture;
uniform material m;

uniform vec3 eye_position;


vec4 calculate_light_by_direction(light base_light, vec3 direction)
{
	vec4 ambient_color = vec4(base_light.color, 1.0f) * base_light.ambient_intensity; 

	float diffuse_factor = max(dot(normalize(normal), normalize(direction)), 0.0f);
	vec4 diffuse_color = vec4(base_light.color, 1.0f) * base_light.diffuse_intensity * diffuse_factor;

	vec4 specular_color = vec4(0,0,0,0);
	if(diffuse_factor > 0.0f)
	{
		vec3 frag_to_eye = normalize(eye_position - frag_pos);
		vec3 reflected_vertex = normalize(reflect(direction, normalize(normal)));
		float specular_factor = dot(frag_to_eye, reflected_vertex);
		if (specular_factor > 0.0f)
		{
			specular_factor = pow(specular_factor, m.shininess);
			specular_color = vec4(base_light.color * m.specular_intensity * specular_factor, 1.0f);
		}
	}

	return (ambient_color + diffuse_color + specular_color);
}


vec4 calculate_directional_light()
{
	return calculate_light_by_direction(direct_light.base, direct_light.direction);
}

vec4 calculate_point_lights()
{
	vec4 total_color = vec4(0,0,0,0);

	for (int i = 0 ; i < point_light_count ; i++) 
	{
		vec3 direction = frag_pos - point_lights[i].position;
		float distance = length(direction);
		direction = normalize(direction);

		vec4 color = calculate_light_by_direction(point_lights[i].base, direction);
		float attenuation = point_lights[i].exponent * distance*distance + 
							point_lights[i].linear * distance + 
							point_lights[i].constant;
		
		total_color += (color/attenuation);
	}

	return total_color;
}


void main()
{
	vec4 final_color = calculate_directional_light();
	final_color += calculate_point_lights();

	color = texture(this_texture, tex_coordinate) * final_color;
}
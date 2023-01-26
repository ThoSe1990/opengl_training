#version 330

in vec4 vertex_color;
in vec2 tex_coordinate;
in vec3 normal;
in vec3 frag_pos;
in vec4 directional_light_space_pos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

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

struct spot_light
{
	point_light base;
	vec3 direction;
	float edge;
};

struct material
{
	float specular_intensity;
	float shininess;
};

uniform int point_light_count;
uniform int spot_light_count;

uniform directional_light direct_light;
uniform point_light point_lights[MAX_POINT_LIGHTS];
uniform spot_light spot_lights[MAX_SPOT_LIGHTS];

uniform sampler2D this_texture;
uniform sampler2D directional_shadow_map;

uniform material m;

uniform vec3 eye_position;


float calculate_directional_shadow_factor(directional_light light)
{
	vec3 proj_coords = directional_light_space_pos.xyz / directional_light_space_pos.w;
	proj_coords = (proj_coords * 0.5) + 0.5;
	
	float current = proj_coords.z;

	vec3 n = normalize(normal);
	vec3 light_dir = normalize(light.direction);

	float bias = max(0.05 * (1 - dot(n, light_dir)), 0.005);

	float shadow = 0.0;

	vec2 texel_size = 1.0 / textureSize(directional_shadow_map, 0);
	for (int x = -1 ; x <= 1 ; ++x){
		for (int y = -1 ; y <= 1 ; ++y){
			float pcf_depth = texture(directional_shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
			shadow += current-bias > pcf_depth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0; // both for loops is -1 0 1 -> 3 itereations 3^2 = 9
	if (proj_coords.z > 1.0){
		shadow = 0.0;
	}

	return shadow;
}

vec4 calculate_light_by_direction(light base_light, vec3 direction, float shadow_factor)
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

	return (ambient_color + (1.0-shadow_factor) * (diffuse_color + specular_color));
}

vec4 calculate_single_point_light(point_light p_light)
{
	vec3 direction = frag_pos - p_light.position;
	float distance = length(direction);
	direction = normalize(direction);

	vec4 color = calculate_light_by_direction(p_light.base, direction, 0.0);
	float attenuation = p_light.exponent * distance*distance + 
						p_light.linear * distance + 
						p_light.constant;
	
	return (color/attenuation);
}

vec4 calculate_directional_light()
{
	float shadow_factor = calculate_directional_shadow_factor(direct_light);
	return calculate_light_by_direction(direct_light.base, direct_light.direction, shadow_factor);
}

vec4 calculate_single_spot_light(spot_light s_light)
{
	vec3 ray_direction = normalize(frag_pos - s_light.base.position);
	float spot_light_factor = dot(ray_direction, s_light.direction);

	if (spot_light_factor > s_light.edge)
	{
		vec4 color = calculate_single_point_light(s_light.base);
		return color * (1.0f - (1.0f-spot_light_factor)*(1.0f/(1.0f - s_light.edge)));
	} 
	else
	{
		return vec4(0,0,0,0);
	}
}
vec4 calculate_spot_lights()
{
	vec4 total_color = vec4(0,0,0,0);
	for (int i = 0 ; i < spot_light_count ; i++) 
	{	
		total_color += calculate_single_spot_light(spot_lights[i]);
	}

	return total_color;
}

vec4 calculate_point_lights()
{
	vec4 total_color = vec4(0,0,0,0);

	for (int i = 0 ; i < point_light_count ; i++) 
	{	
		total_color += calculate_single_point_light(point_lights[i]);
	}

	return total_color;
}


void main()
{
	vec4 final_color = calculate_directional_light();
	final_color += calculate_point_lights();
	final_color += calculate_spot_lights();
	color = texture(this_texture, tex_coordinate) * final_color;
}
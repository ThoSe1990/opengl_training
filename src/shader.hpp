#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "directional_light.hpp"
#include "point_light.hpp"
#include "spot_light.hpp"

class shader 
{
public:
    shader() = default;
    ~shader()
    {
        clear();
    }

    GLuint get_projection_location() const { return m_projection; }
    GLuint get_model_location() const { return m_model; }
    GLuint get_view_location() const { return m_view; }
    GLuint get_ambient_intensity_location() const { return m_uniform_directional_light.uniform_ambient_intensity; }
    GLuint get_ambient_color_location() const { return m_uniform_directional_light.uniform_color; }
    GLuint get_diffuse_intensity_location() const { return m_uniform_directional_light.uniform_diffuse_intensity; }
    GLuint get_diffuse_direction_location() const { return m_uniform_directional_light.uniform_direction; }
    GLuint get_specular_intensity_location() const { return m_uniform_specular_intensity; }
    GLuint get_shininess_location() const { return m_uniform_shininess; }
    GLuint get_eye_position_location() const { return m_uniform_eye_position; }
    GLuint get_omni_light_pos_location() const { return m_uniform_omni_light_pos; }
    GLuint get_far_plane_location() const { return m_uniform_far_plane; }

    void set_omni_light_matrices(std::vector<glm::mat4> light_matrices)
    {   
        // 6 sides of a cube
        for (std::size_t i = 0 ; i < 6 ; i++) {
            glUniformMatrix4fv(m_uniform_light_matrices[i], 1, GL_FALSE, glm::value_ptr(light_matrices[i]));
        }
    }
    void set_directional_light(directional_light* dl)
    {
        dl->use_light(
            m_uniform_directional_light.uniform_ambient_intensity,
            m_uniform_directional_light.uniform_color, 
            m_uniform_directional_light.uniform_diffuse_intensity,
            m_uniform_directional_light.uniform_direction
        );
    }
    void set_point_lights(point_light* point_lights, unsigned int light_count)
    {
        if (light_count > MAX_POINT_LIGHTS) { 
            light_count = MAX_POINT_LIGHTS; 
        }

        glUniform1i(m_uniform_point_light_count, light_count);
        
        for (std::size_t i = 0 ; i < light_count ; i++){
            point_lights[i].use_light(
                m_uniform_point_light[i].uniform_ambient_intensity, 
                m_uniform_point_light[i].uniform_color,
                m_uniform_point_light[i].uniform_diffuse_intensity,
                m_uniform_point_light[i].uniform_position,
                m_uniform_point_light[i].uniform_constant,
                m_uniform_point_light[i].uniform_linear,
                m_uniform_point_light[i].uniform_exponent
            );
        }
    }
    void set_spot_lights(spot_light* spot_lights, unsigned int light_count)
    {
        if (light_count > MAX_SPOT_LIGHTS) { 
            light_count = MAX_SPOT_LIGHTS; 
        }

        glUniform1i(m_uniform_spot_light_count, light_count);
        
        for (std::size_t i = 0 ; i < light_count ; i++){
            spot_lights[i].use_light(
                m_uniform_spot_light[i].uniform_ambient_intensity, 
                m_uniform_spot_light[i].uniform_color,
                m_uniform_spot_light[i].uniform_diffuse_intensity,
                m_uniform_spot_light[i].uniform_position,
                m_uniform_spot_light[i].uniform_direction,
                m_uniform_spot_light[i].uniform_constant,
                m_uniform_spot_light[i].uniform_linear,
                m_uniform_spot_light[i].uniform_exponent,
                m_uniform_spot_light[i].uniform_edge
            );
        }
    }

    void set_texture(GLuint texture_unit)
    {
        glUniform1i(m_uniform_texture, texture_unit);
    }
    void set_directional_shadow_map(GLuint texture_unit)
    {
        glUniform1i(m_uniform_directional_shadow_map, texture_unit);
    }
    void set_directional_light_transform(glm::mat4* light_transform)
    {
        glUniformMatrix4fv(m_uniform_directional_light_transform, 1, GL_FALSE, glm::value_ptr(*light_transform));
    }

    void use()
    {
        glUseProgram(m_id);
    }

    void clear()
    {
        if (m_id != 0) {
            glDeleteProgram(m_id);
            m_id = 0;
        }
        m_model = 0;
        m_projection = 0;
    }

    void compile(const char* vertex_code, const char* fragment_code)
    {
        m_id = glCreateProgram();
        if(!m_id) {
            printf("Error creating shader program!\n"); 
            exit(1);
        }

        add(m_id, vertex_code, GL_VERTEX_SHADER);
        add(m_id, fragment_code, GL_FRAGMENT_SHADER);

        compile_program();
    }

    void compile(const char* vertex_code, const char* fragment_code, const char* geometry_code)
    {
        m_id = glCreateProgram();
        if(!m_id) {
            printf("Error creating shader program!\n"); 
            exit(1);
        }

        add(m_id, vertex_code, GL_VERTEX_SHADER);
        add(m_id, geometry_code, GL_GEOMETRY_SHADER);
        add(m_id, fragment_code, GL_FRAGMENT_SHADER);
    }

private:
    void compile_program()
    {
        GLint result = 0;
        GLchar log[1024] = {0};

        glLinkProgram(m_id);
        glGetProgramiv(m_id, GL_LINK_STATUS, &result);
        if (!result) {
            glGetProgramInfoLog(m_id, sizeof(log), NULL, log);
            printf("Error linking program! %s\n", log);
            return;
        }

        glValidateProgram(m_id);
        glGetProgramiv(m_id, GL_VALIDATE_STATUS, &result);
        if (!result) {
            glGetProgramInfoLog(m_id, sizeof(log), NULL, log);
            printf("Error validating program! %s\n", log);
            return;
        }

        m_model = glGetUniformLocation(m_id, "model");
        m_projection = glGetUniformLocation(m_id, "projection");
        m_view = glGetUniformLocation(m_id, "view");
        m_uniform_directional_light.uniform_color = glGetUniformLocation(m_id, "direct_light.base.color");
        m_uniform_directional_light.uniform_ambient_intensity = glGetUniformLocation(m_id, "direct_light.base.ambient_intensity");
        m_uniform_directional_light.uniform_direction = glGetUniformLocation(m_id, "direct_light.direction");
        m_uniform_directional_light.uniform_diffuse_intensity = glGetUniformLocation(m_id, "direct_light.base.diffuse_intensity");
        m_uniform_specular_intensity = glGetUniformLocation(m_id, "m.specular_intensity");
        m_uniform_shininess = glGetUniformLocation(m_id, "m.shininess");
        m_uniform_eye_position = glGetUniformLocation(m_id, "eye_position");
        
        m_uniform_point_light_count = glGetUniformLocation(m_id, "point_light_count");

        for (int i = 0 ; i < MAX_POINT_LIGHTS ; i++){
            char location_buffer[100] = {'\0'};

            snprintf(location_buffer, sizeof(location_buffer), "point_lights[%i].base.color", i);
            m_uniform_point_light[i].uniform_color = glGetUniformLocation(m_id, location_buffer);
            
            snprintf(location_buffer, sizeof(location_buffer), "point_lights[%i].base.ambient_intensity", i);
            m_uniform_point_light[i].uniform_ambient_intensity = glGetUniformLocation(m_id, location_buffer);

            snprintf(location_buffer, sizeof(location_buffer), "point_lights[%i].base.diffuse_intensity", i);
            m_uniform_point_light[i].uniform_diffuse_intensity = glGetUniformLocation(m_id, location_buffer);

            snprintf(location_buffer, sizeof(location_buffer), "point_lights[%i].position", i);
            m_uniform_point_light[i].uniform_position = glGetUniformLocation(m_id, location_buffer);

            snprintf(location_buffer, sizeof(location_buffer), "point_lights[%i].constant", i);
            m_uniform_point_light[i].uniform_constant = glGetUniformLocation(m_id, location_buffer);
            snprintf(location_buffer, sizeof(location_buffer), "point_lights[%i].linear", i);
            m_uniform_point_light[i].uniform_linear = glGetUniformLocation(m_id, location_buffer);
            snprintf(location_buffer, sizeof(location_buffer), "point_lights[%i].exponent", i);
            m_uniform_point_light[i].uniform_exponent = glGetUniformLocation(m_id, location_buffer);

        }

        
        m_uniform_spot_light_count = glGetUniformLocation(m_id, "spot_light_count");

        for (int i = 0 ; i < MAX_SPOT_LIGHTS ; i++){
            char location_buffer[100] = {'\0'};

            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].base.base.color", i);
            m_uniform_spot_light[i].uniform_color = glGetUniformLocation(m_id, location_buffer);
            
            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].base.base.ambient_intensity", i);
            m_uniform_spot_light[i].uniform_ambient_intensity = glGetUniformLocation(m_id, location_buffer);

            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].base.base.diffuse_intensity", i);
            m_uniform_spot_light[i].uniform_diffuse_intensity = glGetUniformLocation(m_id, location_buffer);

            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].base.position", i);
            m_uniform_spot_light[i].uniform_position = glGetUniformLocation(m_id, location_buffer);

            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].base.constant", i);
            m_uniform_spot_light[i].uniform_constant = glGetUniformLocation(m_id, location_buffer);
            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].base.linear", i);
            m_uniform_spot_light[i].uniform_linear = glGetUniformLocation(m_id, location_buffer);
            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].base.exponent", i);
            m_uniform_spot_light[i].uniform_exponent = glGetUniformLocation(m_id, location_buffer);

            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].direction", i);
            m_uniform_spot_light[i].uniform_direction = glGetUniformLocation(m_id, location_buffer);
            snprintf(location_buffer, sizeof(location_buffer), "spot_lights[%i].edge", i);
            m_uniform_spot_light[i].uniform_edge = glGetUniformLocation(m_id, location_buffer);
        }

        m_uniform_texture = glGetUniformLocation(m_id, "this_texture");
        m_uniform_directional_light_transform = glGetUniformLocation(m_id, "directional_light_transform");
        m_uniform_directional_shadow_map = glGetUniformLocation(m_id, "directional_shadow_map");

        m_uniform_omni_light_pos = glGetUniformLocation(m_id, "light_pos");
        m_uniform_far_plane = glGetUniformLocation(m_id, "far_plane");

        // array size is 6 --> cube 6 sides .. 
        for (int i = 0 ; i < 6 ; i++) {
            char location_buffer[100] = {'\0'};
            snprintf(location_buffer, sizeof(location_buffer), "light_matrices[%i]", i);
            m_uniform_light_matrices[i] = glGetUniformLocation(m_id, location_buffer);
        }
    }

    void add(GLuint program, const char* shader_code, GLenum type)
    {
        GLuint current_shader = glCreateShader(type);

        const GLchar* code[1];
        code[0] = shader_code;

        GLint code_length[1];
        code_length[0] = strlen(shader_code);

        glShaderSource(current_shader, 1, code, code_length);
        glCompileShader(current_shader);

        int result = 0;
        char log[1024] = {0};

        glGetShaderiv(current_shader, GL_COMPILE_STATUS, &result);
        if (!result) {
            glGetShaderInfoLog(current_shader, sizeof(log), NULL, log);
            printf("Error compiling %d  shader! %s\n", (int)type, log);
            return;
        }

        glAttachShader(program, current_shader);
    }

private:

    struct {
        GLuint uniform_color{0};
        GLuint uniform_ambient_intensity{0};
        GLuint uniform_diffuse_intensity{0};
        
        GLuint uniform_direction{0};
    } m_uniform_directional_light;


    int m_point_light_count{0};
    struct {
        GLuint uniform_color{0};
        GLuint uniform_ambient_intensity{0};
        GLuint uniform_diffuse_intensity{0};

        GLuint uniform_position{0};
        GLuint uniform_constant{0};
        GLuint uniform_linear{0};
        GLuint uniform_exponent{0};
    } m_uniform_point_light[MAX_POINT_LIGHTS];
    GLuint m_uniform_point_light_count{0};


    int m_spot_light_count{0};
    struct {
        GLuint uniform_color{0};
        GLuint uniform_ambient_intensity{0};
        GLuint uniform_diffuse_intensity{0};

        GLuint uniform_position{0};
        GLuint uniform_constant{0};
        GLuint uniform_linear{0};
        GLuint uniform_exponent{0};

        GLuint uniform_direction{0};
        GLuint uniform_edge{0};
    } m_uniform_spot_light[MAX_SPOT_LIGHTS];
    GLuint m_uniform_spot_light_count{0};


    GLuint m_id{0};
    GLuint m_projection{0};
    GLuint m_model{0};
    GLuint m_view{0};
    
    GLuint m_uniform_eye_position{0};
    GLuint m_uniform_specular_intensity{0};
    GLuint m_uniform_shininess{0};

    GLuint m_uniform_texture{0};
    GLuint m_uniform_directional_light_transform{0};
    GLuint m_uniform_directional_shadow_map{0};

    GLuint m_uniform_omni_light_pos{0};
    GLuint m_uniform_far_plane{0};

    GLuint m_uniform_light_matrices[6];
};


std::string read_file(const char* location)
{
    std::string content; 
    std::ifstream filestream(location, std::ios::in);

    if (!filestream.is_open()){
        printf("Error reading file %s!", location);
        return std::string{""};
    }

    std::string line = "";
    while(!filestream.eof())
    {
        std::getline(filestream, line);
        content.append(line + '\n');
    }
    filestream.close();
    return content;
}

shader* make_shader(const char* vertex_code, const char* fragment_code)
{
    shader* s = new shader();
    s->compile(vertex_code, fragment_code);
    return s;    
}
shader* make_shader_from_file(const char* vertex_file, const char* fragment_file)
{
    return make_shader(read_file(vertex_file).c_str(), read_file(fragment_file).c_str());
}


shader* make_shader(const char* vertex_code, const char* fragment_code, const char* geometry_code)
{
    shader* s = new shader();
    s->compile(vertex_code, fragment_code, geometry_code);
    return s;    
}
shader* make_shader_from_file(const char* vertex_file, const char* fragment_file, const char* geometry_file)
{
    return make_shader(read_file(vertex_file).c_str(), read_file(fragment_file).c_str(), read_file(geometry_file).c_str());
}
#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include  <GL\glew.h>



class shader 
{
public:
    shader() = default;
    ~shader()
    {
        clear();
    }

    GLuint get_projection_location() { return m_projection; }
    GLuint get_model_location() { return m_model; }
    GLuint get_view_location() { return m_view; }
    GLuint get_ambient_intensity_location() { return m_ambient_intensity; }
    GLuint get_ambient_color_location() { return m_ambient_color; }
    GLuint get_diffuse_intensity_location() { return m_uniform_diffuse_intensity; }
    GLuint get_diffuse_direction_location() { return m_uniform_diffuse_direction; }
    GLuint get_specular_intensity_location() { return m_uniform_specular_intensity; }
    GLuint get_shininess_location() { return m_uniform_shininess; }
    GLuint get_eye_position_location() { return m_uniform_eye_position; }

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

private:
    friend shader* make_shader(const char* vertex_code, const char* fragment_code);
    void compile(const char* vertex_code, const char* fragment_code)
    {
        m_id = glCreateProgram();
        if(!m_id) {
            printf("Error creating shader program!\n"); 
            exit(1);
        }

        add(m_id, vertex_code, GL_VERTEX_SHADER);
        add(m_id, fragment_code, GL_FRAGMENT_SHADER);

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
        m_ambient_color = glGetUniformLocation(m_id, "direct_light.color");
        m_ambient_intensity = glGetUniformLocation(m_id, "direct_light.intensity");
        m_uniform_diffuse_direction = glGetUniformLocation(m_id, "direct_light.direction");
        m_uniform_diffuse_intensity = glGetUniformLocation(m_id, "direct_light.diffuse_intensity");
        m_uniform_specular_intensity = glGetUniformLocation(m_id, "m.specular_intensity");
        m_uniform_shininess = glGetUniformLocation(m_id, "m.shininess");
        m_uniform_eye_position = glGetUniformLocation(m_id, "eye_position");
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

private:
    GLuint m_id{0};
    GLuint m_projection{0};
    GLuint m_model{0};
    GLuint m_view{0};
    GLuint m_ambient_color{0};
    GLuint m_ambient_intensity{0};
    GLuint m_uniform_diffuse_intensity{0};
    GLuint m_uniform_diffuse_direction{0};
    GLuint m_uniform_eye_position{0};
    GLuint m_uniform_specular_intensity{0};
    GLuint m_uniform_shininess{0};
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

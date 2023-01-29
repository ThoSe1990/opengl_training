#pragma once

#include "point_light.hpp"

class spot_light : public point_light
{
public:
    spot_light() : point_light() {}
    spot_light(GLuint shadow_width, GLuint shadow_height,
     GLfloat near, GLfloat far,
     GLfloat red, GLfloat green, GLfloat blue, 
     GLfloat ambient_intensity, GLfloat diffuse_intensity, 
     GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, 
     GLfloat x_dir, GLfloat y_dir, GLfloat z_dir, 
     GLfloat constant, GLfloat linear, GLfloat exponent, 
     GLfloat edge) 
    : point_light(shadow_width, shadow_height, near, far, red, green, blue, ambient_intensity, diffuse_intensity, x_pos, y_pos, z_pos, constant, linear, exponent),
      m_direction{glm::normalize(glm::vec3(x_dir, y_dir, z_dir))},
    //   m_edge{edge},
      m_proc_edge{cosf(glm::radians(edge))}
    {}

    void toggle() 
    {
        m_is_on = !m_is_on;
    }

    void use_light(GLuint ambient_intensity_location, GLuint ambient_color_location, 
        GLuint diffuse_intensity_location, GLuint position_location, GLuint direction_location,
        GLuint constant_location, GLuint linear_location, GLuint exponent_location,
        GLuint edge_location)
    {
        glUniform3f(ambient_color_location, m_color.x, m_color.y, m_color.z);

        if (m_is_on) {
            glUniform1f(ambient_intensity_location, m_ambient_intensity);
            glUniform1f(diffuse_intensity_location, m_diffuse_intensity);
        } else {
            glUniform1f(ambient_intensity_location, 0.0f);
            glUniform1f(diffuse_intensity_location, 0.0f);
        }


        glUniform3f(position_location, m_position.x, m_position.y, m_position.z);
        glUniform1f(constant_location, m_constant);
        glUniform1f(linear_location, m_linear);
        glUniform1f(exponent_location, m_exponent);

        glUniform3f(direction_location, m_direction.x, m_direction.y, m_direction.z);
        glUniform1f(edge_location, m_proc_edge);
    }

    void set_flash(glm::vec3 pos, glm::vec3 dir) 
    {
        m_position = pos;
        m_direction = dir;
    }

private:
    glm::vec3 m_direction{glm::vec3(0.0f, -1.0f, 0.0f)};
    // GLfloat m_edge{0.0f};
    GLfloat m_proc_edge{0.0f};
    bool m_is_on{true};
};
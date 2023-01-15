#pragma once

#include "light.hpp"

class point_light : public light 
{
public: 
    point_light() = default;
    point_light(GLfloat red, GLfloat green, GLfloat blue, 
     GLfloat ambient_intensity, GLfloat diffuse_intensity, 
     GLfloat x_dir, GLfloat y_dir, GLfloat z_dir, 
     GLfloat constant, GLfloat linear, GLfloat exponent) 
    : light(red, green, blue, ambient_intensity, diffuse_intensity),
      m_position{glm::vec3(x_dir, y_dir, z_dir)}, m_constant(constant), m_linear(linear), m_exponent(exponent)
    {}
    ~point_light() = default;


    void use_light(GLuint ambient_intensity_location, GLuint ambient_color_location, 
        GLuint diffuse_intensity_location, GLuint position_location, 
        GLfloat constant_location, GLfloat linear_location, GLfloat exponent_location)
    {
        glUniform3f(ambient_color_location, m_color.x, m_color.y, m_color.z);
        glUniform1f(ambient_intensity_location, m_ambient_intensity);
        glUniform1f(diffuse_intensity_location, m_diffuse_intensity);

        glUniform3f(position_location, m_position.x, m_position.y, m_position.z);
        glUniform1f(constant_location, m_constant);
        glUniform1f(linear_location, m_linear);
        glUniform1f(exponent_location, m_exponent);
    }

private:
private:
    glm::vec3 m_position{glm::vec3(0.0f,0.0f,0.0f)};

    // light distance = ax^2 + bx + c
    GLfloat m_exponent{0.0f}; // a 
    GLfloat m_linear{0.0f}; // b
    GLfloat m_constant{1.0f}; // c

};
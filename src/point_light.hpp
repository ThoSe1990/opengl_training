#pragma once

#include "light.hpp"
#include "omni_shadow_map.hpp"

class point_light : public light 
{
public: 
    point_light() = default;
    point_light(GLuint shadow_width, GLuint shadow_height,
     GLfloat near, GLfloat far,
     GLfloat red, GLfloat green, GLfloat blue, 
     GLfloat ambient_intensity, GLfloat diffuse_intensity, 
     GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, 
     GLfloat constant, GLfloat linear, GLfloat exponent) 
    : light(shadow_width, shadow_height, red, green, blue, ambient_intensity, diffuse_intensity),
      m_position{glm::vec3(x_pos, y_pos, z_pos)}, m_constant(constant), m_linear(linear), m_exponent(exponent), m_far_plane{far}
    {
        float aspect = (float)shadow_width / (float)shadow_height;
        m_light_proj = glm::perspective(glm::radians(90.0f), aspect, near, far);

        m_shadow_map = new omni_shadow_map();
        m_shadow_map->init(shadow_width, shadow_height);
    }
    ~point_light() = default;

    std::vector<glm::mat4> calculate_light_transform()
    {
        std::vector<glm::mat4> light_matrices;
        // +x 
        light_matrices.push_back(m_light_proj * glm::lookAt(m_position, m_position+glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        // -x
        light_matrices.push_back(m_light_proj * glm::lookAt(m_position, m_position+glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        // +y
        light_matrices.push_back(m_light_proj * glm::lookAt(m_position, m_position+glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        // -y
        light_matrices.push_back(m_light_proj * glm::lookAt(m_position, m_position+glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        // +z
        light_matrices.push_back(m_light_proj * glm::lookAt(m_position, m_position+glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        // -z
        light_matrices.push_back(m_light_proj * glm::lookAt(m_position, m_position+glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        return light_matrices;
    }

    void use_light(GLuint ambient_intensity_location, GLuint ambient_color_location, 
        GLuint diffuse_intensity_location, GLuint position_location, 
        GLuint constant_location, GLuint linear_location, GLuint exponent_location)
    {
        glUniform3f(ambient_color_location, m_color.x, m_color.y, m_color.z);
        glUniform1f(ambient_intensity_location, m_ambient_intensity);
        glUniform1f(diffuse_intensity_location, m_diffuse_intensity);

        glUniform3f(position_location, m_position.x, m_position.y, m_position.z);
        glUniform1f(constant_location, m_constant);
        glUniform1f(linear_location, m_linear);
        glUniform1f(exponent_location, m_exponent);
    }

    GLfloat get_far_plane() const { return m_far_plane; }
    glm::vec3 get_position() const { return m_position; }
protected:
    glm::vec3 m_position{glm::vec3(0.0f,0.0f,0.0f)};

    // light distance = ax^2 + bx + c
    GLfloat m_exponent{0.0f}; // a 
    GLfloat m_linear{0.0f}; // b
    GLfloat m_constant{1.0f}; // c

    GLfloat m_far_plane{0.0f};
};
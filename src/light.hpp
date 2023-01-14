#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

class light
{
public:
    light() = default;
    light(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity, GLfloat x_dir, GLfloat y_dir, GLfloat z_dir, GLfloat diffuse_intensity) 
    : m_color{glm::vec3(red, green, blue)}, m_ambient_intensity{intensity}, m_direction{glm::vec3(x_dir, y_dir, z_dir)}, m_diffuse_intensity(diffuse_intensity)
    {}

    void use_light(GLuint ambient_intensity_location, GLuint ambient_color_location, GLuint diffuse_intensity_location, GLuint direction_location)
    {
        glUniform3f(ambient_color_location, m_color.x, m_color.y, m_color.z);
        glUniform1f(ambient_intensity_location, m_ambient_intensity);

        glUniform3f(direction_location, m_direction.x, m_direction.y, m_direction.z);
        glUniform1f(diffuse_intensity_location, m_diffuse_intensity);
    }

private:

private:
    glm::vec3 m_color{glm::vec3(1.0f, 1.0f, 1.0f)};
    GLfloat m_ambient_intensity{1.0f};

    glm::vec3 m_direction{glm::vec3(0.0f, -1.0f, 0.0f)};
    GLfloat m_diffuse_intensity{0.0f};
};
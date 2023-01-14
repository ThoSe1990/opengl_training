#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

class light
{
public:
    light() = default;
    light(GLfloat red, GLfloat green, GLfloat blue, GLfloat intensity) 
    : m_color{glm::vec3(red, green, blue)}, m_ambient_intensity{intensity}
    {}

    void use_light(GLfloat ambient_intensity_location, GLfloat ambient_color_location)
    {
        glUniform3f(ambient_color_location, m_color.x, m_color.y, m_color.z);
        glUniform1f(ambient_intensity_location, m_ambient_intensity);
    }

private:

private:
    glm::vec3 m_color{glm::vec3(1.0f, 1.0f, 1.0f)};
    GLfloat m_ambient_intensity{1.0f};
};
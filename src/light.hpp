#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

class light
{
public:
    light() = default;
    light(GLfloat red, GLfloat green, GLfloat blue, 
    GLfloat ambient_intensity, GLfloat diffuse_intensity) 
    : m_color{glm::vec3(red, green, blue)}, m_ambient_intensity{ambient_intensity}, m_diffuse_intensity(diffuse_intensity)
    {}

private:

protected:
    glm::vec3 m_color{glm::vec3(1.0f, 1.0f, 1.0f)};
    GLfloat m_ambient_intensity{1.0f};
    GLfloat m_diffuse_intensity{0.0f};
};
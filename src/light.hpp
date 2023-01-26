#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "shadow_map.hpp"

class light
{
public:
    light() = default;
    light(GLuint shadow_width, GLuint shadow_height,
        GLfloat red, GLfloat green, GLfloat blue, 
        GLfloat ambient_intensity, GLfloat diffuse_intensity) 
    : m_color{glm::vec3(red, green, blue)}, m_ambient_intensity{ambient_intensity}, m_diffuse_intensity(diffuse_intensity)
    {
        m_shadow_map = new shadow_map(shadow_width, shadow_height);
    }

    shadow_map* get_shadow_map() const { return m_shadow_map; }

private:

protected:
    glm::vec3 m_color{glm::vec3(1.0f, 1.0f, 1.0f)};
    GLfloat m_ambient_intensity{1.0f};
    GLfloat m_diffuse_intensity{0.0f};

    glm::mat4 m_light_proj;

    shadow_map* m_shadow_map;
};
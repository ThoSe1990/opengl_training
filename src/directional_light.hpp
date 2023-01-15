
#pragma once 

#include "light.hpp"

class directional_light : public light
{
public:
    directional_light() = default;
    directional_light(GLfloat red, GLfloat green, GLfloat blue, 
     GLfloat ambient_intensity, GLfloat diffuse_intensity, 
     GLfloat x_dir, GLfloat y_dir, GLfloat z_dir) 
    : light(red, green, blue, ambient_intensity, diffuse_intensity), 
      m_direction{glm::vec3(x_dir, y_dir, z_dir)}
    {}

    ~directional_light() = default;

    void use_light(GLuint ambient_intensity_location, GLuint ambient_color_location, 
        GLuint diffuse_intensity_location, GLuint direction_location)
    {
        glUniform3f(ambient_color_location, m_color.x, m_color.y, m_color.z);
        glUniform1f(ambient_intensity_location, m_ambient_intensity);

        glUniform3f(direction_location, m_direction.x, m_direction.y, m_direction.z);
        glUniform1f(diffuse_intensity_location, m_diffuse_intensity);
    }

private:
private:
    glm::vec3 m_direction{glm::vec3(0.0f, -1.0f, 0.0f)};

};


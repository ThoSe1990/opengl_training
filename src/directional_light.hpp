
#pragma once 

#include "light.hpp"

class directional_light : public light
{
public:
    directional_light() 
    {
        m_light_proj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
    }
    directional_light(GLuint shadow_width, GLuint shadow_height,
     GLfloat red, GLfloat green, GLfloat blue, 
     GLfloat ambient_intensity, GLfloat diffuse_intensity, 
     GLfloat x_dir, GLfloat y_dir, GLfloat z_dir) 
    : light(shadow_width, shadow_height,red, green, blue, ambient_intensity, diffuse_intensity), 
      m_direction{glm::vec3(x_dir, y_dir, z_dir)}
    {
        m_light_proj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
    }

    ~directional_light() = default;

    glm::mat4 calculate_light_transform()
    {
        return m_light_proj * glm::lookAt(-m_direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

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


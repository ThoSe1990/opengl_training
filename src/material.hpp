#pragma once 

#include <GL/glew.h>

class material 
{
public:
    material() = default;
    material(GLfloat intensity, GLfloat shininess) : m_specular_intensity{intensity}, m_shininess{shininess} {}

    void use(GLuint specular_intensity_location, GLuint shininess_location)
    {
        glUniform1f(specular_intensity_location, m_specular_intensity);
        glUniform1f(shininess_location, m_shininess);
    }
private:
private:
    GLfloat m_specular_intensity{0.0f};
    GLfloat m_shininess{0.0f};

};
#pragma once


#include <stdio.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

class window 
{
public:
    window() = default;
    window(GLint width, GLint height) : m_width(width), m_height(height)
    {

    }
    ~window() 
    {

    }

    int initialize()
    {
        // Initialise GLFW
        if (!glfwInit())
        {
            printf("GLFW initialisation failed!");
            glfwTerminate();
            return 1;
        }

        // Setup GLFW window properties
        // OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        // Core Profile = No Backwards Compatibility
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // Allow Forward Compatbility
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Create the window
        m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);
        if (!m_window)
        {
            printf("GLFW window creation failed!");
            glfwTerminate();
            return 1;
        }

        glfwGetFramebufferSize(m_window, &m_buffer_width, &m_buffer_height);

        // Set context for GLEW to use
        glfwMakeContextCurrent(m_window);

        // Allow modern extension features
        glewExperimental = GL_TRUE;

        if (glewInit() != GLEW_OK)
        {
            printf("GLEW initialisation failed!");
            glfwDestroyWindow(m_window);
            glfwTerminate();
            return 1;
        }


        glEnable(GL_DEPTH_TEST);

        // Setup Viewport size
        glViewport(0, 0, m_buffer_width, m_buffer_height);
    }

    GLfloat get_buffer_width() const { return m_buffer_width; }
    GLfloat get_buffer_height() const { return m_buffer_height; }
    bool get_should_close() const { return glfwWindowShouldClose(m_window); }
    void swap_buffers() { glfwSwapBuffers(m_window); }

private:

private:
    GLFWwindow* m_window;
    GLint m_width{800};
    GLint m_height{600};
    GLint m_buffer_width;
    GLint m_buffer_height;
};
#pragma once


#include <stdio.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

class window 
{
public:
    window() = default;
    window(GLint width, GLint height) : m_width(width), m_height(height) { }
    ~window() = default;

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

        // create callbacks for key and mouse input
        create_callbacks();
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

        glfwSetWindowUserPointer(m_window, this);

        return 0;
    }
    void swap_buffers() { glfwSwapBuffers(m_window); }
    GLfloat get_buffer_width() const { return m_buffer_width; }
    GLfloat get_buffer_height() const { return m_buffer_height; }
    bool get_should_close() const { return glfwWindowShouldClose(m_window); }
    bool* get_keys() { return m_keys; }
    GLfloat get_x_change() 
    { 
        GLfloat change = m_x_change;
        m_x_change = 0.0f;
        return change;
    }
    GLfloat get_y_change() 
    { 
        GLfloat change = m_y_change;
        m_y_change = 0.0f;
        return change;
    }
private:
    void create_callbacks()
    {
        glfwSetKeyCallback(m_window, handle_keys);
        glfwSetCursorPosCallback(m_window, handle_mouse);
    }

    static void handle_keys(GLFWwindow* w, int key, int code, int action, int mode)
    {
        window* this_window = static_cast<window*>(glfwGetWindowUserPointer(w));

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(w, GL_TRUE);
        }

        if (key >= 0 && key < 1024) {
            if (action == GLFW_PRESS) {
                this_window->m_keys[key] = true;
            } else if (action == GLFW_RELEASE) {
                this_window->m_keys[key] = false;
            }
        }
    }
    static void handle_mouse(GLFWwindow* w, double x, double y)
    {
        window* this_window = static_cast<window*>(glfwGetWindowUserPointer(w));

        if (this_window->m_mouse_first_moved)
        {
            this_window->m_last_x = x;
            this_window->m_last_y = y;
            this_window->m_mouse_first_moved = false;
        }

        this_window->m_x_change = x - this_window->m_last_x;
        this_window->m_y_change = this_window->m_last_y - y;

        this_window->m_last_x = x;
        this_window->m_last_y = y;
    }

private:
    GLFWwindow* m_window;
    GLint m_width{800};
    GLint m_height{600};
    GLint m_buffer_width;
    GLint m_buffer_height;
    bool m_keys[1024] = {false};

    GLfloat m_last_x{0.0f};
    GLfloat m_last_y{0.0f};
    GLfloat m_x_change{0.0f};
    GLfloat m_y_change{0.0f};
    bool m_mouse_first_moved{true};
};
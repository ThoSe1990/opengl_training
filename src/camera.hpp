#pragma once


#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <GLFW\glfw3.h>

class camera
{
public:
    camera() = default;
    camera(glm::vec3 pos, glm::vec3 up, GLfloat yaw, GLfloat pitch, GLfloat movement_speed, GLfloat turn_speed) :
    m_position(pos), m_world_up(up), m_yaw(yaw), m_pitch(pitch), m_movement_speed(movement_speed), m_turn_speed(turn_speed), m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    { 
        update();
    }
    ~camera() = default;

    void key_control(bool* keys, GLfloat delta_time)
    {
        GLfloat velocity = m_movement_speed * delta_time;
        if (keys[GLFW_KEY_W]) {
            m_position += m_front*velocity;
        }
        if (keys[GLFW_KEY_S]) {
            m_position -= m_front*velocity;
        }
        if (keys[GLFW_KEY_A]) {
            m_position -= m_right*velocity;
        }
        if (keys[GLFW_KEY_D]) {
            m_position += m_right*velocity;
        }
    }

    void mouse_control(GLfloat x_change, GLfloat y_change)
    {
        x_change *= m_turn_speed;
        y_change *= m_turn_speed;

        m_yaw += x_change;
        m_pitch += y_change;

        if (m_pitch > 89.0f) {
            m_pitch = 89.0f;
        }
        if (m_pitch < -89.0f) {
            m_pitch = -89.0f;
        }
        update();
    }

    glm::mat4 calculate_view_matrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

private:
    void update()
    {
        m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front.y = sin(glm::radians(m_pitch));
        m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(m_front);

        m_right = glm::normalize(glm::cross(m_front, m_world_up));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

private:
    glm::vec3 m_position{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_front{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_up{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_right{glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 m_world_up{glm::vec3(0.0f, 0.0f, 0.0f)};
    GLfloat m_yaw{0.0f};
    GLfloat m_pitch{0.0f};
    GLfloat m_movement_speed{0.0f};
    GLfloat m_turn_speed{0.0f};
};
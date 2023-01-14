#pragma once

#include <GL\glew.h>

#include <stb_image.h>

class texture
{
public:
    texture() = default;
    texture(const std::string& file) : m_file(file) { }
    ~texture() 
    {
        clear();
    }

    void load() 
    {
        unsigned char* data = stbi_load(m_file.c_str(), &m_width, &m_height, &m_bit_depth, 0);
        if (!data) {
            printf("failed to load %s\n", m_file);
            return;
        }

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }

    void clear()
    {
        glDeleteTextures(1, &m_id);
        m_id = 0;
        m_width = 0;
        m_height = 0;
        m_bit_depth = 0;
        m_file = "";
    }

    void use()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

private:

private:
    GLuint m_id{0};
    int m_width{0};
    int m_height{0};
    int m_bit_depth{0};
    std::string m_file{""};
};
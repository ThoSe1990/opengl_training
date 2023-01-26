#pragma once 

#include <stdio.h>

#include <GL/glew.h>

class shadow_map
{
public:
    shadow_map() = default;
    shadow_map(GLuint width, GLuint height) { init(width, height); }
    
    ~shadow_map()
    {
        if (m_fbo){
            glDeleteFramebuffers(1, &m_fbo);
        }
        if (m_id) {
            glDeleteTextures(1, &m_id);
        }
    }

    virtual bool init(GLuint width, GLuint height) 
    {
        m_width = width;
        m_height = height;

        glGenFramebuffers(1, &m_fbo);
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float border_colors[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colors);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_id, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            printf("framebuffer error: %i\n", status);
            return false;
        } 

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;

    }
    virtual void write() 
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }
    virtual void read(GLenum texture_unit) 
    {
        glActiveTexture(texture_unit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    GLuint get_width() { return m_width; }
    GLuint get_height() { return m_height; }

protected:
    GLuint m_fbo{0};
    GLuint m_id{0};
    GLuint m_width{0};
    GLuint m_height{0};
};
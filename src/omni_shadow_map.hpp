#pragma once 
#include <cstddef>
#include "shadow_map.hpp"

class omni_shadow_map : public shadow_map 
{
public: 
    omni_shadow_map () : shadow_map() {}

    bool init(GLuint width, GLuint height) override
    {
        m_width = width;
        m_height = height;

        glGenFramebuffers(1, &m_fbo);
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

        // 6 sides of a cube --> 6 textures 
        for (std::size_t i = 0 ; i < 6 ; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr); 
        }

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_id, 0);
        
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

    void write() override
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }
    void read(GLenum texture_unit) override
    {
        glActiveTexture(texture_unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    }
};
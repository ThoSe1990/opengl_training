#pragma once

class skybox 
{
public: 
    skybox() = default;
    skybox(std::vector<std::string> face_locations) 
    {
        // shader setup 
        m_shader = make_shader_from_file(vertex_skybox_file, fragment_skybox_file);
        m_uniform_projection_location = m_shader->get_projection_location();
        m_uniform_view_location = m_shader->get_view_location();


        // texture setup
        glGenTextures(1, &m_texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);

        int width;
        int height;
        int bit_depth;

        // 6 cube sides
        for (std::size_t i = 0 ; i < 6 ; i++) {
            unsigned char* data = stbi_load(face_locations[i].c_str(), &width, &height, &bit_depth, 0);
            if (!data) {
                printf("failed to load %s\n", face_locations[i].c_str());
                return;
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // mesh setup
        unsigned int indices [] = {
            // front
            0, 1, 2,
            2, 1, 3,
            //right
            2, 3, 5,
            5, 3, 7,
            // back
            5, 7, 4, 
            4, 7, 6,
            //left
            4, 6, 0,
            0, 6, 1,
            // top
            4, 0, 5,
            5, 0, 2,
            // bottom 
            1, 6, 3,
            3, 6, 7
        };
        float vertices[] = {
            -1.0f, 1.0f, -1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 
            -1.0f, -1.0f, -1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 
            1.0f, 1.0f, -1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 
            1.0f, -1.0f, -1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 

            -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 
            1.0f, 1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 
            -1.0f, -1.0f,  1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 
            1.0f, -1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f, 
        };

        m_mesh = new mesh(vertices, indices, 64, 36);
    }
    ~skybox() = default;

    void draw(glm::mat4 view_matrix, glm::mat4 projection_matrix)
    {
        view_matrix = glm::mat4(glm::mat3(view_matrix)); // avoids moving out the skybox

        glDepthMask(GL_FALSE);

        m_shader->use();

        glUniformMatrix4fv(m_uniform_projection_location, 1, GL_FALSE, glm::value_ptr(projection_matrix));	
	    glUniformMatrix4fv(m_uniform_view_location, 1, GL_FALSE, glm::value_ptr(view_matrix));	

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_id);

        m_shader->validate();
        m_mesh->render();

        glDepthMask(GL_TRUE);
    }

private: 
    mesh* m_mesh;
    shader* m_shader;

    GLuint m_texture_id{0};
    GLuint m_uniform_projection_location{0};
    GLuint m_uniform_view_location{0};
};
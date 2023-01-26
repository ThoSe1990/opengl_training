#pragma once

#include <vector>
#include <string>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "mesh.hpp"
#include "texture.hpp"

class model 
{
public: 
    model() = default;
    // model(const std::string& filename) { load(filename); }
    ~model() = default;

    void load(const std::string& filename)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

        if (!scene){
            printf("model (%s) failed to load: %s", filename.c_str(), importer.GetErrorString());
            return; 
        }
        load_node(scene->mRootNode, scene);
        load_materials(scene);
    }
    void render()
    {
        for (std::size_t i = 0 ; i < m_meshlist.size() ; i++) {
            unsigned int material_index = m_mesh_to_tex[i];
            if (material_index < m_texturelist.size() && m_texturelist[material_index]) {
                m_texturelist[material_index]->use();
            }
            m_meshlist[i]->render();
        }
    }
    void clear()
    {
        for (std::size_t i = 0 ; i < m_meshlist.size() ; i++){
            if (m_meshlist[i]) {
                delete m_meshlist[i];
                m_meshlist[i] = nullptr;
            }
        }
        for (std::size_t i = 0 ; i < m_texturelist.size() ; i++){
            if (m_texturelist[i]) {
                delete m_texturelist[i];
                m_texturelist[i] = nullptr;
            }
        }
    }
private:
    void load_node(aiNode* node, const aiScene* scene)
    {
        for (std::size_t i = 0 ; i < node->mNumMeshes ; i++){
            load_mesh(scene->mMeshes[node->mMeshes[i]], scene);
        }

        for (std::size_t i = 0 ; i < node->mNumChildren ; i++){
            load_node(node->mChildren[i], scene);
        }
    }

    void load_mesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<GLfloat> vertices;
        std::vector<unsigned int> indices;

        for (std::size_t i = 0 ; i < mesh->mNumVertices ; i++) {
            vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
            if (mesh->mTextureCoords[0]) {
                vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
            } else {
                vertices.insert(vertices.end(), { 0.0f, 0.0f });
            }
            vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
        }

        for (std::size_t i = 0 ; i < mesh->mNumFaces ; i++) {
            aiFace face = mesh->mFaces[i];
            for (std::size_t j = 0 ; j < face.mNumIndices ; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        m_meshlist.push_back(new ::mesh(&vertices[0], &indices[0], vertices.size(), indices.size()));
        m_mesh_to_tex.push_back(mesh->mMaterialIndex);
    }

    void load_materials(const aiScene* scene)
    {
        m_texturelist.resize(scene->mNumMaterials);
        for (std::size_t i = 0 ; i < scene->mNumMaterials ; i++) {
            aiMaterial* material = scene->mMaterials[i];
            m_texturelist[i] = nullptr;
            if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
                aiString path; 
                if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS){
                    int idx = std::string(path.data).rfind('\\');
                    std::string filename = std::string(path.data).substr(idx+1);
                    std::string texture_path = std::string{"textures/"}+filename;

                    m_texturelist[i] = new texture(texture_path);
                    if (!m_texturelist[i]->load()){
                        printf("failed to load texture: %s", texture_path.c_str());
                        delete m_texturelist[i];
                        m_texturelist[i] = nullptr;
                    }
                }
            }
            if (!m_texturelist[i]){
                m_texturelist[i] = new texture("textures/plain.png");
                m_texturelist[i]->load_with_alpha();
            }
        }
        
    }

private:
    std::vector<mesh*> m_meshlist;
    std::vector<texture*> m_texturelist;
    std::vector<unsigned int> m_mesh_to_tex;
};
#include "Mesh_class.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::vector<Texture>& textures, unsigned int instancing_amount, glm::mat4 models_pos[])
        : vertices(vertices), indices(indices), textures(std::move(textures)){
    if (instancing_amount == 0)
        setupMesh();
    else
        setupMesh(instancing_amount, models_pos);
}

void Mesh::setupMesh() {
    vbo = vertices;
    vao = VAO(vbo);
    ebo = indices;
    
    vao.Unbind();
    ebo.Unbind();
 }

void Mesh::setupMesh(unsigned int instancing_amount, glm::mat4 models_pos[]) {
    vbo = vertices;
    vao = VAO(vbo);
    //GLuint ad;
    //glGenBuffers(1, &ad);
    vbo_storage = { models_pos, instancing_amount };
    vao.MatInstance(3, vbo_storage);
    ebo = indices;
    
    vao.Unbind();
    ebo.Unbind();
}

void Mesh::Draw(Shader& shader) {
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    // sets textures
    for (unsigned int i = 0; i < textures.size(); i++) {
        char buff[100]{};
        glActiveTexture(GL_TEXTURE0 + i + (textures[i].GetId())); // "- 1" because texture's id starts with 1
        unsigned int number = 0;
        // gets proper name for a sampler
        if (textures[i].GetType() == "texture_diffuse")
            number = diffuse_nr++;
        else if (textures[i].GetType() == "texture_specular")
            number = specular_nr++;
        else
            continue;
        sprintf(buff, "material.%s%d", textures[i].GetType().c_str(), number);
        // sets sampler to the correct texture unit
        shader.setInt(buff, i + (textures[i].GetId()));
        // binds texture
        glBindTexture(GL_TEXTURE_2D, textures[i].GetId() + 1);
    }
    
    // draws elements
    vao.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    vao.Unbind();
}

void Mesh::Draw(Shader& shader, unsigned int objects_amount) {
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    for (unsigned int i = 0; i < textures.size(); i++) {
        char buff[100]{};
        glActiveTexture(GL_TEXTURE0 + i + (textures[i].GetId())); // "- 1" because texture's id starts with 1
        unsigned int number = 0;
        // gets proper name for a sampler
        if (textures[i].GetType() == "texture_diffuse")
            number = diffuse_nr++;
        else if (textures[i].GetType() == "texture_specular")
            number = specular_nr++;
        else
            continue;
        sprintf(buff, "material.%s%d", textures[i].GetType().c_str(), number);
        // sets sampler to the correct texture unit
        shader.setInt(buff, i + (textures[i].GetId()));
        // binds texture
        glBindTexture(GL_TEXTURE_2D, textures[i].GetId() + 1);
    }

    // draws elements
    vao.Bind();
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, objects_amount);
    vao.Unbind();
}
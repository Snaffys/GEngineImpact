#include "Mesh_class.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, unsigned int instancing_amount, glm::mat4 models_pos[])
        : vertices(vertices), indices(indices), textures(textures){
    if (instancing_amount == 1)
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
    
    //glGenBuffers(1, &ebo_1);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_1);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Mesh::setupMesh(unsigned int instancing_amount, glm::mat4 models_pos[]) {
    vbo = vertices;
    vao = VAO(vbo);
    vbo_storage = { models_pos, instancing_amount };
    vao.MatInstance(3, vbo_storage);
    ebo = indices;

    vao.Unbind();
    ebo.Unbind();
}

void Mesh::Draw(Shader& shader) {
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, 0);
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

    //glActiveTexture(GL_TEXTURE0 + (textures[0].id - 1));
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
#include "Mesh_class.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture_model> &textures)
        : vertices(vertices), indices(indices), textures(textures){
    setupMesh();
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

void Mesh::Draw(Shader& shader) {
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, 0);
    // sets textures
    for (unsigned int i = 0; i < textures.size(); i++) {
        char buff[100]{};
        glActiveTexture(GL_TEXTURE0 + i + (textures[i].id - 1)); // "- 1" because texture's id starts with 1
        unsigned int number = 0;
        // gets proper name for a sampler
        if (textures[i].type == "texture_diffuse")
            number = diffuse_nr++;
        else if (textures[i].type == "texture_specular")
            number = specular_nr++;
        else
            continue;
        sprintf(buff, "material.%s%d", textures[i].type.c_str(), number);
        // sets sampler to the correct texture unit
        shader.setInt(buff, i + (textures[i].id - 1));
        // binds texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    
    // draws elements
    vao.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    vao.Unbind();

    //glActiveTexture(GL_TEXTURE0 + (textures[0].id - 1));
}
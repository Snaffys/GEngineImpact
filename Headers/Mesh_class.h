#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <iostream>

#include <vector>

#include "Shader_class.h"

#include "VAO_class.h"
#include "VBO_class.h"
#include "EBO_class.h"
#include "FBO_class.h"
#include "Texture_class.h"

#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh {
	public:
		Mesh(const std::vector<VertexTan> &vertices, const std::vector<unsigned int> &indices, std::vector<Texture> &textures, unsigned int instancing_amount, glm::mat4 models_pos[]);

		void Draw(Shader &shader);
		void Draw(Shader& shader, unsigned int objects_amount);

	private:
		std::vector<VertexTan> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		void setupMesh();

		void setupMesh(unsigned int instancing_amount, glm::mat4 models_pos[]);

		NormalVAO vao;
		VBO vbo;
		VBO vbo_storage;
		EBO ebo;
};

#endif
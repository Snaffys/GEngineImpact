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

struct Texture_model {
	unsigned int id;
	std::string type;
	std::string filename;	// for future checking if texture already loaded
};

class Mesh {
	public:
		Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures, unsigned int instancing_amount, glm::mat4 models_pos[]);

		void Draw(Shader &shader);
		void Draw(Shader& shader, unsigned int objects_amount);

	private:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		void setupMesh();

		void setupMesh(unsigned int instancing_amount, glm::mat4 models_pos[]);

		VAO vao;
		VBO vbo;
		VBO vbo_storage;
		EBO ebo;
};

#endif
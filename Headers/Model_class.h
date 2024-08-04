#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include "Shader_class.h"
#include "Mesh_class.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

unsigned int TextureFromFile(std::string path, const std::string& directory, bool gamma = false);

class Model {
	public:
		Model() = default;
		Model(const char* path, glm::mat4 models_pos[] = {}, unsigned int instansing_amount = 1);
		void Draw(Shader& shader);
		void Draw(Shader& shader, unsigned int models_amount);
	private:
		unsigned int instancing_amount;
		Texture texture;

		glm::mat4* model_pos;

		std::vector<Texture> textures_loaded;
		std::vector<Mesh> meshes;
		std::filesystem::path directory;
		bool gammaCorrection;

		void loadModel(std::filesystem::path path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

};

#endif
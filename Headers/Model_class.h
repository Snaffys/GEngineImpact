#pragma once
#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include "Shader_class.h"
#include "Mesh_class.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int TextureFromFile(std::string path, const std::string& directory, bool gamma = false);

class Model {
	public:
		Model() = default;
		Model(const char* path);
		void Draw(Shader& shader);
	private:
		std::vector<Texture_model> textures_loaded;
		std::vector<Mesh> meshes;
		std::string directory;
		bool gammaCorrection;

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture_model> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

};

#endif
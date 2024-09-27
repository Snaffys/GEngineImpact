#include "Model_class.h"

Model::Model(const char* path, glm::mat4 models_pos[], unsigned int instansing_amount) {
	this->instancing_amount = instansing_amount;
	model_pos = models_pos;
	loadModel(path);
}

void Model::Draw(Shader& shader) {
	// meshes drawing loop
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::Draw(Shader& shader, unsigned int models_amount) {
	// meshes drawing loop
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, models_amount);
}

void Model::loadModel(std::filesystem::path path) {
	Assimp::Importer importer;
	// reads file and returns its contents
	const aiScene *scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);	// aiProcess_Triangulate - if the model doesn't(entirely) consists of triangles transforms its primitive shapes to triangles
																														// aiProcess_FlipUVs - flips the texture cooridinates on the y-axis where necessary
																														// aiProcess_GenNormals - creates normal vectors for each vertex if the model doesn't contain normal vectors
	// checks errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
		return;
	}
	// finds directory to model
	directory = path.parent_path();
	
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// loops through all meshes in the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// gets current mesh in the node from the scene
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		// sends mesh into the mesh vector
		meshes.push_back(processMesh(mesh, scene));
	}
	// loops through all children in all nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	// mesh data
	std::vector<VertexTan> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		VertexTan vertex;
		// gets position
		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;
		// gets normals
		if (mesh->HasNormals()) {
			vertex.normals.x = mesh->mNormals[i].x;
			vertex.normals.y = mesh->mNormals[i].y;
			vertex.normals.z = mesh->mNormals[i].z;
		}
		else
			vertex.normals = glm::vec3(0.0f, 0.0f, 0.0f);
		// gets texture coords
		if (mesh->mTextureCoords[0]) {	// mesh can have no texture coordinates
			vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;	// model can have up to 8 different texture coordinates per vertex
			vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);
		if (mesh->HasTangentsAndBitangents()) {
			vertex.tangents.x = mesh->mTangents[i].x;
			vertex.tangents.y = mesh->mTangents[i].y;
			vertex.tangents.z = mesh->mTangents[i].z;
			vertex.bitangents.x = mesh->mBitangents[i].x;
			vertex.bitangents.y = mesh->mBitangents[i].y;
			vertex.bitangents.z = mesh->mBitangents[i].z;
		}
		// send the vertices in the vector
		vertices.push_back(vertex);
	}
	// processes indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		// gets i face in the mesh
		aiFace face = mesh->mFaces[i];
		// loops through all indices in the face
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// processes material
	// gets mesh's material index from scene that stores an array of texture loacations for each texture type
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	// inserts diffuseMaps vector into the end of textures vector
	textures.insert(textures.end(), std::make_move_iterator(diffuseMaps.begin()), std::make_move_iterator(diffuseMaps.end()));

	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), std::make_move_iterator(specularMaps.begin()), std::make_move_iterator(specularMaps.end()));

	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal_map");
	textures.insert(textures.end(), std::make_move_iterator(normalMaps.begin()), std::make_move_iterator(normalMaps.end()));

	std::vector<Texture> displacementlMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "displacement_map");
	textures.insert(textures.end(), std::make_move_iterator(displacementlMaps.begin()), std::make_move_iterator(displacementlMaps.end()));

	return Mesh(vertices, indices, textures, instancing_amount, model_pos);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, const char* typeName) {
	std::vector<Texture> textures;
	// looping through all textures with specific type in the material
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
		// name of the file
		aiString material_path;

		std::string filename;

		// gets all textures parameters from the material
		material->GetTexture(type, i, &material_path);
		filename = std::filesystem::path(material_path.data).filename().string();

		bool skip = false;
		// loops through all loaded textures to check if the texture was already loaded
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			// of texture was loaded
			if (std::strcmp((*textures_loaded[j]).GetFilename().data(), filename.c_str()) == 0) {
				// sends texture in the textures vector
				textures.push_back(*textures_loaded[j]);
				// skips texture loading
				skip = true;
				break;
			}
		}
		// if texture wasn't loaded
		if (!skip) {
			textures.emplace_back(filename, this->directory.string(), typeName);
			textures_loaded.emplace_back(&textures.back());
		}
	}
	return textures;
}
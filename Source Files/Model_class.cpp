#include "Model_class.h"

Model::Model(const char* path) {
	loadModel(path);
}

void Model::Draw(Shader& shader) {
	// meshes drawing loop
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::loadModel(std::string path) {
	Assimp::Importer importer;
	// reads file and returns its contents
	//const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcessPreset_TargetRealtime_MaxQuality);	// aiProcess_Triangulate - if the model doesn't(entirely) consists of triangles transforms its primitive shapes to triangles
																														// aiProcess_FlipUVs - flips the texture cooridinates on the y-axis where necessary
																														// aiProcess_GenNormals - creates normal vectors for each vertex if the model doesn't contain normal vectors
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	// checks errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	// finds directory to model
	directory = path.substr(0, path.find_last_of("\\")); // '\' for LINUX

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
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture_model> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
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
		// gets texture coords
		if (mesh->mTextureCoords[0]) {	// mesh can has no texture coordinates
			vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;	// model can has up to 8 differnt texture coordinates per vertex
			vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);
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
	// gets diffuseMaps
	std::vector<Texture_model> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	// inserts diffuseMaps vector into the end of textures vector
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// gets specularMaps
	std::vector<Texture_model> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	// inserts specularMaps vector into the end of textures vector
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	return Mesh(vertices, indices, textures);
}

std::vector<Texture_model> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName) {
	std::vector<Texture_model> textures;
	// looping through all textures with specific type in the material
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
		// name of the file
		aiString string;

		std::string str;

		// gets all textures parameters from the material
		material->GetTexture(type, i, &string);		
		str = string.data;
		str = str.substr(str.find_last_of("\\") + 1, str.length());
		str = str.substr(str.find_last_of("/") + 1, str.length());

		bool skip = false;
		// loops through all loaded textures to check if the texture was already loaded
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			// of texture was loaded
			if (std::strcmp(textures_loaded[j].filename.data(), str.c_str()) == 0) {
				// sends texture in the textures vector
				textures.push_back(textures_loaded[j]);
				// skips texture loading
				skip = true;
				break;
			}
		}
		// if texture wasn't loaded
		if (!skip) {
			Texture_model texture;
			// initializes texture fieds
			texture.id = TextureFromFile(str, this->directory);
			texture.type = typeName;
			texture.filename = str;
			// sends texture in the vectors
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

// generates texture and returnes its id
unsigned int TextureFromFile(std::string path, const std::string& directory, bool gamma)
{
	// name of the file
	std::string filename = std::string(path);
	// path to the file
	filename = directory + "\\textures\\" + filename;
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0 + textureID - 1);
	int width, height, nrComponents;
	// gets data about texture from file
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		// chooses format
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		// binds texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		// generates currently bounded texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		// generates mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		// texture stretching
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// texture filtering(blurred or pixeled)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// frees data
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		// frees data
		stbi_image_free(data);
	}

	return textureID;
}

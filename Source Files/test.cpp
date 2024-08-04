//#include "FBO_class.h"
//#include "Texture_class.h"
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//#include <filesystem>
//
//void asdf(std::filesystem::path path) {
//
//	if (vboModelData.GetBufferID() == 0)
//	{
//		vboModelData.CreateVBO();
//		tTextures.reserve(50);
//	}
//
//
//	Assimp::Importer importer;
//	const aiScene* scene = importer.ReadFile(path.string(),
//		aiProcess_CalcTangentSpace |
//		aiProcess_Triangulate |
//		aiProcess_JoinIdenticalVertices |
//		aiProcess_SortByPType);
//
//	if (!scene)
//	{
//		printf("ERROR::ASSIMP::%s\n", importer.GetErrorString());
//		return;
//	}
//
//
//
//
//
//
//	const int iVertexTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);
//
//	int iTotalVertices = 0;
//
//	for (int i = 0; i < scene->mNumMeshes; i++)
//	{
//		aiMesh* mesh = scene->mMeshes[i];
//		int iMeshFaces = mesh->mNumFaces;
//		iMaterialIndices.push_back(mesh->mMaterialIndex);
//		int iSizeBefore = vboModelData.GetCurrentSize();
//		iMeshStartIndices.push_back(iSizeBefore / iVertexTotalSize);
//		for (int j = 0; j < iMeshFaces; j++)
//		{
//			const aiFace& face = mesh->mFaces[j];
//			for(int k = 0; k < 3; k++)
//			{
//				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
//				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
//				aiVector3D normal = mesh->mNormals[face.mIndices[k]];
//				vboModelData.AddData(&pos, sizeof(aiVector3D));
//				vboModelData.AddData(&uv, sizeof(aiVector2D));
//				vboModelData.AddData(&normal, sizeof(aiVector3D));
//			}
//		}
//		int iMeshVertices = mesh->mNumVertices;
//		iTotalVertices += iMeshVertices;
//		iMeshSizes.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / iVertexTotalSize);
//	}
//
//
//	std::vector materialRemap(iNumMaterials);
//
//	FOR(i, iNumMaterials)
//	{
//		const aiMaterial* material = scene->mMaterials[i];
//		int a = 5;
//		int texIndex = 0;
//		aiString path;  // filename
//
//		if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
//		{
//			string sDir = GetDirectoryPath(sFilePath);
//			string sTextureName = path.data;
//			string sFullPath = sDir + sTextureName;
//			int iTexFound = -1;
//			FOR(j, ESZ(tTextures))if (sFullPath == tTextures[j].GetPath())
//			{
//				iTexFound = j;
//				break;
//			}
//			if (iTexFound != -1)materialRemap[i] = iTexFound;
//			else
//			{
//				CTexture tNew;
//				tNew.LoadTexture2D(sFullPath, true);
//				materialRemap[i] = ESZ(tTextures);
//				tTextures.push_back(tNew);
//			}
//		}
//	}
//
//	FOR(i, ESZ(iMeshSizes))
//	{
//		int iOldIndex = iMaterialIndices[i];
//		iMaterialIndices[i] = materialRemap[iOldIndex];
//	}
//}
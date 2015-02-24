//StaticMesh.cpp - implementation of loading meshes, initialising vertex buffer data etc
// http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/ was a great reference in building 
//this

//last update 05/11/2014 - made non OOP

#include <aiScene.h>
#include <aiMesh.h>
#include <assimp.hpp>
#include <assimp.h>
#include <fstream>
#include <iostream>
#include "StaticMesh.h"
#include "VertexTypes.h"
#include "VertexArray.h"


bool InitStaticMesh(StaticMesh& mesh, const string& fileName, const string& directory, int instances,unsigned int flags){
	
	//check if file exists
	std::ifstream fin(directory + fileName);

	if(!fin.fail()){
		fin.close();
	}else{
		std::cout << "Couldn't open file "<< directory + fileName <<" it doesn't exist!" << std::endl;
		return false;
	}

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(directory + fileName,flags);

	if(!scene){
		std::cout <<"couldn't load scene! Reason: "<< importer.GetErrorString() <<std::endl;
		return false;
	}

	GetBoundingBox(mesh.m_boundingBoxMin,mesh.m_boundingBoxMax,scene);

	float biggestDifferenceAxis = 0.0f;
	biggestDifferenceAxis = mesh.m_boundingBoxMax.x - mesh.m_boundingBoxMin.x; 

	if(mesh.m_boundingBoxMax.y - mesh.m_boundingBoxMin.y > biggestDifferenceAxis ){
		biggestDifferenceAxis = mesh.m_boundingBoxMax.y - mesh.m_boundingBoxMin.y;
	}

	if (mesh.m_boundingBoxMax.z - mesh.m_boundingBoxMin.z > biggestDifferenceAxis){
		biggestDifferenceAxis = mesh.m_boundingBoxMax.z - mesh.m_boundingBoxMin.z;
	}

	std::cout<<"number of meshes in Init: "<< scene->mNumMeshes << std::endl;

	mesh.m_numMeshes = scene->mNumMeshes;
	mesh.m_meshData.reserve(mesh.m_numMeshes);

	//now init vaos and buffers...
		
	//find out if this mesh has positions, normals, textures
	for (unsigned int meshNum = 0; meshNum < mesh.m_numMeshes; meshNum++){
		int numAttribs = 0;

		const aiMesh* thisMesh = scene->mMeshes[meshNum];

		unsigned int numFaces = thisMesh->mNumFaces;

		//copy the mesh's face data

		unsigned int *indices = new unsigned int[numFaces * 3];


		for(unsigned int face = 0, vertexIndex = 0; face < numFaces; face++, vertexIndex += 3){
			const struct aiFace* meshFace = &thisMesh->mFaces[face];
			memcpy(&indices[vertexIndex],meshFace->mIndices,sizeof(unsigned int) * 3);
		}

		//copy the mesh's vertex data

		//create a new objectdata to store the mesh data on the gfx card
		MeshComponent* newComp = new MeshComponent;
		newComp->m_numFaces = numFaces;
		unsigned int numVerts = thisMesh->mNumVertices;
		//here is a slightly messy way of allocating object data....
		//generate vertex array for this mesh using objectData
		if (thisMesh->HasPositions() && thisMesh->HasNormals() && thisMesh->HasTextureCoords(0) && thisMesh->HasTangentsAndBitangents()){
			CustomVertexNormBiTangentUV *vertices = new CustomVertexNormBiTangentUV[numVerts];

			int vecSize = sizeof(float) * 3;
			int texVecSize = sizeof(float) * 2;
			for (unsigned int vertex = 0; vertex < numVerts; vertex++){
				thisMesh->mTextureCoords[0][vertex].x = thisMesh->mTextureCoords[0][vertex].x;
				thisMesh->mTextureCoords[0][vertex].y = thisMesh->mTextureCoords[0][vertex].y;
				memcpy(&vertices[vertex].vertexPoint, &thisMesh->mVertices[vertex], vecSize);
				memcpy(&vertices[vertex].normal,      &thisMesh->mNormals[vertex], vecSize);
				memcpy(&vertices[vertex].biTangent,   &thisMesh->mBitangents[vertex], vecSize);
				memcpy(&vertices[vertex].uv,          &thisMesh->mTextureCoords[0][vertex], texVecSize);
			}

			if (instances > 1){
				newComp->m_vertexBuffer = CreateInstancedBumpmappedVertexArray(vertices, numVerts,
					indices, numFaces, instances, newComp->m_instancedDataBuffer);
			}
			else{
				newComp->m_vertexBuffer = CreateBumpmappedVertexArray(vertices, numVerts, indices, numFaces);
			}
			delete[] vertices;

		}else if(thisMesh->HasPositions() && thisMesh->HasNormals() && thisMesh->HasTextureCoords(0)){
			//create a new objectdata to store the mesh data on the gfx card
			
			CustomVertexNormUV *vertices = new CustomVertexNormUV[numVerts];
			
			int vecSize = sizeof(float) * 3;
			int texVecSize = sizeof(float) * 2;
			for (unsigned int vertex = 0; vertex < numVerts; vertex++){
				thisMesh->mTextureCoords[0][vertex].x = -thisMesh->mTextureCoords[0][vertex].x;
				thisMesh->mTextureCoords[0][vertex].y = -thisMesh->mTextureCoords[0][vertex].y;
				memcpy(&vertices[vertex].vertexPoint, &thisMesh->mVertices[vertex], vecSize);
				memcpy(&vertices[vertex].normal, &thisMesh->mNormals[vertex], vecSize);
				memcpy(&vertices[vertex].uv, &thisMesh->mTextureCoords[0][vertex], texVecSize);
			}

			if (instances > 1){
				newComp->m_vertexBuffer = CreateInstancedVertexNormUVArray(vertices, numVerts, 
																		indices, numFaces,instances,newComp->m_instancedDataBuffer);
			}else{
				newComp->m_vertexBuffer = CreateVertexNormUVArray(vertices, numVerts, indices, numFaces);
			}
			delete[] vertices;
		}else if(thisMesh->HasPositions() && thisMesh->HasNormals()){
			//create a new objectdata to store the mesh data on the gfx card
			CustomVertexNorm *vertices = new CustomVertexNorm[numVerts];

			int vecSize = sizeof(float) * 3;
			for (unsigned int vertex = 0; vertex < numVerts; vertex++){
				memcpy(&vertices[vertex].vertexPoint, &thisMesh->mVertices[vertex], vecSize);
				memcpy(&vertices[vertex].normal, &thisMesh->mNormals[vertex], vecSize);
			}

			if (instances > 1){
				newComp->m_vertexBuffer = CreateInstancedVertexNormArray(vertices, numVerts,
					indices, numFaces, instances, newComp->m_instancedDataBuffer);
			}else{
				newComp->m_vertexBuffer = CreateVertexNormArray(vertices, numVerts,
					indices, numFaces);
			}

			delete[] vertices;
		}else {
			//create a new objectdata to store the mesh data on the gfx card
			glm::vec3 *vertices = new glm::vec3[thisMesh->mNumVertices];

			unsigned int vecSize = sizeof(float) * 3;
			for (unsigned int vertex = 0; vertex < numVerts; vertex++){
				memcpy(&vertices[vertex], &thisMesh->mVertices[vertex], vecSize);
			}

			newComp->m_vertexBuffer = CreateSimpleVertexArray(vertices, numVerts, indices, numFaces);

			delete[] vertices;
		}


		Material newMat;
		GLuint	  newTex = NULL;

		//load materials
		newMat = LoadMaterials(scene,scene->mMaterials[thisMesh->mMaterialIndex]);
		newTex = LoadTextures(mesh,scene,scene->mMaterials[thisMesh->mMaterialIndex],directory);
		
		newComp->m_material = newMat;

		if(newTex != 0){
			newComp->m_hasTexture = true;
			newComp->m_texture = newTex;
		}

		mesh.m_meshData.push_back(newComp);
	}

	mesh.m_numInstances = instances;
	return true;
}



void GetBoundingBox(glm::vec3& min,glm::vec3& max,const aiScene* scene){
	aiMatrix4x4 trafo;
    aiIdentityMatrix4(&trafo);

    min.x = min.y = min.z =  1e10f;
    max.x = max.y = max.z = -1e10f;
    GetBoundingBoxForNode(scene->mRootNode,min,max,trafo,scene);
}

void GetBoundingBoxForNode(const aiNode* node,glm::vec3& min,glm::vec3& max,aiMatrix4x4& trafo,
				                        const aiScene* scene){
	aiMatrix4x4 prev;

	prev = trafo;
	aiMultiplyMatrix4(&trafo,&node->mTransformation);

	for (unsigned int n = 0; n < node->mNumMeshes; ++n) {
        const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];
            
		for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {

                struct aiVector3D tmp = mesh->mVertices[t];
                aiTransformVecByMatrix4(&tmp,&trafo);

                min.x = glm::min(min.x,tmp.x);
                min.y = glm::min(min.y,tmp.y);
                min.z = glm::min(min.z,tmp.z);

                max.x = glm::max(max.x,tmp.x);
                max.y = glm::max(max.y,tmp.y);
                max.z = glm::max(max.z,tmp.z);
        }
    }

    for (unsigned int n = 0; n < node->mNumChildren; n++) {
        GetBoundingBoxForNode(node->mChildren[n],min,max,trafo,scene);
    }

    trafo = prev;
}

Material LoadMaterials(const aiScene* scene,aiMaterial* material)
{
	Material mat;

	float c[4] = {0.8f, 0.8f, 0.8f, 1.0f};

    aiColor4D diffuse;
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse)){
		memcpy(&mat.diffuse,&diffuse, sizeof(diffuse));
	}else{
		memcpy(&mat.diffuse,c, sizeof(c));
	}
 
	c[0] = 0.2f;
	c[1] = 0.2f; 
	c[2] = 0.2f;
	c[3] = 1.0f;

    aiColor4D ambient;
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient)){
        memcpy(&mat.ambient, &ambient, sizeof(ambient));
	}else{
		memcpy(&mat.ambient,c, sizeof(c));
	}
 
	c[0] = 0.6f;
	c[1] = 0.6f; 
	c[2] = 0.6f;
	c[3] = 1.0f;
      
    aiColor4D specular;
   if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular)){
          memcpy(&mat.specular, &specular, sizeof(specular));
   }else{
		memcpy(&mat.specular, c, sizeof(c));
   }
 
    float shininess = 64.0f;
   // aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);
    mat.shininess = shininess;

	return mat;
}

GLuint LoadTextures(StaticMesh& mesh, const aiScene* scene,aiMaterial* material,const string& directory){
		
	int texIndex = 0;
	aiString path;  // filename

	std::vector<char*> texNames;

	aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path, NULL, NULL, NULL,NULL,NULL);

	GLuint texture = 0;
	if (texFound == AI_SUCCESS){

		map<string,GLuint>::iterator it = mesh.m_textures.find(path.data);

		if (it == mesh.m_textures.end()){
			texture = CreateTexture(directory + path.data, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
			mesh.m_textures[path.data] = texture;
		}else{
			texture = it->second;
		}
	}
	
	return texture;
}

void RenderStaticMesh(const StaticMesh& mesh)
{
	for (unsigned int meshNum = 0; meshNum < mesh.m_numMeshes; meshNum++){

		if (mesh.m_meshData[meshNum]->m_hasTexture){
			glBindTexture(GL_TEXTURE_2D, mesh.m_meshData[meshNum]->m_texture);
		}

		glBindVertexArray(mesh.m_meshData[meshNum]->m_vertexBuffer);
		glDrawElements(GL_TRIANGLES, mesh.m_meshData[meshNum]->m_numFaces * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

void RenderInstancedStaticMesh(const StaticMesh& mesh,const vec3* positions){
	
	for (unsigned int meshNum = 0; meshNum < mesh.m_numMeshes; meshNum++){

		if (mesh.m_meshData[meshNum]->m_hasTexture){
			glBindTexture(GL_TEXTURE_2D, mesh.m_meshData[meshNum]->m_texture);
		}

		glBindVertexArray(mesh.m_meshData[meshNum]->m_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.m_meshData[meshNum]->m_instancedDataBuffer);
		glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(vec3) * mesh.m_numInstances, positions);
		glDrawElementsInstanced(GL_TRIANGLES, mesh.m_meshData[meshNum]->m_numFaces * 3, 
								GL_UNSIGNED_INT, 0,mesh.m_numInstances);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void  DestroyMesh(StaticMesh& mesh){
	for (unsigned int meshNum = 0; meshNum < mesh.m_numMeshes; meshNum++){

		if (mesh.m_meshData[meshNum]->m_hasTexture){
			glDeleteTextures(1, &mesh.m_meshData[meshNum]->m_texture);
		}

		glDeleteVertexArrays(1, &mesh.m_meshData[meshNum]->m_vertexBuffer);
	}
}

//StaticMesh.h - this class uses assimp to load models and is used to represent all the data in a mesh
//through assimp's supported classes it can store many kinds of file type
//last updata 03/03/2015 - changed from lat engine type to this one

#pragma once

#include <map>
#include <aiPostProcess.h>
#include <aiMaterial.h>
#include "Texture.h"
#include "Shader.h"
#include "LightsAndMaterials.h"

struct aiScene;
struct aiNode;
struct aiMatrix4x4;
struct aiMaterial;

struct MeshComponent{
	GLuint          m_vertexBuffer		  = 0;
	GLuint          m_numFaces			  = 0;
	GLuint			m_instancedDataBuffer = 0;
	Material		m_material;
};


struct StaticMesh{

		std::vector<MeshComponent*> m_meshData;
		glm::vec3					m_boundingBoxMin;
		glm::vec3					m_boundingBoxMax;
		unsigned int				m_numMaterials;
		unsigned int				m_numMeshes;
		std::map<string, GLuint>    m_textures;

		~StaticMesh();
};
 


bool				InitStaticMesh(StaticMesh& newMesh, const string& fileName, const string& directory,
									unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals);

void				RenderStaticMeshComponent(const MeshComponent* comp);

void				RenderInstancedStaticMeshComponent(const MeshComponent* comp, int numInstances);



//StaticMesh.h - this class uses assimp to load models and is used to represent all the data in a mesh
//through assimp's supported classes it can store many kinds of file type
//last updata 03/03/2015 - changed from lat engine type to this one

#pragma once

#include <assimp\postprocess.h>
#include <assimp\material.h>
#include "BoundingBox.h"
#include "Texture.h"
#include "Shader.h"
#include "LightsAndMaterials.h"

struct aiScene;
struct aiNode;
struct aiMaterial;

struct MaterialInfo{
	GLuint  		m_texture   = 0;
	GLuint          m_normalMap = 0;
	GLuint          m_specMap   = 0;
	GLuint          m_alphaMap = 0;
	Material        m_material;
};

struct MeshComponent{
	GLuint          m_vertexBuffer		  = 0;
	GLuint          m_numFaces			  = 0;
	GLuint			m_instancedDataBuffer = 0;
	int             m_materialIndex		  = 0;
	BoundingBox		m_boundingBox;
};

struct StaticMesh{

		std::vector<MeshComponent>  m_meshData;
		std::vector<MaterialInfo>	m_materialData;
		unsigned int				m_numMaterials;
		unsigned int				m_numMeshes;
		BoundingBox					m_boundingBox;
		~StaticMesh();
};
 
bool				InitStaticMesh(StaticMesh& newMesh, const string& fileName, const string& directory,
									unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals);

void				RenderStaticMeshComponent(const MeshComponent& comp);

void				RenderInstancedStaticMeshComponent(const MeshComponent& comp, int numInstances);




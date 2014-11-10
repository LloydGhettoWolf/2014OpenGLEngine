//StaticMesh.h - this class uses assimp to load models and is used to represent all the data in a mesh
//through assimp's supported classes it can store many kinds of file type
//last updata 05/11/2014 - changed from lat engine type to this one

#pragma once

#include <map>
#include "Texture.h"
#include "Shader.h"
#include "LightsAndMaterials.h"

struct aiScene;
struct aiNode;
struct aiMatrix4x4;
struct aiMaterial;

struct MeshComponent{
	GLuint          m_vertexBuffer	= 0;
	GLuint          m_texture		= 0;
	GLuint          m_numFaces		= 0;
	Material		m_material;
	bool            m_hasTexture	= false;
};


struct StaticMesh{

		std::vector<MeshComponent*> m_meshData;
		glm::vec3					m_boundingBoxMin;
		glm::vec3					m_boundingBoxMax;
		unsigned int				m_numMaterials;
		unsigned int				m_numMeshes;
		int							m_numInstances = 1;
		GLuint						m_instancedDataBuffer;
		std::map<string, GLuint>    m_textures;
};


bool				InitStaticMesh(StaticMesh& newMesh,char* fileName,int instances = 1);
void				RenderStaticMesh(const StaticMesh& mesh,MaterialUniforms& uniforms);

void				RenderInstancedStaticMesh(const StaticMesh& mesh, MaterialUniforms& uniforms,vec3* positions);

//support functions for use by Init();
void				GetBoundingBox(glm::vec3& min, glm::vec3& max, const aiScene* scene);
void				GetBoundingBoxForNode(const aiNode* node, glm::vec3& min, glm::vec3& max, aiMatrix4x4& trafo, const aiScene* scene);
Material			LoadMaterials(const aiScene* scene, aiMaterial* materials);
GLuint   			LoadTextures(StaticMesh& mesh, const aiScene* scene, aiMaterial* material);
void				DestroyMesh(StaticMesh& mesh);


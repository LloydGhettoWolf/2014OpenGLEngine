//TeapotApp.h
#pragma once
#include "MainApp.h"
#include "GBufferData.h"

struct TeapotStandardShaderUniforms{
	GLint cameraMatrixUniform;
	GLint perspectiveMatrixUniform;
	GLint textureUniform;
	GLint rotationUniform;
	GLint eyePosUniform;
	GLint lightVecUniform;
	GLint lightColUniform;
	GLint normalMatrixUniform;
	MaterialUniforms matUni;
	GLint scaleUniform;
	GLint instancedUniform;
};

struct GBufferShaderUniforms{
	GLint  perspectiveMatrixUniform;
	GLint  rotationMatrixUniform;
	GLint  scaleMatrixUniform;
	GLint  normalMatrixUniform;
	GLint  cameraMatrixUniform;
	GLint  instancedUniform;
};

struct LightPassShaderUniforms{
	GLint posTextureUniform;
	GLint normTextureUniform;
	GLint ambTextureUniform;
	GLint diffTextureUniform;
	GLint screenSizeUniform;
	GLint lightPosUniform;
	GLint lightColUniform;
	GLint wvpMatrixUniform;
};

struct QuadPassShaderUniforms{
	GLint ambTextureUniform;
	GLint screenSizeUniform;
};

class TeapotApp : public App{
public:
	bool Init();
	void Run();
	void ShutDown();

private:
	GLuint CreateInstancedLightingShader();
	GLuint CreateLightingShader();
	GLuint CreateGBufferShader();
	GLuint CreateLightPassShader();
	GLuint CreateLightPassQuadShader();
	bool   CreateGBuffer();

	void RenderForward(const vec3* lightPositions, const vec3* teapotPositions);
	void RenderDeferred(const vec3* teapotPositions);

	float CalcSphereDistance(const PointLightData& pLight, int index);

	GLuint		          m_teapotShader;
	GLuint				  m_groundShader;
	GLuint				  m_gBufferShader;
	GLuint				  m_lightPassShader;
	GLuint				  m_quadPassShader;

	StaticMesh            m_teapotMesh;
	StaticMesh			  m_sphereMesh;

	GLuint				  m_groundPlaneBuffer;

	TeapotStandardShaderUniforms teapotUniforms;
	GBufferShaderUniforms		 gBufferUniforms;
	MaterialUniforms			 g_bufferMaterials;
	LightPassShaderUniforms		 lightPassUniforms;
	QuadPassShaderUniforms		 quadPassUniforms;

	GBufferData		m_gBuffer;
	PointLightData  m_lights;
	vec3            lightPos[5];

	//full screen quad
	GLuint m_quadBuffer;
};


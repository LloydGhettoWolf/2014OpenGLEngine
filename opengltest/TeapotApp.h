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
	GLint  materialDiffuseUniform;
	GLint  materialSpecularUniform;
	GLint  materialAmbientUniform;
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
	bool   CreateGBuffer();

	void RenderForward(vec3* lightPositions, vec3* teapotPositions);
	void RenderDeferred(vec3* lightPositions, vec3* teapotPositions);

	GLuint		          m_teapotShader;
	GLuint				  m_groundShader;
	GLuint				  m_gBufferShader;
	StaticMesh            m_teapotMesh;

	GLuint				  m_groundPlaneBuffer;

	TeapotStandardShaderUniforms teapotUniforms;
	GBufferShaderUniforms		 gBufferUniforms;

	GBufferData m_gBuffer;
};


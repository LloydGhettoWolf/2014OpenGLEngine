//DeferredShader.h
#pragma once
#include "LightsAndMaterials.h"
#include "Shader.h"

struct GBufferShaderUniforms{
	GLint  perspectiveMatrixUniform;
	GLint  rotationMatrixUniform;
	GLint  scaleMatrixUniform;
	GLint  normalMatrixUniform;
	GLint  cameraMatrixUniform;
	GLint  instancedUniform;
	MaterialUniforms materialUniforms;
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

class DeferredShader{

private:
	static GLuint m_gBufferHandle,m_lightPassHandle,m_quadPassHandle;
	GBufferShaderUniforms   m_gBufferUniforms;
	LightPassShaderUniforms m_lightPassUniforms;
	QuadPassShaderUniforms  m_quadPassUniforms;

	bool CreateGBufferShader();
	bool CreateLightPassShader();
	bool CreateQuadPassShader();
public:
	DeferredShader(){};
	bool CreateDeferredShader();
	static GLuint GetGBufferHandle()  { return m_gBufferHandle; }
	static GLuint GetLightPassHandle(){ return m_lightPassHandle; }
	static GLuint GetQuadPassHandle() { return m_quadPassHandle; }
	const GBufferShaderUniforms&   GetGBufferUniforms()const   { return m_gBufferUniforms; }
	const LightPassShaderUniforms& GetLightPassUniforms()const { return m_lightPassUniforms; }
	const QuadPassShaderUniforms&  GetQuadPassUniforms()const  { return m_quadPassUniforms; }
};
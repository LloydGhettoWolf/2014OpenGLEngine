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
	GLint  textureUniform;
	GLint  normalMapUniform;
	GLint  specularMapUniform;
	GLint  alphaMapUniform;
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
	GLint eyePosUniform;
};

struct QuadPassShaderUniforms{
	GLint ambTextureUniform;
	GLint bufferUniform;
	GLint screenSizeUniform;
};

class DeferredShader{

private:
	static GLuint m_gBufferHandle,m_lightPassHandle,m_quadPassHandle,m_nullPassHandle;
	GBufferShaderUniforms   m_gBufferUniforms;
	LightPassShaderUniforms m_lightPassUniforms;
	QuadPassShaderUniforms  m_quadPassUniforms;

	GLint					m_nullWVPMatrixUniform;

	bool CreateGBufferShader();
	bool CreateLightPassShader();
	bool CreateQuadPassShader();
	bool CreateNULLShader();
public:
	DeferredShader(){};
	~DeferredShader();
	bool CreateDeferredShader();
	static GLuint GetGBufferHandle()  { return m_gBufferHandle; }
	static GLuint GetLightPassHandle(){ return m_lightPassHandle; }
	static GLuint GetQuadPassHandle() { return m_quadPassHandle; }
	static GLuint GetNULLPassHandle() { return m_nullPassHandle; }
	const GBufferShaderUniforms&   GetGBufferUniforms()const   { return m_gBufferUniforms; }
	const LightPassShaderUniforms& GetLightPassUniforms()const { return m_lightPassUniforms; }
	const QuadPassShaderUniforms&  GetQuadPassUniforms()const  { return m_quadPassUniforms; }
	GLint GetNULLWVPMatrix()const { return m_nullWVPMatrixUniform; }
};
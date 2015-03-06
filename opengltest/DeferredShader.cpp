//DeferredShader.cpp
#include "DeferredShader.h"

DeferredShader::~DeferredShader(){
	glDeleteShader(m_gBufferHandle);
	glDeleteShader(m_lightPassHandle);
	glDeleteShader(m_nullPassHandle);
	glDeleteShader(m_quadPassHandle);
}

bool DeferredShader::CreateGBufferShader(){
	const int numAttribs = 6;
	const char* attribs[numAttribs] = { "inCoords", "inNormals","inTangents","inBitangents","inUVs","inPositions" };
	m_gBufferHandle = CreateShader("gBuffer.vp", "gBuffer.fp", attribs, numAttribs);

	if (!m_gBufferHandle) return false;
	
	m_gBufferUniforms.cameraMatrixUniform   = glGetUniformLocation(m_gBufferHandle, "vpMatrix");
	m_gBufferUniforms.normalMatrixUniform   = glGetUniformLocation(m_gBufferHandle, "normalMatrix");
	m_gBufferUniforms.rotationMatrixUniform = glGetUniformLocation(m_gBufferHandle, "rotationMatrix");
	m_gBufferUniforms.scaleMatrixUniform    = glGetUniformLocation(m_gBufferHandle, "scaleMatrix");

	m_gBufferUniforms.textureUniform		= glGetUniformLocation(m_gBufferHandle, "myTexture");


	return true;
}

bool DeferredShader::CreateLightPassShader(){
	const int numAttribs = 1;
	const char* attribs[numAttribs] = { "inCoords" };
	m_lightPassHandle = CreateShader("lightPass.vp", "lightPass.fp", attribs, numAttribs);

	if (!m_lightPassHandle) return false;

	m_lightPassUniforms.diffTextureUniform = glGetUniformLocation(m_lightPassHandle, "diffuse");
	m_lightPassUniforms.normTextureUniform = glGetUniformLocation(m_lightPassHandle, "normal");
	m_lightPassUniforms.posTextureUniform  = glGetUniformLocation(m_lightPassHandle, "position");
	m_lightPassUniforms.screenSizeUniform  = glGetUniformLocation(m_lightPassHandle, "screenSize");
	m_lightPassUniforms.lightPosUniform    = glGetUniformLocation(m_lightPassHandle, "lightPos");
	m_lightPassUniforms.lightColUniform    = glGetUniformLocation(m_lightPassHandle, "lightColor");
	m_lightPassUniforms.wvpMatrixUniform   = glGetUniformLocation(m_lightPassHandle, "mvpMatrix");
	m_lightPassUniforms.eyePosUniform	   = glGetUniformLocation(m_lightPassHandle, "eyePos");

	return true;
}

bool DeferredShader::CreateQuadPassShader(){

	const char* attribs[1] = { "inCoords" };
	m_quadPassHandle = CreateShader("quadPass.vp", "quadPass.fp", attribs, 1);

	if (!m_quadPassHandle) return false;

	m_quadPassUniforms.ambTextureUniform = glGetUniformLocation(m_quadPassHandle, "ambient");
	m_quadPassUniforms.screenSizeUniform = glGetUniformLocation(m_quadPassHandle, "screenSize");
	m_quadPassUniforms.bufferUniform     = glGetUniformLocation(m_quadPassHandle, "buff");

	return true;
}

bool DeferredShader::CreateNULLShader(){
	const char* attribs[1] = { "inCoords" };
	m_nullPassHandle = CreateShader("nullTechnique.vp", "nullTechnique.fp", attribs, 1);

	if (!m_nullPassHandle) return false;

	m_nullWVPMatrixUniform = glGetUniformLocation(m_nullPassHandle,"WVPMatrix");

	return true;
}

bool DeferredShader::CreateDeferredShader(){

	if (!CreateQuadPassShader())
		return false;

	if (!CreateGBufferShader())
		return false;

	if (!CreateLightPassShader())
		return false;

	if (!CreateNULLShader())
		return false;

	return true;
}

GLuint DeferredShader::m_gBufferHandle   = 0;
GLuint DeferredShader::m_lightPassHandle = 0;
GLuint DeferredShader::m_quadPassHandle  = 0;
GLuint DeferredShader::m_nullPassHandle = 0;
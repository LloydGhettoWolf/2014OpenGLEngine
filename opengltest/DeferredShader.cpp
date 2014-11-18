//DeferredShader.cpp
#include "DeferredShader.h"

bool DeferredShader::CreateGBufferShader(){
	const int numAttribs = 4;
	const char* attribs[numAttribs] = { "inCoords", "inNormals", "inUVs", "inPositions" };
	m_quadPassHandle = CreateShader("gBuffer.vp", "gBuffer.fp", attribs, numAttribs);

	if (!m_quadPassHandle) return false;
	
	m_gBufferUniforms.cameraMatrixUniform   = glGetUniformLocation(m_quadPassHandle, "vpMatrix");
	m_gBufferUniforms.normalMatrixUniform   = glGetUniformLocation(m_quadPassHandle, "normalMatrix");
	m_gBufferUniforms.rotationMatrixUniform = glGetUniformLocation(m_quadPassHandle, "rotationMatrix");
	m_gBufferUniforms.scaleMatrixUniform    = glGetUniformLocation(m_quadPassHandle, "scaleMatrix");
	m_gBufferUniforms.instancedUniform      = glGetUniformLocation(m_quadPassHandle, "instanced");

	m_gBufferUniforms.materialUniforms.diffuseUniform  = glGetUniformLocation(m_quadPassHandle, "materialDiffuse");
	m_gBufferUniforms.materialUniforms.ambientUniform  = glGetUniformLocation(m_quadPassHandle, "materialAmbient");
	m_gBufferUniforms.materialUniforms.specularUniform = glGetUniformLocation(m_quadPassHandle, "materialSpecular");

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

	return true;
}

bool DeferredShader::CreateQuadPassShader(){

	const char* attribs[1] = { "inCoords" };
	m_quadPassHandle = CreateShader("quadPass.vp", "quadPass.fp", attribs, 1);

	if (!m_quadPassHandle) return false;

	m_quadPassUniforms.ambTextureUniform = glGetUniformLocation(m_quadPassHandle, "ambient");
	m_quadPassUniforms.screenSizeUniform = glGetUniformLocation(m_quadPassHandle, "screenSize");

	return true;
}

bool DeferredShader::CreateDeferredShader(){

	if (!CreateGBufferShader())
		return false;

	if (!CreateLightPassShader())
		return false;

	if (!CreateQuadPassShader())
		return false;

	return true;
}

GLuint DeferredShader::m_gBufferHandle   = 0;
GLuint DeferredShader::m_lightPassHandle = 0;
GLuint DeferredShader::m_quadPassHandle  = 0;
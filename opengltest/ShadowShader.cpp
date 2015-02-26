//ShadowShader.cpp
#include "ShadowShader.h"
#include <gl\glew.h>

ShadowShader::~ShadowShader(){
	glDeleteShader(m_depthHandle);
	glDeleteShader(m_shadowHandle);
}

bool ShadowShader::CreateShadowShader(){

	const int numAttribsDepth = 1;
	const char* attribsdepth[numAttribsDepth] = { "inCoords"};

	m_depthHandle = CreateShader("depth.vp", "depth.fp", attribsdepth, numAttribsDepth);

	if (!m_depthHandle)
		return false;

	m_depthUniforms.cameraMatrixUniform			= glGetUniformLocation(m_depthHandle, "cameraMatrix");
	m_depthUniforms.perspectiveMatrixUniform	= glGetUniformLocation(m_depthHandle, "perspectiveMatrix");
	m_depthUniforms.worldMatrixUniform		    = glGetUniformLocation(m_depthHandle, "worldMatrix");


	const int numAttribs = 2;
	const char* attribs[numAttribs] = { "inCoords", "inNormals" };

	m_shadowHandle = CreateShader("shadow.vp", "shadow.fp", attribs, numAttribs);


	if (!m_shadowHandle)
		return false;

	m_shadowUniforms.cameraMatrixUniform		= glGetUniformLocation(m_shadowHandle, "cameraMatrix");
	m_shadowUniforms.perspectiveMatrixUniform	= glGetUniformLocation(m_shadowHandle, "perspectiveMatrix");
	m_shadowUniforms.worldMatrixUniform			= glGetUniformLocation(m_shadowHandle, "worldMatrix");
	m_shadowUniforms.eyePosUniform				= glGetUniformLocation(m_shadowHandle, "eyePos");
	m_shadowUniforms.lightPosUniform			= glGetUniformLocation(m_shadowHandle, "lightPos");
	m_shadowUniforms.normalMatrixUniform		= glGetUniformLocation(m_shadowHandle, "normalMatrix");
	m_shadowUniforms.matUni.diffuseUniform		= glGetUniformLocation(m_shadowHandle, "materialDiffuse");
	m_shadowUniforms.matUni.ambientUniform		= glGetUniformLocation(m_shadowHandle, "materialAmbient");
	m_shadowUniforms.matUni.specularUniform		= glGetUniformLocation(m_shadowHandle, "materialSpecular");
	m_shadowUniforms.matUni.shininessUniform	= glGetUniformLocation(m_shadowHandle, "shininess");
	m_shadowUniforms.shadowMatrixUniform		= glGetUniformLocation(m_shadowHandle, "shadowMatrix");
	m_shadowUniforms.shadowTextureUniform		= glGetUniformLocation(m_shadowHandle, "shadowTexture");
	return true;
}

void ShadowShader::SetShadowUniforms(const mat4& projMatrix){
	glUseProgram(m_shadowHandle);
	glUniformMatrix4fv(m_shadowUniforms.perspectiveMatrixUniform, 1, GL_FALSE, &projMatrix[0][0]);
	glUseProgram(0);
}

void ShadowShader::SetDepthUniforms(const mat4& projMatrix){
	glUseProgram(m_depthHandle);
	glUniformMatrix4fv(m_depthUniforms.perspectiveMatrixUniform, 1, GL_FALSE, &projMatrix[0][0]);
	glUseProgram(0);
}

void ShadowShader::UpdateShadowUniforms(const mat4& worldMatrix, const mat3& normalMatrix, const mat4& viewMatrix,
	const vec3& pos, const mat4& shadowMatrix,const vec3& lightPos){
	glUniformMatrix4fv(m_shadowUniforms.worldMatrixUniform, 1, GL_FALSE, &worldMatrix[0][0]);
	glUniformMatrix3fv(m_shadowUniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(m_shadowUniforms.cameraMatrixUniform, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniform3fv(m_shadowUniforms.eyePosUniform, 1, &pos[0]);
	glUniformMatrix4fv(m_shadowUniforms.shadowMatrixUniform, 1, GL_FALSE, &shadowMatrix[0][0]);
	glUniform3fv(m_shadowUniforms.lightPosUniform, 1, &lightPos[0]);
}

void ShadowShader::UpdateDepthUniforms(const mat4& worldMatrix, const mat4& viewMatrix){
	glUniformMatrix4fv(m_depthUniforms.worldMatrixUniform, 1, GL_FALSE, &worldMatrix[0][0]);
	glUniformMatrix4fv(m_depthUniforms.cameraMatrixUniform, 1, GL_FALSE, &viewMatrix[0][0]);

}

GLuint ShadowShader::m_shadowHandle = 0;
GLuint ShadowShader::m_depthHandle  = 0;
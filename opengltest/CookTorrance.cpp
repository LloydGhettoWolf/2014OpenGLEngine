//CookTorrance.cpp
#include "CookTorrance.h"
#include <gl\glew.h>

bool CookTorranceShader::CreateCookTorranceShader(){

	const int numAttribs = 2;
	const char* attribs[numAttribs] = { "inCoords", "inNormals" };

	m_handle = CreateShader("cookTorrance.vp", "cookTorrance.fp", attribs, numAttribs);


	if (!m_handle)
		return false;

	m_uniforms.cameraMatrixUniform		= glGetUniformLocation(m_handle, "cameraMatrix");
	m_uniforms.perspectiveMatrixUniform = glGetUniformLocation(m_handle, "perspectiveMatrix");
	m_uniforms.worldMatrixUniform		= glGetUniformLocation(m_handle, "worldMatrix");
	m_uniforms.eyePosUniform			= glGetUniformLocation(m_handle, "eyePos");
	m_uniforms.lightDirUniform			= glGetUniformLocation(m_handle, "lightDir");
	m_uniforms.normalMatrixUniform		= glGetUniformLocation(m_handle, "normalMatrix");

	m_uniforms.roughnessUniform			= glGetUniformLocation(m_handle, "roughness");
	m_uniforms.fresnelUniform			= glGetUniformLocation(m_handle, "fresnel");
	m_uniforms.kUniform					= glGetUniformLocation(m_handle, "k");
	
	return true;
}

void CookTorranceShader::SetUniforms(const mat4& projMatrix){
	glUseProgram(m_handle);
	glUniformMatrix4fv(m_uniforms.perspectiveMatrixUniform, 1, GL_FALSE, &projMatrix[0][0]);
	glUseProgram(0);
}

void CookTorranceShader::UpdateUniforms(const mat4& worldMatrix, const mat3& normalMatrix, const mat4& viewMatrix,
	const vec3& pos, const vec3& lightDir){
	glUniformMatrix4fv(m_uniforms.worldMatrixUniform, 1, GL_FALSE, &worldMatrix[0][0]);
	glUniformMatrix3fv(m_uniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(m_uniforms.cameraMatrixUniform, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniform3fv(m_uniforms.eyePosUniform, 1, &pos[0]);
	glUniform3fv(m_uniforms.lightDirUniform, 1, &lightDir[0]);
}

GLuint CookTorranceShader::m_handle = 0;
//ForwardShader.cpp
#include "ForwardShader.h"
#include <gl\glew.h>

bool ForwardShader::CreateForwardShader(bool withUVs){

	if (withUVs){
		const int numAttribs = 4;
		const char* attribs[numAttribs] = { "inCoords", "inNormals", "inUVs", "inPositions" };

		m_handle = CreateShader("instancedLighting.vp", "instancedLighting.fp", attribs, numAttribs);
	}
	else{
		const int numAttribs = 3;
		const char* attribs[numAttribs] = { "inCoords", "inNormals", "inPositions" };

		m_handle = CreateShader("instancedLighting.vp", "instancedLighting.fp", attribs, numAttribs);
	}

	if (!m_handle)
		return false;

	m_uniforms.cameraMatrixUniform		= glGetUniformLocation(m_handle, "cameraMatrix");
	m_uniforms.perspectiveMatrixUniform = glGetUniformLocation(m_handle, "perspectiveMatrix");
	m_uniforms.textureUniform			= glGetUniformLocation(m_handle, "tex");
	m_uniforms.worldMatrixUniform		= glGetUniformLocation(m_handle, "worldMatrix");
	m_uniforms.eyePosUniform			= glGetUniformLocation(m_handle, "eyePos");
	m_uniforms.lightVecUniform			= glGetUniformLocation(m_handle, "lightPos");
	m_uniforms.lightColUniform			= glGetUniformLocation(m_handle, "lightColors");
	m_uniforms.normalMatrixUniform		= glGetUniformLocation(m_handle, "normalMatrix");
	m_uniforms.matUni.diffuseUniform	= glGetUniformLocation(m_handle, "materialDiffuse");
	m_uniforms.matUni.ambientUniform	= glGetUniformLocation(m_handle, "materialAmbient");
	m_uniforms.matUni.specularUniform   = glGetUniformLocation(m_handle, "materialSpecular");
	m_uniforms.matUni.shininessUniform	= glGetUniformLocation(m_handle, "shininess");

	return true;
}

void ForwardShader::SetUniforms(const mat4& projMatrix,const vec3& lightColors,int numLights){
	glUseProgram(m_handle);
		glUniformMatrix4fv(m_uniforms.perspectiveMatrixUniform, 1, GL_FALSE, &projMatrix[0][0]);
		glUniform3fv(m_uniforms.lightColUniform, numLights, &lightColors[0]);
	glUseProgram(0);
}

void ForwardShader::UpdateUniforms(const mat4& worldMatrix,const mat3& normalMatrix,const mat4& viewMatrix,
									const vec3& pos, const vec3& lightPos, int numLights){
	glUniformMatrix4fv(m_uniforms.worldMatrixUniform,  1, GL_FALSE, &worldMatrix[0][0]);
	glUniformMatrix3fv(m_uniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(m_uniforms.cameraMatrixUniform, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniform3fv(m_uniforms.eyePosUniform, 1, &pos[0]);
	glUniform3fv(m_uniforms.lightVecUniform, numLights, &lightPos[0]);
}

GLuint ForwardShader::m_handle = 0;
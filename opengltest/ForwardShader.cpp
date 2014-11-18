//ForwardShader.cpp
#include "ForwardShader.h"
#include <gl\glew.h>

bool ForwardShader::CreateForwardShader(){

	const int numAttribs = 4;
	const char* attribs[numAttribs] = { "inCoords", "inNormals", "inUVs", "inPositions" };

	m_handle = CreateShader("instancedLighting.vp", "instancedLighting.fp", attribs, numAttribs);

	if (!m_handle)
		return false;

	m_uniforms.cameraMatrixUniform		= glGetUniformLocation(m_handle, "cameraMatrix");
	m_uniforms.perspectiveMatrixUniform = glGetUniformLocation(m_handle, "perspectiveMatrix");
	m_uniforms.textureUniform			= glGetUniformLocation(m_handle, "tex");
	m_uniforms.rotationUniform			= glGetUniformLocation(m_handle, "rotationMatrix");
	m_uniforms.eyePosUniform			= glGetUniformLocation(m_handle, "eyePos");
	m_uniforms.lightVecUniform			= glGetUniformLocation(m_handle, "lightPos");
	m_uniforms.lightColUniform			= glGetUniformLocation(m_handle, "lightColors");
	m_uniforms.normalMatrixUniform		= glGetUniformLocation(m_handle, "normalMatrix");
	m_uniforms.matUni.diffuseUniform	= glGetUniformLocation(m_handle, "materialDiffuse");
	m_uniforms.matUni.ambientUniform	= glGetUniformLocation(m_handle, "materialAmbient");
	m_uniforms.matUni.specularUniform   = glGetUniformLocation(m_handle, "materialSpecular");
	m_uniforms.matUni.shininessUniform	= glGetUniformLocation(m_handle, "shininess");
	m_uniforms.scaleUniform				= glGetUniformLocation(m_handle, "scaleMatrix");
	m_uniforms.instancedUniform			= glGetUniformLocation(m_handle, "instanced");

	return true;
}

GLuint ForwardShader::m_handle = 0;
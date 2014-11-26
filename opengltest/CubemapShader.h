//cubemap shader
#pragma once
#include "Shader.h"

class CubemapShader{

private:
	static GLuint m_cubemapHandle;

	GLint	m_WVPMatrixUniform;
	GLint   m_samplerUniform;

public:
	CubemapShader(){};
	bool CreateCubemapShader();
	GLint GetWVPMatrix()const { return m_WVPMatrixUniform; }
	GLint GetSampler()const { return m_WVPMatrixUniform; }

	static GLuint GetHandle() { return m_cubemapHandle; }
};
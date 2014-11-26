//CubemapShader.cpp
#include "CubemapShader.h"

bool CubemapShader::CreateCubemapShader(){
	const int numAttribs = 1;
	const char* attribs[numAttribs] = { "inCoords"};
	m_cubemapHandle = CreateShader("cubeMap.vp", "cubeMap.fp", attribs, numAttribs);

	if (!m_cubemapHandle) return false;

	m_WVPMatrixUniform = glGetUniformLocation(m_cubemapHandle, "wvpMatrix");
	m_samplerUniform   = glGetUniformLocation(m_cubemapHandle, "cubeSampler");

	return true;
}

GLuint CubemapShader::m_cubemapHandle = 0;
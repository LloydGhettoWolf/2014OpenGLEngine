//DepthBuffer.h
#pragma once
#include <gl\glew.h>

bool CreateDepthTextureComplex(GLuint& depthFBO,bool bindBuffer = true,bool stencil = false);
bool CreateDepthTextureBasic(GLuint& depthFBO, GLuint& depthTexture);
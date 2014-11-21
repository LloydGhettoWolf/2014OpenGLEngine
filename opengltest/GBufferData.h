//GbufferData.h
#pragma once
#include <gl\glew.h>

const int NUM_MRT = 4;


struct GBufferData{
	GLuint  fboObject;
	GLuint  textures[NUM_MRT];
	GLuint  depthTexture;
	GLuint  finalTexture;
};

bool CreateGBufferData(GBufferData& data);
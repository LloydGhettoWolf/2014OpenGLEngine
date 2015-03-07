//GbufferData.h
#pragma once
#include <gl\glew.h>

const int NUM_MRT = 3;


struct GBufferData{
	GLuint  fboObject;
	GLuint  textures[NUM_MRT];
	GLuint  depthTexture;
	GLuint  finalTexture;

	~GBufferData();
};

bool CreateGBufferData(GBufferData& data);
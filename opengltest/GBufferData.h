//GbufferData.h
#pragma once
#include <gl\glew.h>


enum BufferIndexes{POSITION,DIFFUSE,NORMAL};

struct GBufferData{
	GLuint  fboObject;
	GLuint  textures[3];
	GLuint  depthTexture;
};

bool CreateGBufferData(GBufferData& data);
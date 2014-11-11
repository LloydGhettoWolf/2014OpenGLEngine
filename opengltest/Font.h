//Font.h
#pragma once
#include <GL/glew.h>

struct FontData{
	GLuint Text2DTextureID;
	GLuint Text2DVertexBufferID;
	GLuint Text2DUVBufferID;
	GLuint Text2DShaderID;
	GLuint Text2DUniformID;
	GLuint halfVecUniformID;
	GLuint fontVAO;
	int    numVertices = 0;
};

void InitText2D(FontData& fData, const char * texturePath, const char * text, int x, int y, 
				int size,GLuint fontShaderID);
void UpdateCounter(FontData& fData, const char* text);
void PrintText2D(FontData& fData);
void ChangeText2D(FontData& fData, const char * text, int x, int y, int size);
void CleanupText2D(FontData& fData);
GLuint CreateFontShader();


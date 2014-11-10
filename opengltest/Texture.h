//Texture.h
#pragma once
#include <gl\glew.h>
#include <stdio.h>

GLuint CreateTexture(const char*  fileName, GLint magFilter, GLint minFilter);
GLuint CreateFontShader();
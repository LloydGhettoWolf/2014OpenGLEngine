//Texture.h
#pragma once
#include <gl\glew.h>
#include <stdio.h>
#include <IL\il.h>

GLuint CreateTexture(const char*  fileName, GLint internalFormat, GLint magFilter, GLint minFilter);
GLuint CreateFontShader();
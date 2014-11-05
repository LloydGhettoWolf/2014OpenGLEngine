//Texture.h
#pragma once
#include <gl\glew.h>
#include <stdio.h>
#include <IL\il.h>

GLuint CreateTexture(char*  fileName, GLint internalFormat, GLint magFilter, GLint minFilter);

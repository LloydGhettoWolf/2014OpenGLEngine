//Texture.h
#pragma once
#include <gl\glew.h>
#include <stdio.h>
#include <string>

GLuint CreateTexture(const std::string&  fileName, GLint magFilter, GLint minFilter);
GLuint CreateFontShader();
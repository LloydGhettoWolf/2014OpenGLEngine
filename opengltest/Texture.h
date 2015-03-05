//Texture.h
#pragma once
#include <gl\glew.h>
#include <stdio.h>
#include <string>

GLuint CreateTexture(const std::string&  fileName, GLint magFilter, GLint minFilter);
GLubyte* ReadImage(const std::string& fileName, unsigned int& bytes, unsigned int& width, unsigned int& height,unsigned int& format);
GLuint CreateFontShader();
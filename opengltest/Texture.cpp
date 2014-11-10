//Texture.cpp
#include "Texture.h"
#include <string>

GLubyte* ReadImage(const char* fileName,unsigned int& internalFormat, unsigned int& width, unsigned int& height);

GLuint CreateTexture(const char* fileName,GLint magFilter,GLint minFilter){
	GLuint texHandle;
	glGenTextures(1, &texHandle);

	glBindTexture(GL_TEXTURE_2D, texHandle);

	unsigned int width = 0, height = 0,internalFormat = 0;

	GLubyte* data = 0;

	data = ReadImage(fileName,internalFormat, width, height);

	if (!data) return 0;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, 
				 GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] data;

	return texHandle;
}

GLubyte* ReadImage(const char* fileName,unsigned int& internalFormat,unsigned int& width, unsigned int& height){
	
	if (!ilLoadImage(fileName)){
		return 0;
	}

	width  = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	unsigned int bytes  = ilGetInteger(IL_IMAGE_BPP);

	internalFormat = ilGetInteger(IL_IMAGE_FORMAT);

	const unsigned char* Pixels = ilGetData();

	GLubyte* img = new GLubyte[width * height * bytes];
	memcpy(img, Pixels, width * height * bytes);

	return img;
}
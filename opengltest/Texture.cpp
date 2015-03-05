//Texture.cpp
#include "Texture.h"
#include <IL\il.h>
#include <IL\ilu.h>

using namespace std;


GLuint CreateTexture(const string& fileName,GLint magFilter,GLint minFilter){

	GLuint newTex;
	glGenTextures(1, &newTex);
	glBindTexture(GL_TEXTURE_2D, newTex);


	unsigned int width, height, bytes,format;
	GLubyte* data = ReadImage(fileName,bytes, width, height,format);

	switch (bytes)
	{
		case 1:
			format = GL_LUMINANCE;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
	}

	GLenum type;
	glGetInternalformativ(GL_TEXTURE_2D, format, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete[] data;

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glBindTexture(GL_TEXTURE_2D, 0);

	return newTex;
}

GLubyte* ReadImage(const string& fileName,unsigned int& bytes,unsigned int& width, unsigned int& height,unsigned int& format){
	
	// Generate a new texture like in opengl
	ILuint ilTexture;
	ilGenImages(1, &ilTexture);
	ilBindImage(ilTexture);

	ilLoadImage(fileName.c_str());
	
	//get image info - width height and bytes per pixel
	width  = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	bytes  = ilGetInteger(IL_IMAGE_BPP);
	format = ilGetInteger(IL_IMAGE_FORMAT);

	//if the image has an alpha channel...
	if (format == IL_BGR){
		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	}else if (format == IL_BGRA){
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	}

	//get the image info and copy over to the img pointer to return to the app
	const unsigned char* Pixels = ilGetData();

	GLubyte* img = new GLubyte[width * height * bytes];
	memcpy(img, Pixels, width * height * bytes);

	//now unbind and delete orig info....
	ilBindImage(0);
	ilDeleteImages(1, &ilTexture);

	return img;
}
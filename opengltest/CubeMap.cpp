//CubeMap.cpp
#include "CubeMap.h"
#include "Texture.h"

GLint mapFaces[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
					 GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
					 GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

GLuint CreateCubeMap(const std::vector<std::string>& fileNames){
	GLuint cubeMap;

	glGenTextures(1, &cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	for (int face = 0; face < 6; face++){
		unsigned int width, height, bytes,format;
		GLubyte* data = ReadImage(fileNames[face], bytes, width, height,format);

		switch (bytes)
		{
		case 1:
			format = GL_LUMINANCE;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		GLenum type;
		glGetInternalformativ(GL_TEXTURE_2D, GL_RGB, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);
		glTexImage2D(mapFaces[face], 0, GL_RGB, width, height, 0, format, type, data);

		delete[] data;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cubeMap;
}
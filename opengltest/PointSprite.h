//PontSprite.h
#pragma once
#include <gl\glew.h>
#include <string>
#include <glm\glm.hpp>

struct PointSprite{
	GLuint VAO;
	GLuint posBuffer;
	GLuint shader;
	GLuint texture;
	GLint  matrixUniform;
	GLint  textureUniform;
	GLint  camPos;
	int num;
};

bool SetupPointSprite(PointSprite& pSprite, void* positions,const std::string& fileName,int num);
void DrawPointSprites(PointSprite& pSprite);
void UpdatePointSpritePosition(PointSprite& pSprite, void* positions);
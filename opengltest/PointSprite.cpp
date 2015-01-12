//PointSprite.cpp
#include "PointSprite.h"
#include "Texture.h"
#include "Shader.h"

void UpdatePointSpritePosition(PointSprite& pSprite, void* positions);

bool SetupPointSprite(PointSprite& pSprite,void* positions, const std::string& fileName, int num){
	glGenVertexArrays(1, &pSprite.VAO);
	glBindVertexArray(pSprite.VAO);
	glGenBuffers(1, &pSprite.posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pSprite.posBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * num, positions, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	pSprite.texture = CreateTexture(fileName, GL_NEAREST, GL_LINEAR);
	pSprite.num = num;
	const int numAttribs = 1;
	const char* attribs[numAttribs] = { "inCoords" };
	pSprite.shader = CreateShader("pointSprite.vp", "pointSprite.fp", "pointSprite.gp", attribs, 1);

	if (pSprite.shader == 0) return false;

	pSprite.matrixUniform  = glGetUniformLocation(pSprite.shader, "viewProjectionMatrix");
	pSprite.textureUniform = glGetUniformLocation(pSprite.shader, "texture");
	pSprite.camPos		   = glGetUniformLocation(pSprite.shader, "cameraPos");
	return true;
}

void UpdatePointSpritePosition(PointSprite& pSprite, void* positions){
	glBindVertexArray(pSprite.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, pSprite.posBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*pSprite.num, positions);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawPointSprites(PointSprite& pSprite, void* positions,const vec3& eyePos,const mat4& projMatrix){
	glUniformMatrix4fv(pSprite.matrixUniform, 1, GL_FALSE, &projMatrix[0][0]);
	glUniform3fv(pSprite.camPos, 1, &eyePos[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pSprite.texture);
	UpdatePointSpritePosition(pSprite, positions);
	glBindVertexArray(pSprite.VAO);
	glDrawArrays(GL_POINTS, 0, pSprite.num);
	glBindVertexArray(0);
}
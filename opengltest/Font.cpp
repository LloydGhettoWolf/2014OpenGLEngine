//Font.cpp
#include "Font.h"

#include <vector>
#include <cstring>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"


using namespace glm;

GLuint Text2DTextureID;
GLuint Text2DVertexBufferID;
GLuint Text2DUVBufferID;
GLuint Text2DShaderID;
GLuint Text2DUniformID;
GLuint halfVecUniformID;
GLuint fontVAO;
int    numVertices = 0;

void InitText2D(const char * texturePath,const char* textMessage,
				int x, int y, int size){

	// Initialize texture
	Text2DTextureID = CreateTexture(texturePath, GL_RGB, GL_NEAREST, GL_NEAREST);

	// Initialize VBO
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);

	// Initialize Shader
	Text2DShaderID = CreateFontShader();

		
	// Initialize uniforms' IDs
	Text2DUniformID  = glGetUniformLocation(Text2DShaderID, "fontTexture");
	halfVecUniformID = glGetUniformLocation(Text2DShaderID, "halfScreenSize");

	unsigned int length = strlen(textMessage);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for (unsigned int i = 0; i<length; i++){

		glm::vec2 vertex_up_left = glm::vec2(x + i*size, y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i*size + size, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i*size + size, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i*size, y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = textMessage[i];
		float uv_x = (character % 8) / 8.0f;
		float uv_y = (character / 8) / 8.0f;

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 8.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 8.0f, (uv_y + 1.0f / 8.0f));
		glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 8.0f));
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}

	glGenVertexArrays(1, &fontVAO);
	glBindVertexArray(fontVAO);

	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindVertexArray(0);

	numVertices = vertices.size();
}

void PrintText2D(){

	glBindVertexArray(fontVAO);
	// Bind shader
	glUseProgram(Text2DShaderID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(Text2DUniformID, 0);

	const int APP_WIDTH = 1024;
	const int APP_HEIGHT = 768;
	vec2 halfScreenWidth(APP_WIDTH / 2, APP_HEIGHT / 2);
	glUniform2fv(halfVecUniformID, 1, &halfScreenWidth[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	glDisable(GL_BLEND);

	glBindVertexArray(0);

}

void CleanupText2D(){

	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	// Delete shader
	glDeleteProgram(Text2DShaderID);
}

GLuint CreateFontShader(){
	GLuint vertexShader;
	GLuint fragmentShader;

	const int numAttribs = 2;
	char* attribs[numAttribs] = { "inCoords", "inUVs"};

	vertexShader = CreateShader(GL_VERTEX_SHADER, "font.vp");
	fragmentShader = CreateShader(GL_FRAGMENT_SHADER, "font.fp");

	if (!vertexShader || !fragmentShader)
		return -1;

	return CreateShaderProgram(&vertexShader, 1, &fragmentShader, 1, 0, 0, numAttribs, &attribs[0]);
};
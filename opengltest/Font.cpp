//Font.cpp
#include "Font.h"

#include <vector>
#include <cstring>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VertexTypes.h"
#include "Shader.h"
#include "Texture.h"


using namespace glm;

void InitVertexArrays(FontData& fData, std::vector<CustomVertexHud>& vertices);
void UpdateVertices(FontData& fData, const char* textMessage,int x, int y, int size,
					std::vector<CustomVertexHud>& vertices);

void InitText2D(FontData& fData, const char * texturePath, const char* textMessage,
				int x, int y, int size,GLuint fontShaderID){

	// Initialize texture
	fData.Text2DTextureID = CreateTexture(texturePath, GL_NEAREST, GL_NEAREST);

	// Initialize VBO
	glGenBuffers(1, &fData.Text2DVertexBufferID);

	// Initialize Shader
	fData.Text2DShaderID = fontShaderID;

	// Initialize uniforms' IDs
	fData.Text2DUniformID = glGetUniformLocation(fData.Text2DShaderID, "fontTexture");
	fData.halfVecUniformID = glGetUniformLocation(fData.Text2DShaderID, "halfScreenSize");

	std::vector<CustomVertexHud> vertices;
	UpdateVertices(fData,textMessage,x,y,size,vertices);

	InitVertexArrays(fData, vertices);

	fData.numVertices = vertices.size();
}

void PrintText2D(FontData& fData){

	glBindVertexArray(fData.fontVAO);
	// Bind shader
	glUseProgram(fData.Text2DShaderID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fData.Text2DTextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(fData.Text2DUniformID, 0);

	const int APP_WIDTH = 1024;
	const int APP_HEIGHT = 768;
	vec2 halfScreenWidth(APP_WIDTH / 2, APP_HEIGHT / 2);
	glUniform2fv(fData.halfVecUniformID, 1, &halfScreenWidth[0]);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, fData.numVertices);

	glBindTexture(GL_TEXTURE_2D,0);
	glBindVertexArray(0);
}

void CleanupText2D(FontData& fData){

	// Delete buffers
	glDeleteBuffers(1, &fData.Text2DVertexBufferID);

	// Delete texture
	glDeleteTextures(1, &fData.Text2DTextureID);

	// Delete shader
	glDeleteProgram(fData.Text2DShaderID);
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

void InitVertexArrays(FontData& fData, std::vector<CustomVertexHud>& vertices){
	glGenVertexArrays(1, &fData.fontVAO);
	glBindVertexArray(fData.fontVAO);

	glBindBuffer(GL_ARRAY_BUFFER, fData.Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CustomVertexHud), &vertices[0], GL_DYNAMIC_DRAW);

	//1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CustomVertexHud), (void*)0);

	//2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CustomVertexHud), (void*)8);
	glBindVertexArray(0);
}

void UpdateVertexArrays(FontData& fData, std::vector<CustomVertexHud>& vertices){
	glBindVertexArray(fData.fontVAO);

	glBindBuffer(GL_ARRAY_BUFFER, fData.Text2DVertexBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(CustomVertexHud), &vertices[0]);

	glBindVertexArray(0);
}

void ChangeText2D(FontData& fData, const char * text, int x, int y, int size){
	std::vector<CustomVertexHud> vertices;
	UpdateVertices(fData, text, x, y, size, vertices);
	UpdateVertexArrays(fData,vertices);
}

void UpdateVertices(FontData& fData, const char* textMessage, int x, int y, int size,
						std::vector<CustomVertexHud>& vertices){
	unsigned int length = strlen(textMessage);

	// Fill buffers
	CustomVertexHud vertex;
	for (unsigned int i = 0; i<length; i++){

		char character = textMessage[i];
		float uv_x = ((character) % 16) / 16.0f;
		float uv_y = -((character+16) / 16) / 16.0f;

		//up left
		glm::vec2 upLeftVertex = glm::vec2(x + i*size, y + size);
		glm::vec2 upLeftUV = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
		//up right
		glm::vec2 upRightVertex = glm::vec2(x + i*size + size, y + size);
		glm::vec2 upRightUV = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		//down right
		glm::vec2 downRightVertex = glm::vec2(x + i*size + size, y);
		glm::vec2 downRightUV = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		//down left
		glm::vec2 downLeftVertex = glm::vec2(x + i*size, y);
		glm::vec2 downLeftUV = glm::vec2(uv_x, uv_y);

		vertex.vertexPoint = upLeftVertex;
		vertex.uv = upLeftUV;
		vertices.push_back(vertex);
		vertex.vertexPoint = downLeftVertex;
		vertex.uv = downLeftUV;
		vertices.push_back(vertex);
		vertex.vertexPoint = upRightVertex;
		vertex.uv = upRightUV;
		vertices.push_back(vertex);
		vertex.vertexPoint = downRightVertex;
		vertex.uv = downRightUV;
		vertices.push_back(vertex);
		vertex.vertexPoint = upRightVertex;
		vertex.uv = upRightUV;
		vertices.push_back(vertex);
		vertex.vertexPoint = downLeftVertex;
		vertex.uv = downLeftUV;
		vertices.push_back(vertex);
	}
}
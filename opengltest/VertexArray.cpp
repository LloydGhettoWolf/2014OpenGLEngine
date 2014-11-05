//VertexArray.cpp
#include "VertexArray.h"
#include "VertexTypes.h"
using namespace glm;

void AssignSubData(GLuint buffer, void* vertices, unsigned int vertexSize, unsigned int numPoints,
	GLintptr offset){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, offset, numPoints*vertexSize, vertices);
}

void CreateVertexArrayBuffer(void* vertexData,unsigned int elemSize, unsigned int numPoints){
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, elemSize * numPoints, vertexData, GL_STATIC_DRAW);
}

void CreateIndexArrayBuffer(unsigned int* indexData, unsigned int numPoints){
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numPoints, indexData, GL_STATIC_DRAW);
}

GLuint CreateVertexNormUVArray(void* vertices, unsigned int numPoints, 
								  unsigned int* indices,unsigned int numFaces){
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	int vertexSize = sizeof(CustomVertexNormUV);
	CreateVertexArrayBuffer(vertices,vertexSize,numPoints);
	CreateIndexArrayBuffer(indices, numFaces * 3);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)24);
	glBindVertexArray(0);
	return VAO;
}

GLuint CreateVertexUVArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	int vertexSize = sizeof(CustomVertexUV);
	CreateVertexArrayBuffer(vertices, vertexSize, numPoints);
	CreateIndexArrayBuffer(indices, numFaces * 3);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)12);
	glBindVertexArray(0);
	return VAO;
}

GLuint CreateVertexNormArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	int vertexSize = sizeof(CustomVertexNorm);
	CreateVertexArrayBuffer(vertices, vertexSize, numPoints);
	CreateIndexArrayBuffer(indices, numFaces * 3);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)12);
	glBindVertexArray(0);
	return VAO;
}

GLuint CreateSimpleVertexArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	int vertexSize = sizeof(glm::vec3);
	CreateVertexArrayBuffer(vertices, vertexSize, numPoints);
	CreateIndexArrayBuffer(indices, numFaces * 3);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)NULL);
	glBindVertexArray(0);
	return VAO;
}

GLuint CreateBumpmappedVertexArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	int vertexSize = sizeof(CustomVertexNormBiNormUV);
	CreateVertexArrayBuffer(vertices, vertexSize, numPoints);
	CreateIndexArrayBuffer(indices, numFaces * 3);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)24);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)36);
	glBindVertexArray(0);
	return VAO;
}
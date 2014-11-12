//VertexArray.cpp
#include "VertexArray.h"
#include "VertexTypes.h"
using namespace glm;

void CreateInstancedAttrib(unsigned int attribNum, GLuint VAO, GLuint& instancedData,int numInstances);
GLuint InitVAO(int vertexSize, void* vertices, int numPoints, int numFaces, unsigned int* indices);

void AssignSubData(GLuint buffer, void* vertices, unsigned int vertexSize, unsigned int numPoints,
	GLintptr offset){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, offset, numPoints*vertexSize, vertices);
}

GLuint CreateVertexArrayBuffer(void* vertexData,unsigned int elemSize, unsigned int numPoints,GLenum usage = GL_STATIC_DRAW){
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, elemSize * numPoints, vertexData, usage);
	return VBO;
}

void CreateIndexArrayBuffer(unsigned int* indexData, unsigned int numPoints){
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numPoints, indexData, GL_STATIC_DRAW);
}

GLuint CreateVertexNormUVArray(void* vertices, unsigned int numPoints, 
								  unsigned int* indices,unsigned int numFaces){
	int vertexSize = sizeof(CustomVertexNormUV);
	GLuint VAO = InitVAO(vertexSize,vertices,numPoints,numFaces,indices);
	for (int attrib = 0; attrib < 2; attrib++){
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)(attrib * 12));
	}
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)(24));
	return VAO;
}

GLuint CreateVertexUVArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	int vertexSize = sizeof(CustomVertexUV);
	GLuint VAO = InitVAO(vertexSize, vertices, numPoints, numFaces, indices);
	for (int attrib = 0; attrib < 2; attrib++){
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)(attrib * 12));
	}
	return VAO;
}

GLuint CreateVertexNormArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	int vertexSize = sizeof(CustomVertexNorm);
	GLuint VAO = InitVAO(vertexSize, vertices, numPoints, numFaces, indices);
	for (int attrib = 0; attrib < 2; attrib++){
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)(attrib * 12));
	}
	return VAO;
}

GLuint CreateSimpleVertexArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	int vertexSize = sizeof(glm::vec3);
	GLuint VAO = InitVAO(vertexSize, vertices, numPoints, numFaces, indices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)NULL);
	glBindVertexArray(0);
	return VAO;
}

GLuint CreateBumpmappedVertexArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces){
	int vertexSize = sizeof(CustomVertexNormBiNormUV);
	GLuint VAO = InitVAO(vertexSize, vertices, numPoints, numFaces, indices);
	for (int attrib = 0; attrib < 4; attrib++){
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (GLubyte*)(attrib*12));
	}
	glBindVertexArray(0);
	return VAO;
}

GLuint CreateInstancedVertexUVArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces,int numInstances,GLuint& instancedData){
	GLuint VAO = CreateVertexUVArray(vertices, numPoints, indices, numFaces);
	CreateInstancedAttrib(2, VAO, instancedData,numInstances);
	return VAO;
}

GLuint CreateInstancedVertexNormArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces, int numInstances, GLuint& instancedData){
	GLuint VAO = CreateVertexNormArray(vertices, numPoints, indices, numFaces);
	CreateInstancedAttrib(2, VAO,instancedData,numInstances);
	return VAO;
}

GLuint CreateInstancedVertexNormUVArray(void* vertices, unsigned int numPoints,
	unsigned int* indices, unsigned int numFaces, int numInstances,GLuint& instancedData){
	GLuint VAO = CreateVertexNormUVArray(vertices, numPoints, indices, numFaces);
	CreateInstancedAttrib(3, VAO,instancedData,numInstances);
	return VAO;
}

GLuint InitVAO(int vertexSize,void* vertices,int numPoints,int numFaces,unsigned int* indices){
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	CreateVertexArrayBuffer(vertices, vertexSize, numPoints);
	CreateIndexArrayBuffer(indices, numFaces * 3);
	return VAO;
}

void CreateInstancedAttrib(unsigned int attribNum,GLuint VAO,GLuint& posVBO,int numInstances){
	glBindVertexArray(VAO);
	posVBO = CreateVertexArrayBuffer(0, sizeof(vec3),numInstances,GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(attribNum);
	glVertexAttribPointer(attribNum, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	glVertexAttribDivisor(attribNum, 1); 
	glBindVertexArray(0);
}
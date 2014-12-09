//Shader.h - interface for a shader wrapper class
//Last update: 20/07/2012
#pragma once
#include <gl\glew.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>

using namespace glm;
using namespace std;

void PrintActiveUniforms(GLuint handle);
void PrintActiveAttribs(GLuint handle);

void InitAttribs(vector<char*>& m_attribs,GLuint program);
bool LinkProgram(GLuint program);

string ReadFile(const string& filename);
GLuint CreateShader(const char* vert,const char* frag,const char** attribs,int numAttribs);
GLuint CreateShader(const char* vert, const char* frag, const char* geom, const char** attribs, int numAttribs);
GLuint CompileShader(GLenum eShaderType, const string& shaderFile, bool fromFile = true);
GLuint CreateShaderProgram(GLuint* vertShaders, unsigned int numVert, GLuint* fragShaders, unsigned int numFrag,
	GLuint* geomShaders, unsigned int numGeom, unsigned int numAttribs,const char** attribs);
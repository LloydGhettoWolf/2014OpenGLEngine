//Shader.cpp - implementation of shader wrapper class
//Last Update: 16/10/2014 - removed all class cruft, made this a library of shader funcs
//Reason? Mainly because it is silly to store name and filepath - what was I thinking?
#include "Shader.h"
#include <iostream>
#include <fstream>

GLuint CreateShader(const char* vert, const char* frag, const char** attribs, int numAttribs){
	GLuint vertexShader;
	GLuint fragmentShader;

	vertexShader = CompileShader(GL_VERTEX_SHADER, vert);
	fragmentShader = CompileShader(GL_FRAGMENT_SHADER, frag);

	if (!vertexShader || !fragmentShader)
		return NULL;

	return CreateShaderProgram(&vertexShader, 1, &fragmentShader, 1, 0, 0, numAttribs, attribs);
	return true;
}

GLuint CompileShader(GLenum eShaderType,const string& shaderFile,bool fromFile){
	GLuint shader = glCreateShader(eShaderType);

	if(fromFile){
		string strFileData = ReadFile("shaders\\" + shaderFile);

		if (strFileData == "")
			return 0;

		const char* file = strFileData.c_str();
		glShaderSource(shader,1,&file,NULL);
	}else{
		const char* file = shaderFile.c_str();
		glShaderSource(shader,1,&file,NULL);
	}

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&status);

	if(status==GL_FALSE){
		GLint infoLogLength;
		glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader,infoLogLength,NULL,strInfoLog);

		const char* strShaderType = NULL;

		switch(eShaderType){
			case GL_VERTEX_SHADER: 
				strShaderType = "vertex";
				break;
			case GL_FRAGMENT_SHADER:
				strShaderType = "fragment";
				break;
			case GL_GEOMETRY_SHADER:
				strShaderType = "geometry";
				break;
		}

		fprintf(stderr,"Compile failure in %s shader: \n %s \n",strShaderType,strInfoLog);
		delete[] strInfoLog;

		return NULL;
	}

	return shader;
}

GLuint CreateShaderProgram(GLuint* vertShaders,unsigned int numVert,GLuint* fragShaders,unsigned int numFrag,
	                       GLuint* geomShaders,unsigned int numGeom,unsigned int numAttribs,const char** attribs){

	GLuint program = glCreateProgram();

	for (unsigned int iprog = 0; iprog < numVert; iprog++){
		glAttachShader(program, vertShaders[iprog]);
	}

	for (unsigned int iprog = 0; iprog < numFrag; iprog++){
		glAttachShader(program, fragShaders[iprog]);
	}

	for (unsigned int iprog = 0; iprog < numGeom; iprog++){
		glAttachShader(program, geomShaders[iprog]);
	}

	for(unsigned int iLoop = 0; iLoop < numAttribs;iLoop++){
		glBindAttribLocation(program,iLoop,attribs[iLoop]);
	}

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program,GL_LINK_STATUS,&status);

	if(status == GL_FALSE){
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;

		return NULL;
	}

	for (unsigned int iprog = 0; iprog < numVert; iprog++){
		glDetachShader(program, vertShaders[iprog]);
	}

	for (unsigned int iprog = 0; iprog < numFrag; iprog++){
		glDetachShader(program, fragShaders[iprog]);
	}

	for (unsigned int iprog = 0; iprog < numGeom; iprog++){
		glDetachShader(program, geomShaders[iprog]);
	}

	return program;
}

void PrintActiveUniforms(GLuint handle){
		
	GLint activeUniforms,length;
	glGetProgramiv(handle,GL_ACTIVE_UNIFORMS,&activeUniforms);

	for(int index = 0; index < activeUniforms; index++){
		char* uniformName = new char[50];
		glGetActiveUniformName(handle,index,50,&length,uniformName);
		printf(uniformName);
		delete [] uniformName;
	}
}

void PrintActiveAttribs(GLuint handle){
	GLint maxLength, nAttribs,location,written, size;
	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES,&nAttribs);
	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,&maxLength);

	GLenum type;

	for(int index = 0; index < nAttribs; index++){
		char* name = new char[50];
		glGetActiveAttrib(handle, index, maxLength, &written,&size, &type, name );
		location = glGetAttribLocation(handle, name);
		printf(" %-5d | %s\n",location, name);
		delete [] name;
	}
}


string ReadFile(const string& filename){
    ifstream fileIn(filename);

    if (!fileIn.good()){
		cerr << "Could not load shader: " << filename << endl;
		return "";
    }

    string stringBuffer(istreambuf_iterator<char>(fileIn), (istreambuf_iterator<char>()));
	return stringBuffer;
}
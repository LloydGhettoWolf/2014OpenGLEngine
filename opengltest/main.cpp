#include <gl\glew.h>
#include <gl\glfw.h>
#include <gl\GL.h>
#include <glm\glm.hpp>
#include <glm\gtx\inverse_transpose.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <math.h>
#include <IL\il.h>
#include "Shader.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "VertexTypes.h"
#include "StaticMesh.h"

using namespace glm;

GLuint CreateTriangleShader();
void GLFWCALL MouseCallback(int x, int y);
void ReadMouse(Camera& myCam,int x, int y);

Camera myCam;

int main(int argc, char** argv){

	GLuint shaderProg; 

	CustomVertexUV vertices[3];
	vertices[0].vertexPoint = vec3(-1.0f, -1.0f,  0.0f);
	vertices[0].uv			= vec2(0.0f, 0.0f);
	vertices[1].vertexPoint = vec3(1.0f, -1.0f,  0.0f);
	vertices[1].uv			= vec2(1.0f, 0.0f);
	vertices[2].vertexPoint = vec3(0.0f, 1.0f,  0.0f);
	vertices[2].uv			= vec2(0.5f, 1.0f);

	unsigned int indices[3] = { 0, 1, 2 };
	
	if (glfwInit() != GL_TRUE){
		std::cout << "glfw failed!" << std::endl;
		return 1;
	}

	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 3);

	if (!glfwOpenWindow(1024, 768, 8, 8, 8, 8, 32, 32, GLFW_WINDOW)){
		std::cout << "cant create window" << std::endl;
		glfwTerminate();
		return 1;
	}

	if (glewInit() != GLEW_OK){
		std::cout << "glew failed!" << std::endl;
		return 1;
	}

	ilInit();

	shaderProg = CreateTriangleShader();

	if (shaderProg == -1){
		std::cerr << "error creating shader" << std::endl;
		return -1;
	}

	glEnable(GL_TEXTURE_2D);
	GLuint tex = CreateTexture("rockTexture.png", GL_RGB, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

	GLuint depthTex, depthBuf;

	if (!CreateDepthTexture(depthTex, depthBuf)) return 1;

	//Depth states
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	StaticMesh sm;
	InitStaticMesh(sm, "palm_lod0.obj");

	glfwSetMousePosCallback(MouseCallback);

	GLuint VAO = CreateVertexUVArray(vertices,sizeof(CustomVertexUV),indices,1);
	vec3 camPos(0.0f, 0.0f, -4.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	mat4x4 rotationMatrix,moveMatrix;
	mat3x3 normalMatrix;
	mat4x4 perspectiveMatrix = glm::perspective(60.0f, 1024.0f/768.0f,1.0f,500.0f);
	moveMatrix = glm::translate(moveMatrix, glm::vec3(0.0f, 0.0f, -15.0f));
	myCam = CreateCamera(camPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	GLint cameraMatrixUniform		= glGetUniformLocation(shaderProg, "cameraMatrix");
	GLint perspectiveMatrixUniform  = glGetUniformLocation(shaderProg, "perspectiveMatrix");
	GLint textureUniform			= glGetUniformLocation(shaderProg, "tex");
	GLint rotationUniform			= glGetUniformLocation(shaderProg, "rotationMatrix");

	vec3 lightPosition(0.0f, 0.0f, 5.0f);
	vec3 upVec(0.0f, 1.0f, 0.0f);
	

	glUseProgram(shaderProg);
		glUniformMatrix4fv(perspectiveMatrixUniform, 1, GL_FALSE, &perspectiveMatrix[0][0]);
		glUniform1i(textureUniform,0);
	glUseProgram(0);

	do{
		static float rotation = 0.0f;


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProg);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,tex);
			glUniformMatrix4fv(cameraMatrixUniform, 1, GL_FALSE, &myCam.viewMatrix[0][0]);
			RenderStaticMesh(sm);
		glUseProgram(0);

		glfwSwapBuffers();

		rotation = 0.4f;

		rotationMatrix = glm::rotate(rotationMatrix, rotation, upVec);
		normalMatrix   = inverseTranspose(mat3(rotationMatrix));
 
	} while (true);

	return 0;
}

GLuint CreateTriangleShader(){
	GLuint vertexShader;
	GLuint fragmentShader;

	const int numAttribs = 3;
	char* attribs[numAttribs] = { "inCoords","inNormals","inUV" };

	vertexShader   = CreateShader(GL_VERTEX_SHADER, "basic.vp");
	fragmentShader = CreateShader(GL_FRAGMENT_SHADER, "basic.fp");

	if (!vertexShader || !fragmentShader)
		return -1;

	return CreateShaderProgram(&vertexShader, 1, &fragmentShader, 1, 0, 0, numAttribs, &attribs[0]);
}

void GLFWCALL MouseCallback(int x, int y){
	ReadMouse(myCam,x, y);
}

void ReadMouse(Camera& myCam,int x, int y)
{
	static int prevX, prevY;

	//mouse stuff
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
		ComboRotate(myCam,(float)(x - prevX)*1.0f, (float)(y - prevY)*1.0f);
	}

	prevX = x;
	prevY = y;
}
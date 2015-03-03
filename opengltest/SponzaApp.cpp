//SponzaApp.cpp
#include "SponzaApp.h"


#include <gtc\matrix_transform.hpp>

#include <iostream>
#include <math.h>
#include <time.h>
#include <IL\il.h>
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "VertexTypes.h"

bool SponzaApp::Init(){

	if (glfwInit() != GL_TRUE){
		std::cout << "glfw failed!" << std::endl;
		return false;
	}

	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 4);
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 0);


	if (!glfwOpenWindow(APP_WIDTH, APP_HEIGHT, 8, 8, 8, 8, 32, 32, GLFW_WINDOW)){
		std::cout << "cant create window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(KeyCallback);

	if (glewInit() != GLEW_OK){
		std::cout << "glew failed!" << std::endl;
		return false;
	}

	ilInit();

	//Depth states
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_CLAMP);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_lightingShader = CreateLightingShader();

	InitStaticMesh(m_sponzaMesh, "san-miguel.obj", "meshes\\san\ miguel\\");

	m_camera = CreateCamera(vec3(0.0f, 2.0f, 0.0f), vec3(0.0f, 2.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(60.0f, 1024.0f / 768.0f, 1.0f, 1000.0f);

	return true;
}

void SponzaApp::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;

	GLuint shaderProg = m_lightingShader;

	MaterialUniforms matUni;

	GLint cameraMatrixUniform      = glGetUniformLocation(shaderProg, "cameraMatrix");
	GLint perspectiveMatrixUniform = glGetUniformLocation(shaderProg, "perspectiveMatrix");
	GLint textureUniform           = glGetUniformLocation(shaderProg, "tex");
	GLint eyePosUniform            = glGetUniformLocation(shaderProg, "eyePos");
	GLint normalMatrixUniform      = glGetUniformLocation(shaderProg, "normalMatrix");
	matUni.diffuseUniform          = glGetUniformLocation(shaderProg, "materialDiffuse");
	matUni.ambientUniform          = glGetUniformLocation(shaderProg, "materialAmbient");
	matUni.specularUniform         = glGetUniformLocation(shaderProg, "materialSpecular");
	matUni.shininessUniform        = glGetUniformLocation(shaderProg, "shininess");
	GLint scaleUniform             = glGetUniformLocation(shaderProg, "scaleMatrix");
	GLint lightDir                 = glGetUniformLocation(shaderProg, "lightDir");
	GLint lightCol				   = glGetUniformLocation(shaderProg, "lightCol");

	vec3 lightVec(1.0f, -1.0f, 1.0f);
	vec3 lightColVec(1.0f, 1.0f, 1.0f);
	mat4x4 rotation = rotate(mat4x4(), glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(rotation));
	mat4x4 scaleMatrix;
	scaleMatrix = scale(scaleMatrix, vec3(32.0f, 16.0f, 32.0f));

	Material myMaterial;

	string firstStr = "ms per frame: ";
	string time = to_string(deltaTime);

	glUseProgram(shaderProg);
	glUniformMatrix4fv(perspectiveMatrixUniform, 1, GL_FALSE, &m_camera.projectionMatrix[0][0]);
	glUniformMatrix3fv(normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniform3fv(lightDir, 1, &lightVec[0]);
	glUniform3fv(lightCol, 1, &lightColVec[0]);
	glUniformMatrix4fv(scaleUniform, 1, GL_FALSE, &scaleMatrix[0][0]);
	glUniform1i(textureUniform, 0);
	glUseProgram(0);


	currentFrame = glfwGetTime();

	do{
		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProg);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(textureUniform, 0);

		glUniformMatrix4fv(cameraMatrixUniform, 1, GL_FALSE, &m_camera.viewMatrix[0][0]);
		glUniform3fv(eyePosUniform, 1, &m_camera.pos[0]);
		RenderStaticMeshComponent(m_sponzaMesh.m_meshData[0]);

		glUseProgram(0);

		time = to_string(deltaTime * 1000.0f);

		glfwSwapBuffers();

		ReadMouse();
		ReadKeys();
		MoveCameraForward(m_camera, deltaTime * m_moveForwardAmount);
		MoveCameraVertically(m_camera, deltaTime *  m_moveUpAmount);
		MoveCameraHorizontally(m_camera, deltaTime *  m_moveSidewaysAmount);

		lastFrame = currentFrame;
		currentFrame = glfwGetTime();
	} while (m_run);

};

void SponzaApp::ShutDown(){
	glDeleteShader(m_lightingShader);

	m_run = false;
};

GLuint SponzaApp::CreateLightingShader(){

	const int numAttribs = 3;
	const char* attribs[numAttribs] = { "inCoords", "inNormals", "inUVs" };

	return CreateShader("lighting.vp", "lighting.fp", attribs, numAttribs);
}

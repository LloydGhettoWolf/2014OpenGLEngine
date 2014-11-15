//MainApp.cpp
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <math.h>
#include <IL\il.h>
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "VertexTypes.h"
#include "TeapotApp.h"
#include "GroundPlane.h"
#include "Defines.h"

bool TeapotApp::Init(){

	if (glfwInit() != GL_TRUE){
		std::cout << "glfw failed!" << std::endl;
		return false;
	}

	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 3);

	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);

	if (!glfwOpenWindow(APP_WIDTH, APP_HEIGHT, 8, 8, 8, 8,32, 32, GLFW_WINDOW)){
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

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_teapotShader = CreateInstancedLightingShader();

	GLuint shaderProg = m_teapotShader;
	teapotUniforms.cameraMatrixUniform		= glGetUniformLocation(shaderProg, "cameraMatrix");
	teapotUniforms.perspectiveMatrixUniform = glGetUniformLocation(shaderProg, "perspectiveMatrix");
	teapotUniforms.textureUniform			= glGetUniformLocation(shaderProg, "tex");
	teapotUniforms.rotationUniform			= glGetUniformLocation(shaderProg, "rotationMatrix");
	teapotUniforms.eyePosUniform			= glGetUniformLocation(shaderProg, "eyePos");
	teapotUniforms.lightVecUniform			= glGetUniformLocation(shaderProg, "lightPos");
	teapotUniforms.lightColUniform			= glGetUniformLocation(shaderProg, "lightColors");
	teapotUniforms.normalMatrixUniform		= glGetUniformLocation(shaderProg, "normalMatrix");
	teapotUniforms.matUni.diffuseUniform	= glGetUniformLocation(shaderProg, "materialDiffuse");
	teapotUniforms.matUni.ambientUniform	= glGetUniformLocation(shaderProg, "materialAmbient");
	teapotUniforms.matUni.specularUniform	= glGetUniformLocation(shaderProg, "materialSpecular");
	teapotUniforms.matUni.shininessUniform	= glGetUniformLocation(shaderProg, "shininess");
	teapotUniforms.scaleUniform				= glGetUniformLocation(shaderProg, "scaleMatrix");
	teapotUniforms.instancedUniform			= glGetUniformLocation(shaderProg, "instanced");

	m_gBufferShader = CreateGBufferShader();

	shaderProg = m_gBufferShader;
	gBufferUniforms.cameraMatrixUniform			= glGetUniformLocation(shaderProg, "vpMatrix");
	gBufferUniforms.normalMatrixUniform			= glGetUniformLocation(shaderProg, "normalMatrix");
	gBufferUniforms.rotationMatrixUniform		= glGetUniformLocation(shaderProg, "rotationMatrix");
	gBufferUniforms.materialDiffuseUniform		= glGetUniformLocation(shaderProg, "materialDiffuse");
	gBufferUniforms.materialAmbientUniform		= glGetUniformLocation(shaderProg, "materialAmbient");
	gBufferUniforms.materialSpecularUniform		= glGetUniformLocation(shaderProg, "materialSpecular");
	gBufferUniforms.scaleMatrixUniform			= glGetUniformLocation(shaderProg, "scaleMatrix");
	gBufferUniforms.instancedUniform			= glGetUniformLocation(shaderProg, "instanced");

	InitStaticMesh(m_teapotMesh, "teapot.obj","teapot\\",64);

	m_camera = CreateCamera(vec3(0.0f,0.0f,-40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	m_fontShader = CreateFontShader();
	InitText2D(m_counterFont,"exportedFont.bmp", "ms per frame: 0.00", 50, 50, 24,m_fontShader);

	m_groundPlaneBuffer = CreateGroundPlaneData();

	if (!CreateGBuffer()){
		return false;
	}
	
	return true;
}

void TeapotApp::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;

	GLuint shaderProg = m_teapotShader;


	mat4x4 rotation = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(rotation));

	vec3 movement[64];

	vec3 lightPositions[5];

	vec3 lightColors[5];

	lightColors[0] = vec3(0.0f,0.0f,1.0f);
	lightColors[1] = vec3(1.0f, 0.0f, 0.0f);
	lightColors[2] = vec3(0.0f, 1.0f, 0.0f);
	lightColors[3] = vec3(0.5f, 0.0f, 0.5f);
	lightColors[4] = vec3(0.2f, 0.6f, 0.2f);


	for (int potRow = 0; potRow < 8; potRow++){
		for (int potCol = 0; potCol < 8; potCol++){
			int index = potRow * 8 + potCol;
			movement[index] = vec3(potCol * 60.0f - 240.0f,0.0f,potRow * 42.0f - 168.0f);
		}
	}

	string firstStr = "ms per frame: ";
	string time     = to_string(deltaTime);

	shaderProg = m_teapotShader;
	glUseProgram(shaderProg);
	glUniformMatrix4fv(teapotUniforms.perspectiveMatrixUniform, 1, GL_FALSE, &m_camera.projectionMatrix[0][0]);
	
	glUniformMatrix3fv(teapotUniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniform3fv(teapotUniforms.lightColUniform, 5, &lightColors[0][0]);
	mat4x4 translationMatrix = rotation;
	glUniformMatrix4fv(teapotUniforms.rotationUniform, 1, GL_FALSE, &translationMatrix[0][0]);
	glUseProgram(0);

	shaderProg = m_gBufferShader;
	glUseProgram(shaderProg);
	glUniformMatrix3fv(gBufferUniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(gBufferUniforms.rotationMatrixUniform, 1, GL_FALSE, &translationMatrix[0][0]);
	glUseProgram(0);

	lastFrame = currentFrame = glfwGetTime();

	float rotationAmount = 0.0f;


	do{
		float cosAmount = cosf(rotationAmount);
		float sinAmount = sinf(rotationAmount);
		float cosAmount2 = cosf(rotationAmount*2.0f);
		float sinAmount2 = sinf(rotationAmount*4.0f);

		rotationAmount += 0.02f;

		lightPositions[0] = vec3(45.0f*cosAmount, 20.0f, 180.0f*sinAmount2);
		lightPositions[1] = vec3(-45.0f*cosAmount, -20.0f, 42.0f*sinAmount);
		lightPositions[2] = vec3(240.0f*cosAmount2 , 20.0f, -42.0f*sinAmount);
		lightPositions[3] = vec3(-120.0f*cosAmount,  40.0f, -100.0f*sinAmount);
		lightPositions[4] = vec3(0.0f, 60.0f * cosAmount, 50.0f*sinAmount2);
		
		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	//	RenderForward(lightPositions, movement);
		RenderDeferred(lightPositions, movement);
				
		time = to_string(deltaTime * 1000.0f);
		ChangeText2D(m_counterFont, (firstStr + time.substr(0,4)).c_str(), 50, 50, 24);
		PrintText2D(m_counterFont);

		glfwSwapBuffers();

		ReadMouse();
		ReadKeys();
		MoveCameraForward(m_camera, deltaTime * m_moveForwardAmount);
		MoveCameraVertically(m_camera, deltaTime *  m_moveUpAmount);
		MoveCameraHorizontally(m_camera, deltaTime *  m_moveSidewaysAmount);

		lastFrame = currentFrame;
		currentFrame = glfwGetTime();
	} while (m_run);

	ShutDown();
};

void TeapotApp::ShutDown(){
	CleanupText2D(m_counterFont);
	DestroyMesh(m_teapotMesh);
	glDeleteVertexArrays(1, &m_groundPlaneBuffer);
	glDeleteShader(m_teapotShader);
	glDeleteFramebuffers(1, &m_gBuffer.fboObject);
};

GLuint TeapotApp::CreateInstancedLightingShader(){
	GLuint vertexShader;
	GLuint fragmentShader;

	const int numAttribs = 4;
	char* attribs[numAttribs] = { "inCoords", "inNormals","inUVs","inPositions" };

	vertexShader = CreateShader(GL_VERTEX_SHADER, "instancedLighting.vp");
	fragmentShader = CreateShader(GL_FRAGMENT_SHADER, "instancedLighting.fp");

	if (!vertexShader || !fragmentShader)
		return -1;

	return CreateShaderProgram(&vertexShader, 1, &fragmentShader, 1, 0, 0, numAttribs, &attribs[0]);
}

void TeapotApp::RenderForward(vec3* lightPositions,vec3* teapotPositions){
	vec3 diff = vec3(0.4f, 0.4f, 0.4f);

	mat4x4 scaleMatrix1, identity;
	scaleMatrix1 = scale(scaleMatrix1, vec3(0.2f, 0.2f, 0.2f));

	glUseProgram(m_teapotShader);
		
		glUniformMatrix4fv(teapotUniforms.scaleUniform, 1, GL_FALSE, &scaleMatrix1[0][0]);
		glUniformMatrix4fv(teapotUniforms.cameraMatrixUniform, 1, GL_FALSE, &m_camera.viewMatrix[0][0]);
		glUniform3fv(teapotUniforms.eyePosUniform, 1, &m_camera.pos[0]);
		glUniform3fv(teapotUniforms.lightVecUniform, 5, &lightPositions[0][0]);

		glUniform1i(teapotUniforms.instancedUniform, 1);
		RenderInstancedStaticMesh(m_teapotMesh, teapotUniforms.matUni, &teapotPositions[0]);
		glUniformMatrix4fv(teapotUniforms.scaleUniform, 1, GL_FALSE, &identity[0][0]);
		glUniform1i(teapotUniforms.instancedUniform, 0);
		glUniform3fv(teapotUniforms.matUni.diffuseUniform, 1, &diff[0]);
		glBindVertexArray(m_groundPlaneBuffer);
		glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
	glUseProgram(0);
}

void TeapotApp::RenderDeferred(vec3* lightPositions, vec3* teapotPositions){
	vec3 diff = vec3(0.4f, 0.4f, 0.4f);

	mat4x4 scaleMatrix1,identity;
	scaleMatrix1 = scale(scaleMatrix1, vec3(0.2f, 0.2f, 0.2f));

	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.fboObject);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(m_gBufferShader);

			glUniformMatrix4fv(gBufferUniforms.scaleMatrixUniform, 1, GL_FALSE, &scaleMatrix1[0][0]);
			mat4 viewProjection = m_camera.projectionMatrix * m_camera.viewMatrix;
			glUniformMatrix4fv(gBufferUniforms.cameraMatrixUniform, 1, GL_FALSE, &viewProjection[0][0]);

			glUniform1i(gBufferUniforms.instancedUniform, 1);
			RenderInstancedStaticMesh(m_teapotMesh, teapotUniforms.matUni, &teapotPositions[0]);
			glUniformMatrix4fv(gBufferUniforms.scaleMatrixUniform, 1, GL_FALSE, &identity[0][0]);
			glUniform1i(gBufferUniforms.instancedUniform, 0);
			glUniform3fv(gBufferUniforms.materialDiffuseUniform, 1, &diff[0]);
			glBindVertexArray(m_groundPlaneBuffer);
			glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
		glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint TeapotApp::CreateGBufferShader(){
	GLuint vertexShader;
	GLuint fragmentShader;

	const int numAttribs = 4;
	char* attribs[numAttribs] = { "inCoords", "inNormals", "inUVs", "inPositions" };

	vertexShader = CreateShader(GL_VERTEX_SHADER, "gBuffer.vp");
	fragmentShader = CreateShader(GL_FRAGMENT_SHADER, "gBuffer.fp");

	if (!vertexShader || !fragmentShader)
		return -1;

	return CreateShaderProgram(&vertexShader, 1, &fragmentShader, 1, 0, 0, numAttribs, &attribs[0]);
}

bool TeapotApp::CreateGBuffer(){
	return CreateGBufferData(m_gBuffer);
}




//MainApp.cpp
#include <glm\gtx\inverse_transpose.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <math.h>
#include <IL\il.h>
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "VertexTypes.h"
#include "MainApp.h"
#include "Font.h"

bool App::Init(){

	if (glfwInit() != GL_TRUE){
		std::cout << "glfw failed!" << std::endl;
		return false;
	}

	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 3);

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

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_teapotShader = CreateTeapotShader();	


	if (!CreateDepthTexture(m_depthBuffer)) {
		cout << "failure to create depthBuffer" << endl;
		return false;
	}

	InitStaticMesh(m_teapotMesh, "teapot.obj");

	m_camera = CreateCamera(vec3(0.0f,0.0f,-4.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(60.0f, 1024.0f / 768.0f, 1.0f, 500.0f);

	InitText2D("myFont.png", "test test test", 50, 50, 24);
	
	return true;
}

void App::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;

	GLuint shaderProg = m_teapotShader;

	Material myMaterial;
	myMaterial.ambient  = glm::vec3(0.1f, 0.1f, 0.1f);
	myMaterial.diffuse  = glm::vec3(0.6f, 0.2f, 0.0f);
	myMaterial.specular = glm::vec3(0.4f,0.4f,0.4f);

	GLint cameraMatrixUniform		= glGetUniformLocation(shaderProg, "cameraMatrix");
	GLint perspectiveMatrixUniform	= glGetUniformLocation(shaderProg, "perspectiveMatrix");
	GLint textureUniform			= glGetUniformLocation(shaderProg, "tex");
	GLint rotationUniform			= glGetUniformLocation(shaderProg, "rotationMatrix");
	GLint eyePosUniform				= glGetUniformLocation(shaderProg, "eyePos");
	GLint shininessUniform			= glGetUniformLocation(shaderProg, "shininess");
	GLint lightVecUniform			= glGetUniformLocation(shaderProg, "lightVector");
	GLint normalMatrixUniform		= glGetUniformLocation(shaderProg, "normalMatrix");
	GLint diffuseUniform			= glGetUniformLocation(shaderProg, "materialDiffuse");
	GLint ambientUniform			= glGetUniformLocation(shaderProg, "materialAmbient");
	GLint specularUniform			= glGetUniformLocation(shaderProg, "materialSpecular");

	vec3 lightVec(0.0f, 0.0f, 1.0);
	mat4x4 rotation = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(rotation));

	glUseProgram(shaderProg);
	glUniformMatrix4fv(perspectiveMatrixUniform, 1, GL_FALSE, &m_camera.projectionMatrix[0][0]);
	glUniformMatrix3fv(normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniform3fv(lightVecUniform, 1, &lightVec[0]);
	glUniform1f(shininessUniform, 32.0f);
	glUniformMatrix4fv(rotationUniform, 1, GL_FALSE, &rotation[0][0]);
	glUseProgram(0);

	vec3 movement[24];

	vec3   movementX(7.0f, 0.0f, 0.0f);
	vec3   movementZ(0.0f, 0.0f, 7.0f);

	for (int potRow = 0; potRow < 4; potRow++){
		for (int pot = 0; pot < 3; pot++){
			movement[potRow * 3 + pot] = (float)pot * movementX + (float)potRow * movementZ;
		}
	}

	currentFrame = glfwGetTime();

	do{
		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProg);
		glUniform3fv(diffuseUniform, 1, &myMaterial.diffuse[0]);
		glUniform3fv(specularUniform, 1, &myMaterial.specular[0]);
		glUniform3fv(ambientUniform, 1, &myMaterial.ambient[0]);
		glUniformMatrix4fv(cameraMatrixUniform, 1, GL_FALSE, &m_camera.viewMatrix[0][0]);
		glUniform3fv(eyePosUniform, 1, &m_camera.pos[0]);

		RenderStaticMesh(m_teapotMesh);
		
		glUseProgram(0);

		PrintText2D();

		glfwSwapBuffers();

		ReadMouse();
		ReadKeys();
		MoveCameraForward(m_camera, deltaTime * m_moveForwardAmount);
		MoveCameraVertically(m_camera, deltaTime *  m_moveUpAmount);
		MoveCameraHorizontally(m_camera, deltaTime *  m_moveSidewaysAmount);

		lastFrame = currentFrame;
		currentFrame = glfwGetTime();
	} while (true);

};

void App::ShutDown(){
	CleanupText2D();
};

GLuint App::CreateTeapotShader(){
	GLuint vertexShader;
	GLuint fragmentShader;

	const int numAttribs = 3;
	char* attribs[numAttribs] = { "inCoords", "inNormals", "inPositions" };

	vertexShader = CreateShader(GL_VERTEX_SHADER, "basic.vp");
	fragmentShader = CreateShader(GL_FRAGMENT_SHADER, "basic.fp");

	if (!vertexShader || !fragmentShader)
		return -1;

	return CreateShaderProgram(&vertexShader, 1, &fragmentShader, 1, 0, 0, numAttribs, &attribs[0]);
}


void App::ReadMouse(){
	static int prevX, prevY;

	int x = 0;
	int y = 0;
	glfwGetMousePos(&x,&y);

	//mouse stuff
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
		ComboRotate(m_camera,(float)(x - prevX)*1.0f, (float)(y - prevY)*1.0f);
	}

	prevX = x;
	prevY = y;
}
void App::ReadKeys(){

	switch (lastKeyPress){
		case 'W':
			m_moveForwardAmount = lastKeyAction * FORWARDS_SPEED;
			break;
		case 'S':
			m_moveForwardAmount = lastKeyAction * -FORWARDS_SPEED;
			break;
		case 'A':
			m_moveSidewaysAmount = lastKeyAction * -FORWARDS_SPEED;
			break;
		case 'D':
			m_moveSidewaysAmount = lastKeyAction * FORWARDS_SPEED;
			break;
		case GLFW_KEY_UP:
			m_moveUpAmount		 = lastKeyAction * FORWARDS_SPEED;
			break;
		case GLFW_KEY_DOWN:
			m_moveUpAmount		 = lastKeyAction * -FORWARDS_SPEED;
			break;
	}
	
}

int App::lastKeyPress  = 0;
int App::lastKeyAction = 0;

void GLFWCALL KeyCallback(int key, int action){
	App::lastKeyPress  = key;
	App::lastKeyAction = action;
}
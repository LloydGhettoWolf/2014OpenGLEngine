//MainApp.cpp
#include "MainApp.h"
#include <gtc\matrix_transform.hpp>
#include <IL\il.h>
#include <iostream>

using namespace std;

bool App::StandardInit(){

	if (glfwInit() != GL_TRUE){
		std::cout << "glfw failed!" << std::endl;
		return false;
	}

	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 4);
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 4);

	m_windowWidth = APP_WIDTH;
	m_windowHeight= APP_HEIGHT;

	if (!glfwOpenWindow(m_windowWidth, m_windowHeight, 8, 8, 8, 8, 32, 32, GLFW_WINDOW)){
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


	//Depth states - in deffered render this is disabled
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);

	m_camera = CreateCamera(vec3(0.0f, 5.0f, -20.0f), vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	glfwSetWindowSizeCallback(MyResize);
}

void App::ReadMouse(){
	static int prevX, prevY;

	int x = 0;
	int y = 0;
	glfwGetMousePos(&x,&y);

	int mouseButtonLeft  = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
	int mouseButtonRight = glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
	int mouseButtonMid   = glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE);

	int TwResult = TwEventMousePosGLFW(x, y);

		
	if (mouseButtonLeft == GLFW_PRESS) {
		if (TwResult  && !mouseDown){
			TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
			mouseDown = true;
		}else if(!mouseDown){
			ComboRotate(m_camera, (float)(x - prevX)*1.0f, (float)(y - prevY)*1.0f);
		}
	}else if (mouseButtonLeft == GLFW_RELEASE && mouseDown){
		TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
		mouseDown = false;
	}

	if (mouseButtonRight == GLFW_PRESS){
		if (TwResult){
			TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_RIGHT);
		}else{
			MoveCameraCustom(m_camera, m_camera.pos + (float)(prevY - y) * 0.5f * m_camera.lookVec);
		}
	}

	if (mouseButtonMid == GLFW_PRESS){
		if (TwResult){
			TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_MIDDLE);
		}else{
			vec3 displacement = (prevY - y) * 0.5f * m_camera.upVec +
				(prevX - x) * -0.5f * m_camera.rightVec;
			MoveCameraCustom(m_camera, m_camera.pos + displacement);
		}
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
		case 'Q':
			m_run = false;
			break;
	}
	
}

void App::Resize(float aspect,int width,int height){
	glViewport(0, 0, width, height);
	m_camera.projectionMatrix = glm::perspective(45.0f,aspect, 3.0f, 500.0f);
	glfwSetWindowSize(width,height);
	TwWindowSize(width, height);

	m_windowWidth  = width;
	m_windowHeight = height;
}

int  App::lastKeyPress  = 0;
int  App::lastKeyAction = 0;
bool App::mouseDown = false;

void GLFWCALL KeyCallback(int key, int action){
	App::lastKeyPress  = key;
	App::lastKeyAction = action;
}

float RandomFloat(float low,float high){
	return low + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(high-low));
}


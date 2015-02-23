//MainApp.cpp
#include "MainApp.h"
#include <gtc\matrix_transform.hpp>


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
		if (TwResult){
			TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
		}else{
			ComboRotate(m_camera, (float)(x - prevX)*1.0f, (float)(y - prevY)*1.0f);
		}
	}else if (mouseButtonLeft == GLFW_RELEASE){
		TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
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
}

int App::lastKeyPress  = 0;
int App::lastKeyAction = 0;

void GLFWCALL KeyCallback(int key, int action){
	App::lastKeyPress  = key;
	App::lastKeyAction = action;
}

float RandomFloat(float low,float high){
	return low + static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(high-low));
}


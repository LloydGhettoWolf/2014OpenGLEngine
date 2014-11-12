//MainApp.cpp
#include "MainApp.h"



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
		case 'Q':
			m_run = false;
			break;
	}
	
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
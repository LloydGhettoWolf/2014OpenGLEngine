//MainApp.h
#pragma once
#include <gl\glew.h>
#include <gl\glfw.h>
#include <glm\glm.hpp>

#include <map>
#include <string>

#include "Camera.h"
#include "StaticMesh.h"
#include "Font.h"

const float FORWARDS_SPEED = 50.0f;
const float SIDEWAYS_SPEED = 50.0f;
const float ROTATE_YAW_SPEED = 80.0f;
const float ROTATE_PITCH_SPEED = 80.0f;
const float CLIMB_SPEED = 50.0f;



const float APP_WIDTH  = 1024.0f;
const float APP_HEIGHT = 768.0f;

class App{
public:
	virtual bool Init() = 0;
	virtual void Run()  = 0;
	virtual void ShutDown() = 0;

	void ReadMouse();
	void ReadKeys();

	static int			  lastKeyPress;
	static int			  lastKeyAction;

protected:
	Camera				  m_camera;

	bool				  m_run = true;

	float				  m_moveForwardAmount  = 0.0f;
	float				  m_moveUpAmount	   = 0.0f;
	float				  m_moveSidewaysAmount = 0.0f;

	GLuint				 m_fontShader = 0;
	FontData			 m_counterFont;
};

void GLFWCALL KeyCallback(int key, int action);

float RandomFloat(float high,float low);
//MainApp.h
#pragma once
#include <gl\glew.h>
#include <gl\glfw.h>
#include <glm.hpp>

#include <AntTweakBar.h>

#include <map>
#include <string>

#include "Camera.h"
#include "StaticMesh.h"
#include "Font.h"
#include "Defines.h"

class App{
public:

	App(){};
	virtual ~App(){ }

	virtual bool Init() = 0;
	virtual void Run()  = 0;
	virtual void ShutDown() = 0;

	void ReadMouse();
	void ReadKeys();

	void Resize(float aspect, int width, int height);

	static int			  lastKeyPress;
	static int			  lastKeyAction;

	static bool			  mouseDown;

protected:

	bool StandardInit();

	Camera				  m_camera;

	bool				  m_run = true;

	float				  m_moveForwardAmount  = 0.0f;
	float				  m_moveUpAmount	   = 0.0f;
	float				  m_moveSidewaysAmount = 0.0f;

	float				  m_windowWidth, m_windowHeight;

};

void GLFWCALL KeyCallback(int key, int action);
void GLFWCALL MyResize(int width, int height);

float RandomFloat(float high,float low);
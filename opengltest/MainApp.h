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
#include "Defines.h"
#include "Frustum.h"

class App{
public:

	App(){};
	virtual ~App(){ }

	virtual bool Init() = 0;
	virtual void Run()  = 0;
	virtual void ShutDown() = 0;

	void ReadMouse();
	void ReadKeys();

	void Resize(float fov, float aspect, int width, int height, float nearZ, float farZ);

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

	Frustum               m_frustum;

};

void GLFWCALL KeyCallback(int key, int action);
void GLFWCALL MyResize(float fov, int width, int height, float nearZ, float farZ);

float RandomFloat(float high,float low);
//MainApp.h
#include <gl\glew.h>
#include <gl\glfw.h>
#include <glm\glm.hpp>

#include <map>
#include <string>

#include "Camera.h"
#include "StaticMesh.h"

const float FORWARDS_SPEED = 0.2f;
const float SIDEWAYS_SPEED = 0.2f;
const float ROTATE_YAW_SPEED = 80.0f;
const float ROTATE_PITCH_SPEED = 80.0f;
const float CLIMB_SPEED = 0.2f;

const int APP_WIDTH  = 1024;
const int APP_HEIGHT = 768;

class App{
public:
	bool Init();
	void Run();
	void ShutDown();

	void ReadMouse();
	void ReadKeys();

	static int			  lastKeyPress;
	static int			  lastKeyAction;

private:
	GLuint CreateTriangleShader();

	Camera				  m_camera;
	GLuint		          m_teapotShader;
	GLuint                m_depthBuffer;
	StaticMesh            m_teapotMesh;

	float				  m_moveForwardAmount  = 0.0f;
	float				  m_moveUpAmount	   = 0.0f;
	float				  m_moveSidewaysAmount = 0.0f;
};

void GLFWCALL KeyCallback(int key, int action);
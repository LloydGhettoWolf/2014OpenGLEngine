#include "Ocean.h"
#include "CubeMap.h"
#include "GroundPlane.h"
#include "Shader.h"
#include "VertexArray.h"
#include <iostream>
#include <random>
#include <gtc\matrix_transform.hpp>
#include <gtx\rotate_vector.hpp>

using namespace std;

const float TEXTURE_SIZE = 1024.0f;

OceanApp::~OceanApp(){

	glDeleteFramebuffers(1,&m_fbo);
	glDeleteTextures(1, &m_waveTex);
}

bool OceanApp::Init(){
	StandardInit();

	glClearColor(0.0f, 0.6f, 0.9f, 1.0f);


	if (!m_cubeMapShader.CreateCubemapShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!LoadShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!InitStaticMesh(m_cube, "cube.obj", "meshes\\")){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	m_camera = CreateCamera(vec3(50.0f, 0.0f, 50.0f), vec3(0.0f, 2.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f, 2000.0f, APP_WIDTH, APP_HEIGHT, 45.0f);

	m_planeData = CreatePlaneData(512, 512, 1.0f, 0.25f);

	if (!CreateWaveTex(m_fbo,m_waveTex)){
		cout << "wave tex create fail!" << endl;
		return false;
	}
	
	//Gerster eqn is:
	//x = QA * DiX * cos(wDx(x,y) + rho*t)
	//y = QA * DiY * cos(wDx(x,y) + rho*t)

	//rho is speed * 2 pi/ L (wavelength)
	//Q is 1/wA (w is freq 2 pi / L)

	vec3   dir = normalize(vec3(1.0f, 0.0f,0.0f));
	
	for (int i = 0; i < NUM_WAVES; ++i){

		float  waveLength = (float)(m_medianLength / 2 + rand() % (2 * m_medianLength) + 1);
		float  angle = (float)(rand() % m_variation + 1);

		float speed = (float)(rand() % m_speed);

		vec2 newDir = (vec2)rotateZ(dir, angle);

		m_waves.amplitude[i] = (float)(rand() % m_medianAmp + 1);
		m_waves.dir[i]       = newDir;
		m_waves.freq[i]      = (2.0f*3.14129f) / waveLength;
		m_waves.rho[i]		 = m_waves.freq[i] * speed;
		m_waves.q[i]         = 0.5f / (m_waves.freq[i] * m_waves.amplitude[i] * (float)NUM_WAVES);
	}

	vec3 screenQuad[6];

	screenQuad[0] = vec3(1.0f, 1.0f, 0.0f);
	screenQuad[1] = vec3(-1.0f, 1.0f, 0.0f);
	screenQuad[2] = vec3(-1.0f, -1.0f, 0.0f);
	screenQuad[3] = vec3(1.0f, 1.0f, 0.0f);
	screenQuad[4] = vec3(-1.0f, -1.0f, 0.0f);
	screenQuad[5] = vec3(1.0f, -1.0f, 0.0f);

	glGenVertexArrays(1, &m_quadBuffer);
	glBindVertexArray(m_quadBuffer);
		GLuint vertexBuff;
		glGenBuffers(1, &vertexBuff);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 6, screenQuad, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	return true;
}

bool OceanApp::LoadShader(){
	const int numAttribs = 1;
	const char* attribs[numAttribs] = { "inCoords"};

	m_oceanShader = CreateShader("waveGen.vp", "waveGen.fp", attribs, numAttribs);

	if (!m_oceanShader)
		return false;

	m_oceanDispshader = CreateShader("waveDisplacement.vp", "waveDisplacement.fp", attribs, numAttribs);

	if (!m_oceanDispshader)
		return false;

	return true;
}


bool OceanApp::InitGui(){
	//define a 5 component vector for amp,dir,freq,speed values in the ui

	return true;
}

void OceanApp::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;

	float oscillTime = 0.0f;

	//get time display
	m_time = deltaTime;

	lastFrame = currentFrame = glfwGetTime();

	float rotationAmount = 0.0f;


	float w = sqrt(9.81f * 1.0f);


	do{
		deltaTime = currentFrame - lastFrame;
		oscillTime += deltaTime;
		//update vertices

		rotationAmount += 0.005f;

		Render();
		glfwSwapBuffers();

		ReadMouse();
		ReadKeys();
		MoveCameraForward(m_camera, deltaTime      * m_moveForwardAmount);
		MoveCameraVertically(m_camera, deltaTime *  m_moveUpAmount);
		MoveCameraHorizontally(m_camera, deltaTime *  m_moveSidewaysAmount);

		lastFrame = currentFrame;
		currentFrame = glfwGetTime();
	} while (m_run);

	ShutDown();

}

void OceanApp::Render(){
	

	GLint directionUni = glGetUniformLocation(m_oceanShader, "directions");
	GLint amplitudeUni = glGetUniformLocation(m_oceanShader, "amplitude");
	GLint freqUni      = glGetUniformLocation(m_oceanShader, "freq");
	GLint rhoUni       = glGetUniformLocation(m_oceanShader, "rho");
	//GLint qUni         = glGetUniformLocation(m_oceanShader, "qVar");
	GLint t		       = glGetUniformLocation(m_oceanShader, "t");
	GLint screenSizeUni= glGetUniformLocation(m_oceanShader, "texSize");

	static float tVal = 0.0f;
	vec2 size = vec2(TEXTURE_SIZE, TEXTURE_SIZE);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);



		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glUseProgram(m_oceanShader);

			glUniform2fv(directionUni, NUM_WAVES, &(m_waves.dir[0][0]));
			glUniform1fv(amplitudeUni, NUM_WAVES, &m_waves.amplitude[0]);
			glUniform1fv(freqUni,      NUM_WAVES, &m_waves.freq[0]);
			glUniform1fv(rhoUni,       NUM_WAVES, &m_waves.rho[0]);
			//glUniform1fv(qUni,         NUM_WAVES, &m_waves.q[0]);
			glUniform2fv(screenSizeUni, 1, &size[0]);
			glUniform1f(t, tVal);

			glViewport(0, 0, TEXTURE_SIZE, TEXTURE_SIZE);
			glBindVertexArray(m_quadBuffer);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

		glUseProgram(0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	tVal += 0.05f;

	GLint textureUni = glGetUniformLocation(m_oceanDispshader, "waveTexture");
	GLint mvpUni     = glGetUniformLocation(m_oceanDispshader, "mvpMatrix");
	GLint texSize    = glGetUniformLocation(m_oceanDispshader, "texSize");

	
	glViewport(0, 0, APP_WIDTH, APP_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(m_oceanDispshader);
		mat4 mvp = m_camera.projectionMatrix * m_camera.viewMatrix;
		glUniformMatrix4fv(mvpUni, 1, GL_FALSE, &mvp[0][0]);
		glUniform2fv(texSize, 1, &size[0]);
		glUniform1i(textureUni, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, m_waveTex);
		glBindVertexArray(m_planeData);
			//glPolygonMode(GL_FRONT, GL_LINE);
			glDrawElements(GL_TRIANGLES, 511 * 511 * 6, GL_UNSIGNED_INT, 0);
			//glPolygonMode(GL_FRONT, GL_FILL);
		glBindVertexArray(0);

	glUseProgram(0);
	
	
}

void OceanApp::ShutDown(){
}

bool OceanApp::CreateWaveTex(GLuint& data, GLuint& waveData){
	GLenum format, type;

	glGenFramebuffers(1, &data);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, data);

	glGenTextures(1, &waveData);

	//glGetInternalformativ(GL_TEXTURE_2D, GL_RGB32F, GL_TEXTURE_IMAGE_TYPE, 1, (GLint*)&type);

	
	glBindTexture(GL_TEXTURE_2D, waveData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,waveData, 0);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	
	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "problem creating wave tex!" << std::endl;
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}
//DeferredApp.cpp
#include "DeferredApp.h"
#include <gtc\matrix_transform.hpp>
#include <gtc\matrix_inverse.hpp>

#include <iostream>
#include <math.h>
#include <IL\il.h>
#include "VertexArray.h"
#include "Texture.h"
#include "VertexTypes.h"
#include "GroundPlane.h"
#include "CubeMap.h"

StaticMesh teapotMesh;
GLuint groundPlaneBuffer;
vec3   positions[NUM_MESHES];
GLuint	m_teaTexture,m_normalTexture;

CubemapShader		  cubemapShader;
StaticMesh            cubeMesh;
GLuint				  cubeMap;

bool DeferredApp::Init(){

	StandardInit();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	if (!m_deferredRenderer.Init()){
		cout << "failed to init def renderer!" << endl;
		return false;
	}


	if (!cubemapShader.CreateCubemapShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!InitStaticMesh(teapotMesh, "teapot.obj", "teapot\\", 64, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}


	if (!InitStaticMesh(cubeMesh, "cube.obj", "meshes\\", 1)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	m_camera = CreateCamera(vec3(0.0f, 0.0f, -40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	glfwSetWindowSizeCallback(MyResize);

	groundPlaneBuffer = CreateGroundPlaneData();


	m_lights.position		= new vec3[NUM_POINT_LIGHTS];
	m_lights.color			= new vec3[NUM_POINT_LIGHTS];
	m_lights.constantAtt	= new float[NUM_POINT_LIGHTS];
	m_lights.expAtt			= new float[NUM_POINT_LIGHTS];
	m_lights.linearAtt		= new float[NUM_POINT_LIGHTS];

	for (int light = 0; light < NUM_POINT_LIGHTS; light++){
		m_lights.constantAtt[light] = 0.0f;
		m_lights.linearAtt[light] = 0.0f;
		m_lights.expAtt[light] = 0.7f;


		float randomX = (float)(rand() % 120) - 60.0f;
		float randomZ = (float)(rand() % 90) - 45.0f;
		float randomY = (float)(rand() % 5);

		lightPos[light] = vec3(randomX, randomY, randomZ);

		float randomR = (float)(rand() % 100) / 100.0;
		float randomG = (float)(rand() % 100) / 100.0;
		float randomB = (float)(rand() % 100) / 100.0;

		m_lights.color[light] = vec3(randomR, randomG, randomB);

		m_radii[light] = (float)(rand() % 20) - 10.0;
	}


	//cubemap
	std::vector<string> cubemapFilenames;
	cubemapFilenames.push_back("CubeMaps\\Pylons\\posx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Pylons\\negx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Pylons\\posy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Pylons\\negy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Pylons\\posz.jpg");
	cubemapFilenames.push_back("CubeMaps\\Pylons\\negz.jpg");
	cubeMap = CreateCubeMap(cubemapFilenames);

	m_material.ambient = vec3(0.1f, 0.1f, 0.1f);
	m_material.diffuse = vec3(0.5f, 0.4f, 0.5f);
	m_material.shininess = 64.0f;


	m_teaTexture    = CreateTexture("Textures\\160.jpg", GL_LINEAR, GL_NEAREST);
	m_normalTexture = CreateTexture("Textures\\160_norm.jpg", GL_LINEAR, GL_NEAREST);

	//if (!InitGUI()){
	//	return false;
	//}

	return true;
}

bool DeferredApp::InitGUI(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(APP_WIDTH, APP_HEIGHT);

	return true;
}

void DeferredApp::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;


	mat4x4 rotation = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(rotation));

	vec3 movement[64];

	for (int potRow = 0; potRow < 8; potRow++){
		for (int potCol = 0; potCol < 8; potCol++){
			int index = potRow * 8 + potCol;
			positions[index] = vec3(potCol * 15.0f - 60.0f, 0.0f, potRow * 10.0f - 40.0f);
		}
	}

	string firstStr = "ms per frame: ";
	string time = to_string(deltaTime);


	mat4x4 translationMatrix = rotation;
	vec2 screenSize((float)APP_WIDTH, (float)APP_HEIGHT);
	
	m_deferredRenderer.SetUniformsFirstTime(screenSize, normalMatrix, translationMatrix);

	lastFrame = currentFrame = glfwGetTime();

	float rotationAmount = 0.0f;

	do{
		float cosAmount = cosf(rotationAmount);
		float sinAmount = sinf(rotationAmount);

		rotationAmount += 0.05f;


		for (int light = 0; light < NUM_POINT_LIGHTS; light++){
			m_lights.position[light].x = lightPos[light].x + m_radii[light] * cosAmount;
			m_lights.position[light].z = lightPos[light].z + m_radii[light] * sinAmount;

			m_lights.position[light].y = lightPos[light].y;
		}


		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderDeferred(movement,NUM_POINT_LIGHTS);

		time = to_string(deltaTime * 1000.0f);
		//TwDraw();

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
};


void DeferredApp::RenderDeferred(const vec3* teapotPositions,int numLights){
	
	m_deferredRenderer.RenderDeferred(teapotPositions, m_camera.projectionMatrix * m_camera.viewMatrix, m_lights.position, m_lights.color,
										m_camera.pos, numLights, RenderGeometry);

	
}


void DeferredApp::ShutDown(){

	DestroyMesh(teapotMesh);
	//DestroyMesh(m_cubeMesh);
	glDeleteVertexArrays(1, &groundPlaneBuffer);

	glDeleteShader(cubemapShader.GetHandle());
	glDeleteTextures(1, &cubeMap);

	glDeleteTextures(1, &m_teaTexture);
};

void DeferredApp::RenderGeometry(GLint shaderHandle,mat4& viewProjection){
	vec3 diff = vec3(0.4f, 0.4f, 0.4f);

	mat4x4 scaleMatrix1, cubemapScaleMatrix, identity;
	scaleMatrix1 = scale(scaleMatrix1, vec3(0.05f, 0.05f, 0.05f));
	glUseProgram(shaderHandle);

	GLint cameraUniform = glGetUniformLocation(shaderHandle, "vpMatrix");
	GLint scaleUniform  = glGetUniformLocation(shaderHandle, "scaleMatrix");
	GLint diffUniform   = glGetUniformLocation(shaderHandle, "materialDiffuse");
	GLint texUniform	= glGetUniformLocation(shaderHandle, "myTexture");
	GLint normalUniform = glGetUniformLocation(shaderHandle, "normalMap");

	glUniformMatrix4fv(scaleUniform, 1, GL_FALSE, &scaleMatrix1[0][0]);
	glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, &viewProjection[0][0]);
	glUniform3fv(diffUniform, 1, &diff[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_teaTexture);
	glUniform1i(texUniform,0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	glUniform1i(normalUniform, 1);

	RenderInstancedStaticMesh(teapotMesh, &positions[0]);

	glUniformMatrix4fv(scaleUniform,1, GL_FALSE, &identity[0][0]);
	//glUniform3fv(gBufferUniforms.materialUniforms.diffuseUniform, 1, &diff[0]);
	glBindVertexArray(groundPlaneBuffer);

	glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DeferredApp::RenderCubemap(mat4& viewProjection,vec3& camPos){

	mat4 identity;

	glUseProgram(cubemapShader.GetHandle());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		glUniformMatrix4fv(cubemapShader.GetWVPMatrix(), 1, GL_FALSE, &(viewProjection * translate(identity, camPos))[0][0]);
		glUniform1i(cubemapShader.GetSampler(), 0);
		glCullFace(GL_FRONT);
		RenderStaticMesh(cubeMesh);
		glCullFace(GL_BACK);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
}
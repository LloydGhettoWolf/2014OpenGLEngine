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

	if (!InitStaticMesh(m_teapotMesh, "teapot.obj", "teapot\\", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	vec3 movement[64];

	for (int potRow = 0; potRow < 8; potRow++){
		for (int potCol = 0; potCol < 8; potCol++){
			int index = potRow * 8 + potCol;
			m_positions[index] = vec3(potCol * 15.0f - 60.0f, 0.0f, potRow * 10.0f - 40.0f);
		}
	}

	for (unsigned int meshNum = 0; meshNum < m_teapotMesh.m_numMeshes; ++meshNum){
		CreateInstancedAttrib(5, m_teapotMesh.m_meshData[meshNum].m_vertexBuffer, m_teapotMesh.m_meshData[meshNum].m_instancedDataBuffer, 64);
		glBindBuffer(GL_ARRAY_BUFFER, m_teapotMesh.m_meshData[meshNum].m_instancedDataBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * NUM_MESHES, m_positions);
	}


	if (!InitStaticMesh(cubeMesh, "cube.obj", "meshes\\")){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	m_camera = CreateCamera(vec3(0.0f, 0.0f, -40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),3.0f,500.0f,APP_WIDTH,APP_HEIGHT,45.0f);

	m_groundPlaneBuffer = CreateGroundPlaneData();


	m_lights.position		= new vec3[NUM_POINT_LIGHTS];
	m_lights.color			= new vec3[NUM_POINT_LIGHTS];
	m_lights.attData	    = new Attenuation[NUM_POINT_LIGHTS];

	for (int light = 0; light < NUM_POINT_LIGHTS; light++){
		m_lights.attData[light].constantAtt = 1.0f;
		m_lights.attData[light].linearAtt = 0.1f;
		m_lights.attData[light].expAtt = 0.7f;


		float randomX = (float)(rand() % 120) - 60.0f;
		float randomZ = (float)(rand() % 90) - 45.0f;
		float randomY = (float)(rand() % 5);

		m_lightPos[light] = vec3(randomX, randomY, randomZ);

		float randomR = (float)(rand() % 100) / 100.0f;
		float randomG = (float)(rand() % 100) / 100.0f;
		float randomB = (float)(rand() % 100) / 100.0f;

		m_lights.color[light] = vec3(randomR, randomG, randomB);

		m_radii[light] = (float)(rand() % 20) - 10.0f;
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


	m_teaTexture    = CreateTexture("Textures\\160.jpg", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	m_normalTexture = CreateTexture("Textures\\160_norm.jpg", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

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



	//get time display
	string firstStr = "ms per frame: ";
	string time = to_string(deltaTime);

	//set up initial transforms
	mat4x4 translationMatrix = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(translationMatrix));
	vec2 screenSize((float)APP_WIDTH, (float)APP_HEIGHT);
	
	m_deferredRenderer.SetUniformsFirstTime(screenSize, normalMatrix, translationMatrix);

	lastFrame = currentFrame = (float)glfwGetTime();

	float rotationAmount = 0.0f;

	do{
		float cosAmount = cosf(rotationAmount);
		float sinAmount = sinf(rotationAmount);

		rotationAmount += 0.05f;


		for (int light = 0; light < NUM_POINT_LIGHTS; light++){
			m_lights.position[light].x = m_lightPos[light].x + m_radii[light] * cosAmount;
			m_lights.position[light].z = m_lightPos[light].z + m_radii[light] * sinAmount;

			m_lights.position[light].y = m_lightPos[light].y;
		}


		deltaTime = currentFrame - lastFrame;

		RenderDeferred(NUM_POINT_LIGHTS);

		time = to_string(deltaTime * 1000.0f);

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


void DeferredApp::RenderDeferred(int numLights){
	
	m_deferredRenderer.BindFBO();
	m_deferredRenderer.PrepareGeometry();
	RenderGeometry();
	RenderCubemap();
	m_deferredRenderer.RenderLights(m_camera.projectionMatrix * m_camera.viewMatrix, m_lights, m_camera.pos, NUM_POINT_LIGHTS);
	m_deferredRenderer.UnbindFBO();
	m_deferredRenderer.PresentToScreen();
}


void DeferredApp::ShutDown(){

	glDeleteVertexArrays(1, &m_groundPlaneBuffer);

	glDeleteTextures(1, &cubeMap);
	glDeleteTextures(1, &m_teaTexture);
	glDeleteTextures(1, &m_normalTexture);
};

void DeferredApp::RenderGeometry(){
	vec3 diff = vec3(0.4f, 0.4f, 0.4f);

	mat4x4 scaleMatrix1, cubemapScaleMatrix, identity;
	scaleMatrix1 = scale(scaleMatrix1, vec3(0.05f, 0.05f, 0.05f));

	GLint shaderHandle = m_deferredRenderer.GetGBufferShaderHandle();

	glUseProgram(shaderHandle);

	GLint viewProjUniform = glGetUniformLocation(shaderHandle, "vpMatrix");
	GLint scaleUniform    = glGetUniformLocation(shaderHandle, "scaleMatrix");
	GLint diffUniform     = glGetUniformLocation(shaderHandle, "materialDiffuse");
	GLint texUniform	  = glGetUniformLocation(shaderHandle, "myTexture");
	GLint normalUniform   = glGetUniformLocation(shaderHandle, "normalMap");

	glUniformMatrix4fv(scaleUniform, 1, GL_FALSE, &scaleMatrix1[0][0]);
	glUniformMatrix4fv(viewProjUniform, 1, GL_FALSE, &(m_camera.projectionMatrix * m_camera.viewMatrix)[0][0]);
	glUniform3fv(diffUniform, 1, &diff[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_teaTexture);
	glUniform1i(texUniform,0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	glUniform1i(normalUniform, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderInstancedStaticMeshComponent(m_teapotMesh.m_meshData[0],64);
	RenderInstancedStaticMeshComponent(m_teapotMesh.m_meshData[1], 64);

	glUniformMatrix4fv(scaleUniform,1, GL_FALSE, &identity[0][0]);
	glBindVertexArray(m_groundPlaneBuffer);

	glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void DeferredApp::RenderCubemap(){

	mat4 identity;
	mat4 viewProjection = m_camera.projectionMatrix * m_camera.viewMatrix;
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glUseProgram(cubemapShader.GetHandle());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		GLint uniform = cubemapShader.GetWVPMatrix();
		glUniformMatrix4fv(uniform, 1, GL_FALSE, &(viewProjection * translate(identity, m_camera.pos))[0][0]);
		uniform = cubemapShader.GetSampler();
		glUniform1i(uniform, 0);
		glCullFace(GL_FRONT);
			RenderStaticMeshComponent(cubeMesh.m_meshData[0]);
		glCullFace(GL_BACK);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
}
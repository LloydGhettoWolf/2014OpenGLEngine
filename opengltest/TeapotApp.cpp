//TeapotApp.cpp
#include <gtc\matrix_transform.hpp>
#include <gtc\matrix_inverse.hpp>

#include <iostream>
#include <math.h>
#include <IL\il.h>
#include "VertexArray.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "VertexTypes.h"
#include "TeapotApp.h"
#include "GroundPlane.h"
#include "Defines.h"
#include "CubeMap.h"


bool TeapotApp::Init(){

	StandardInit();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (!m_teapotShader.CreateForwardShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!m_cubemapShader.CreateCubemapShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}
	
	if (!InitStaticMesh(m_teapotMesh, "simpleTeapot.obj", "teapot\\")){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	if (!InitStaticMesh(m_cubeMesh, "cube.obj", "meshes\\")){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	m_camera = CreateCamera(vec3(0.0f,0.0f,-40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	glfwSetWindowSizeCallback(MyResize);

	m_groundPlaneBuffer = CreateGroundPlaneData();

	
	for (int light = 0; light < NUM_POINT_LIGHTS; light++){
		m_lights.constantAtt[light] = 0.0f;
		m_lights.linearAtt[light] = 0.0f;
		m_lights.expAtt[light] = 0.7f;


		float randomX = (float)(rand() % 120) - 60.0f;
		float randomZ = (float)(rand() % 90) - 45.0f;
		float randomY = (float)(rand() % 5);

		lightPos[light] = vec3(randomX, randomY, randomZ);

		float randomR = (float)(rand() % 100)/100.0;
		float randomG = (float)(rand() % 100)/100.0;
		float randomB = (float)(rand() % 100)/100.0;

		m_lights.color[light] = vec3(randomR,randomG,randomB);

		m_radii[light] = (float)(rand() % 20) -10.0;
	}



	//cubemap
	std::vector<string> cubemapFilenames;
	cubemapFilenames.push_back("CubeMaps\\Park\\posx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\negx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\posy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\negy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\posz.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\negz.jpg");
	m_cubeMap = CreateCubeMap(cubemapFilenames);

	m_material.ambient = vec3(0.1f,0.1f,0.1f);
	m_material.diffuse = vec3(0.5f, 0.4f, 0.5f);
	m_material.shininess = 64.0f;

	//if (!InitGUI()){
	//	return false;
	//}

	return true;
}

bool TeapotApp::InitGUI(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(APP_WIDTH, APP_HEIGHT);

	myBar = TwNewBar("MaterialProperties");
	TwAddVarRW(myBar, "shininess", TW_TYPE_FLOAT, &m_material.shininess, "min=0.0f max=512.0f step=2.0f");
	TwDefine(" MaterialProperties size='200 160'");

	//define a 3 component vector for spec, ambient, diff values in the ui
	TwStructMember members[] = {
		{ "r", TW_TYPE_FLOAT, offsetof(vec3, x), "min=0.0f max=1.0f step=0.01f" },
		{ "g", TW_TYPE_FLOAT, offsetof(vec3, y), "min=0.0f max=1.0f step=0.01f" },
		{ "b", TW_TYPE_FLOAT, offsetof(vec3, z), "min=0.0f max=1.0f step=0.01f" },
	};

	TwType colType = TwDefineStruct("ColType", members, 3, sizeof(vec3), NULL, NULL);
	TwAddVarRW(myBar, "Diffuse",  colType, &m_material.diffuse[0],  NULL);
	TwAddVarRW(myBar, "LightDiffuse", colType, &m_lights.color[0], NULL);
	TwAddVarRW(myBar, "Specular", colType, &m_material.specular[0], NULL);
	TwAddVarRW(myBar, "Ambient",  colType, &m_material.ambient[0],  NULL);
	return true;
}

void TeapotApp::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;


	mat4x4 rotation = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(rotation));

	vec3 movement[64];

	for (int potRow = 0; potRow < 8; potRow++){
		for (int potCol = 0; potCol < 8; potCol++){
			int index = potRow * 8 + potCol;
			movement[index] = vec3(potCol * 15.0f - 60.0f,0.0f,potRow * 10.0f - 40.0f);
		}
	}

	string firstStr = "ms per frame: ";
	string time     = to_string(deltaTime);

	m_teapotShader.SetUniforms(m_camera.projectionMatrix,m_lights.color[0],NUM_POINT_LIGHTS);


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
		
	    RenderForward(&m_lights.position[0], movement);
		
		time = to_string(deltaTime * 1000.0f);
		//TwDraw();

		glfwSwapBuffers();

		ReadMouse();
		ReadKeys();
		MoveCameraForward(m_camera, deltaTime * m_moveForwardAmount);
		MoveCameraVertically(m_camera, deltaTime *  m_moveUpAmount);
		MoveCameraHorizontally(m_camera, deltaTime *  m_moveSidewaysAmount);

		lastFrame = currentFrame;
		currentFrame = glfwGetTime();
	} while (m_run);

	ShutDown();
};

void TeapotApp::RenderForward(const vec3* lightPositions,const vec3* teapotPositions){
	vec3 diff = vec3(0.4f, 0.4f, 0.4f);

	mat4x4 scaleMatrix, identity, rotation;
	scaleMatrix = scale(scaleMatrix, vec3(0.04f, 0.04f, 0.04f));
	rotation    = rotate(identity, 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat4x4 worldMatrix = rotation *scaleMatrix;
	mat3x3 normalMatrix = inverseTranspose(mat3(worldMatrix));
	mat4x4 viewProjection = m_camera.projectionMatrix * m_camera.viewMatrix;


	glUseProgram(m_teapotShader.GetHandle());
		ForwardShaderUniforms teapotUniforms = m_teapotShader.GetUniforms();
		m_teapotShader.UpdateUniforms(worldMatrix, normalMatrix, m_camera.viewMatrix, m_camera.pos, m_lights.position[0], 1);

		glUniform3fv(teapotUniforms.matUni.diffuseUniform,  1, &m_material.diffuse[0]);
		glUniform3fv(teapotUniforms.matUni.ambientUniform,  1, &m_material.ambient[0]);
		glUniform3fv(teapotUniforms.matUni.specularUniform, 1, &m_material.specular[0]);
		glUniform3fv(teapotUniforms.lightColUniform,        1, &m_lights.color[0][0]);
		glUniform1f(teapotUniforms.matUni.shininessUniform, m_material.shininess);


		RenderStaticMeshComponent(m_teapotMesh.m_meshData[0]);
		
		glUniformMatrix4fv(teapotUniforms.worldMatrixUniform, 1, GL_FALSE, &identity[0][0]);
		glUniform1i(teapotUniforms.instancedUniform, 0);
		glBindVertexArray(m_groundPlaneBuffer);
		m_teapotShader.UpdateUniforms(identity, normalMatrix, m_camera.viewMatrix, m_camera.pos, m_lights.position[0], 1);
		glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
	glUseProgram(0);

	glUseProgram(m_cubemapShader.GetHandle());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
		glUniformMatrix4fv(m_cubemapShader.GetWVPMatrix(), 1, GL_FALSE, &(m_camera.projectionMatrix * m_camera.viewMatrix * translate(identity,m_camera.pos) )[0][0]);
		glUniform1i(m_cubemapShader.GetSampler(), 0);
		glCullFace(GL_FRONT);
		RenderStaticMeshComponent(m_cubeMesh.m_meshData[0]);
		glCullFace(GL_BACK);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
}

void TeapotApp::ShutDown(){

	glDeleteVertexArrays(1, &m_groundPlaneBuffer);

	glDeleteShader(m_teapotShader.GetHandle());
	glDeleteShader(m_cubemapShader.GetHandle());

	glDeleteTextures(1, &m_cubeMap);
};
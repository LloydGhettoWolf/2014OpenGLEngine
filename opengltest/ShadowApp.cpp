//ShadowApp.cpp
#include "ShadowApp.h"
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


bool ShadowApp::Init(){

	StandardInit();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (!m_shadowShader.CreateShadowShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	InitStaticMesh(m_teapotMesh, "buddha.obj", "meshes\\", 1);

	m_centerOffset = (m_teapotMesh.m_boundingBoxMax.y - m_teapotMesh.m_boundingBoxMin.y) / 2.0f;

	m_camera = CreateCamera(vec3(0.0f, 0.0f, -40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	m_lightPos = vec3(60.0f, 30.0f, 10.0f);
	m_lightDir = vec3(1.0f, -1.0f, 0.0f);


	m_shadowCamera = CreateCamera(m_lightPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_shadowCamera.projectionMatrix = glm::perspective(30.0f, APP_WIDTH / APP_HEIGHT, 5.0f, 3500.0f);

	glfwSetWindowSizeCallback(MyResize);

	m_groundPlaneBuffer = CreateGroundPlaneData();

	m_material.ambient = vec3(0.1f, 0.1f, 0.1f);
	m_material.diffuse = vec3(0.5f, 0.4f, 0.5f);
	m_material.shininess = 64.0f;


	m_biasMatrix = glm::mat4(0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0);


	if (!CreateDepthTextureBasic(m_fbo,m_depthTexture)){
		return false;
	}


	if (!InitGUI()){
		return false;
	}

	return true;
}

bool ShadowApp::InitGUI(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(APP_WIDTH, APP_HEIGHT);

	MaterialGUI = TwNewBar("MaterialProperties");
	TwAddVarRW(MaterialGUI, "shininess", TW_TYPE_FLOAT, &m_material.shininess, "min=0.0f max=512.0f step=2.0f");
	TwAddVarRW(MaterialGUI, "Rotation", TW_TYPE_QUAT4F, &m_model1Rotation, "");
	TwDefine(" MaterialProperties size='200 160'");

	//define a 3 component vector for spec, ambient, diff values in the ui
	TwStructMember members[] = {
		{ "r", TW_TYPE_FLOAT, offsetof(vec3, x), "min=0.0f max=1.0f step=0.01f" },
		{ "g", TW_TYPE_FLOAT, offsetof(vec3, y), "min=0.0f max=1.0f step=0.01f" },
		{ "b", TW_TYPE_FLOAT, offsetof(vec3, z), "min=0.0f max=1.0f step=0.01f" },
	};

	TwType colType = TwDefineStruct("ColType", members, 3, sizeof(vec3), NULL, NULL);
	TwAddVarRW(MaterialGUI, "Diffuse", colType, &m_material.diffuse[0], NULL);
	TwAddVarRW(MaterialGUI, "Specular", colType, &m_material.specular[0], NULL);
	TwAddVarRW(MaterialGUI, "Ambient", colType, &m_material.ambient[0], NULL);

	LightDir = TwNewBar("LightDir");
	TwAddVarRW(LightDir, "light direction", TW_TYPE_DIR3F, &m_lightPos[0], "");
	TwDefine("LightDir size='200 200' position='20 400'");
	return true;
}

void ShadowApp::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;


	mat4x4 rotation = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(rotation));

	m_shadowShader.SetShadowUniforms(m_camera.projectionMatrix);
	m_shadowShader.SetDepthUniforms(m_shadowCamera.projectionMatrix);

	lastFrame = currentFrame = glfwGetTime();

	float rotationAmount = 0.0f;

	do{
		float cosAmount = cosf(rotationAmount);
		float sinAmount = sinf(rotationAmount);

		rotationAmount += 0.005f;

		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		vec3 diff = vec3(0.4f, 0.4f, 0.4f);
		
		mat4 identity;
		mat4x4 fromQuat = mat4_cast(m_model1Rotation);
		m_scaleMatrix = scale(identity, vec3(0.5f, 0.5f, 0.5f));
		m_worldMatrix =  fromQuat * rotate(identity, 180.0f, vec3(0.0f, 1.0f, 0.0f)) * m_scaleMatrix * translate(identity, vec3(0.0f, -m_centerOffset, 0.0f));

		m_normalMatrix = inverseTranspose(mat3(m_worldMatrix));

		RenderShadow();
		RenderMeshes();

		TwDraw();

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


void ShadowApp::RenderShadow(){
	mat4 identity;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		glUseProgram(m_shadowShader.GetDepthHandle());
			DepthShaderUniforms shadowUniforms = m_shadowShader.GetDepthUniforms();

			m_shadowShader.UpdateDepthUniforms(m_worldMatrix,m_shadowCamera.viewMatrix);

			RenderStaticMesh(m_teapotMesh);

			m_shadowShader.UpdateDepthUniforms(identity, m_shadowCamera.viewMatrix);
			glBindVertexArray(m_groundPlaneBuffer);
			glCullFace(GL_BACK);
			glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		glUseProgram(0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void ShadowApp::RenderMeshes(){

	mat4 identity;

	glUseProgram(m_shadowShader.GetShadowHandle());
		
		ShadowShaderUniforms shadowUniforms = m_shadowShader.GetShadowUniforms();
		m_shadowShader.UpdateShadowUniforms(m_worldMatrix, m_normalMatrix, m_camera.viewMatrix, m_camera.pos,m_shadowMatrix,m_lightPos);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(shadowUniforms.shadowTextureUniform, 0);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);

		glUniform3fv(shadowUniforms.matUni.diffuseUniform, 1, &m_material.diffuse[0]);
		glUniform3fv(shadowUniforms.matUni.ambientUniform, 1, &m_material.ambient[0]);
		glUniform3fv(shadowUniforms.matUni.specularUniform, 1, &m_material.specular[0]);
		glUniform1f(shadowUniforms.matUni.shininessUniform, m_material.shininess);

		m_shadowMatrix = m_biasMatrix * m_shadowCamera.projectionMatrix * m_shadowCamera.viewMatrix * m_worldMatrix;

		RenderStaticMesh(m_teapotMesh);

		m_shadowMatrix = m_biasMatrix * m_shadowCamera.projectionMatrix * m_shadowCamera.viewMatrix;
		glBindVertexArray(m_groundPlaneBuffer);
		m_shadowShader.UpdateShadowUniforms(identity, mat3(identity), m_camera.viewMatrix, m_camera.pos, m_shadowMatrix, m_lightPos);
		glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	glUseProgram(0);
}

void ShadowApp::ShutDown(){
	DestroyMesh(m_teapotMesh);
};
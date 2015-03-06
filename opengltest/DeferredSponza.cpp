//DeferredSponza.cpp
#include "DeferredSponza.h"
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


bool DeferredSponza::Init(){

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

	if (!InitStaticMesh(m_sponzaMesh, "sponza.obj", "meshes\\sponza_obj\\", aiProcess_Triangulate | aiProcess_CalcTangentSpace)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}


	if (!InitStaticMesh(cubeMesh, "cube.obj", "meshes\\")){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	m_camera = CreateCamera(vec3(5.0f, 2.0f, 0.0f), vec3(0.0f, 2.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(60.0f, 1024.0f / 768.0f, 1.0f, 1000.0f);

	glfwSetWindowSizeCallback(MyResize);


	m_lights.position		= new vec3[NUM_POINT_LIGHTS];
	m_lights.color			= new vec3[NUM_POINT_LIGHTS];
	m_lights.constantAtt	= new float[NUM_POINT_LIGHTS];
	m_lights.expAtt			= new float[NUM_POINT_LIGHTS];
	m_lights.linearAtt		= new float[NUM_POINT_LIGHTS];

	for (int light = 0; light < NUM_POINT_LIGHTS; light++){
		m_lights.constantAtt[light] = 0.0f;
		m_lights.linearAtt[light] = 0.0f;
		m_lights.expAtt[light] = 0.7f;


		float randomX = (float)(rand() % 400) - 200.0f;
		float randomZ = (float)(rand() % 150) - 75.0f;
		float randomY = (float)(rand() % 100);

		m_lightPos[light] = vec3(randomX, randomY, randomZ);

		float randomR = (float)(rand() % 100) / 100.0;
		float randomG = (float)(rand() % 100) / 100.0;
		float randomB = (float)(rand() % 100) / 100.0;

		m_lights.color[light] = vec3(randomR, randomG, randomB);

		m_radii[light] = (float)(rand() % 10);
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




	//if (!InitGUI()){
	//	return false;
	//}

	return true;
}

bool DeferredSponza::InitGUI(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(APP_WIDTH, APP_HEIGHT);

	return true;
}

void DeferredSponza::Run(){

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

	lastFrame = currentFrame = glfwGetTime();

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


void DeferredSponza::RenderDeferred(int numLights){

	m_deferredRenderer.BindFBO();
	m_deferredRenderer.PrepareGeometry();
	RenderSponza();
	//RenderCubemap();
	m_deferredRenderer.RenderLights(m_camera.projectionMatrix * m_camera.viewMatrix, m_lights.position, m_lights.color, m_camera.pos, NUM_POINT_LIGHTS);
	m_deferredRenderer.UnbindFBO();
	m_deferredRenderer.PresentToScreen();
}


void DeferredSponza::ShutDown(){

	
};


void DeferredSponza::RenderSponza(){

	GLuint shaderProg = m_deferredRenderer.GetGBufferShaderHandle();

	MaterialUniforms matUni;

	GLint perspectiveMatrixUniform = glGetUniformLocation(shaderProg, "vpMatrix");

	GLint textureUniform  = glGetUniformLocation(shaderProg, "myTexture");
	GLint normalUniform   = glGetUniformLocation(shaderProg, "normalMap");
	GLint specMapUniform  = glGetUniformLocation(shaderProg, "specularMap");
	GLint alphaMapUniform = glGetUniformLocation(shaderProg, "alphaMap");

	matUni.diffuseUniform   = glGetUniformLocation(shaderProg, "materialDiffuse");
	matUni.ambientUniform   = glGetUniformLocation(shaderProg, "materialAmbient");

	GLint scaleUniform = glGetUniformLocation(shaderProg, "scaleMatrix");

	GLint usesNormalMap    = glGetUniformLocation(shaderProg, "useNormalMap");
	GLint usesAlphaMap     = glGetUniformLocation(shaderProg, "useAlphaMap");

	
	
	mat4 scaleMatrix;
	scaleMatrix = scale(scaleMatrix, vec3(0.15f, 0.15f, 0.15f));

	glUseProgram(shaderProg);
		glUniformMatrix4fv(perspectiveMatrixUniform, 1, GL_FALSE, &(m_camera.projectionMatrix * m_camera.viewMatrix)[0][0]);
		glUniformMatrix4fv(scaleUniform, 1, GL_FALSE, &scaleMatrix[0][0]);
		glUniform1i(textureUniform, 0);
		glUniform1i(normalUniform, 1);
		glUniform1i(specMapUniform, 2);
		glUniform1i(alphaMapUniform, 3);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (int mesh = 0; mesh < m_sponzaMesh.m_numMeshes; ++mesh){
		int materialIndex = m_sponzaMesh.m_meshData[mesh].m_materialIndex;
		MaterialInfo mat = m_sponzaMesh.m_materialData[materialIndex];

		glUniform3fv(matUni.diffuseUniform, 1, &mat.m_material.diffuse[0]);
		glUniform3fv(matUni.ambientUniform, 1, &mat.m_material.ambient[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mat.m_texture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mat.m_normalMap);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mat.m_specMap);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mat.m_alphaMap);


		if (mat.m_normalMap != 0){
			glUniform1i(usesNormalMap, 1);
		}else{
			glUniform1i(usesNormalMap, 0);
		}

		if (mat.m_alphaMap != 0){
			glUniform1i(usesAlphaMap, 1);
		}else{
			glUniform1i(usesAlphaMap, 0);
		}

		RenderStaticMeshComponent(m_sponzaMesh.m_meshData[mesh]);
	}

	glUseProgram(0);
}

void DeferredSponza::RenderCubemap(){

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
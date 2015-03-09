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

void CalcSphereDistance(const PointLightData& pLight, int index);


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
	m_camera.projectionMatrix = glm::perspective(30.0f, APP_WIDTH / APP_HEIGHT, 1.0f, 1000.0f);




	m_lights.position		= new vec3[NUM_POINT_LIGHTS];
	m_lights.color			= new vec3[NUM_POINT_LIGHTS];
	m_lights.attData		= new Attenuation[NUM_POINT_LIGHTS];
	m_lights.effectiveDist  = new float[NUM_POINT_LIGHTS];

	for (int light = 0; light < NUM_POINT_LIGHTS; light++){
		m_lights.attData[light].constantAtt = 1.0f;
		m_lights.attData[light].linearAtt   = 0.1f;
		m_lights.attData[light].expAtt      = 0.01f;


		float randomX = (float)(rand() % 600) - 300.5f;
		float randomZ = (float)(rand() % 300) - 150.0f;
		float randomY = (float)(rand() % 100);

		m_lightPos[light] = vec3(randomX, randomY, randomZ);

		float randomR = (float)(rand() % 100) / 100.0;
		float randomG = (float)(rand() % 100) / 100.0;
		float randomB = (float)(rand() % 100) / 100.0;

		m_lights.color[light] = vec3(randomR, randomG, randomB);

		m_radii[light] = (float)(rand() % 10);

		CalcSphereDistance(m_lights, light);
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
		//return false;
	//}

	return true;
}

bool DeferredSponza::InitGUI(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(APP_WIDTH, APP_HEIGHT);
	FPSGUI = TwNewBar("FPSBar");
	TwAddVarRO(FPSGUI, "MsPerFrame", TW_TYPE_FLOAT,&m_time,"");
	TwAddVarRO(FPSGUI, "num visible lights", TW_TYPE_INT32, &m_numVisibleLights, "");
	TwAddVarRO(FPSGUI, "num visible meshes", TW_TYPE_INT32, &m_numVisibleMeshes, "");
	TwAddVarRO(FPSGUI, "pos", TW_TYPE_FLOAT, &m_camera.pos.x, "");
	return true;
}

void DeferredSponza::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;

	//get time display
	m_time = deltaTime;

	//set up initial transforms
	mat4x4 translationMatrix = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(translationMatrix));
	vec2 screenSize((float)APP_WIDTH, (float)APP_HEIGHT);

	m_deferredRenderer.SetUniformsFirstTime(screenSize, normalMatrix, translationMatrix);

	lastFrame = currentFrame = glfwGetTime();

	float rotationAmount = 0.0f;

	vector<int> visibleLights;
	visibleLights.reserve(NUM_POINT_LIGHTS);

	do{
		float cosAmount = cosf(rotationAmount);
		float sinAmount = sinf(rotationAmount);

		rotationAmount += 0.05f;

		Extract(m_frustum, m_camera.pos, m_camera.viewMatrix, m_camera.projectionMatrix);

		for (int light = 0; light < NUM_POINT_LIGHTS; light++){
			m_lights.position[light].x = m_lightPos[light].x + m_radii[light] * cosAmount;
			m_lights.position[light].z = m_lightPos[light].z + m_radii[light] * sinAmount;

			m_lights.position[light].y = m_lightPos[light].y;

			vec4 testPos =  vec4(m_lights.position[light],1.0);


			if (TestSphere(m_frustum, m_lights.position[light], m_lights.effectiveDist[light])){
				visibleLights.push_back(light);
			}
		}


		deltaTime = currentFrame - lastFrame;


		RenderDeferred(NUM_POINT_LIGHTS);

		m_time = deltaTime * 1000.0f;
		m_numVisibleLights = visibleLights.size();

		//TwDraw();

		glfwSwapBuffers();

		ReadMouse();
		ReadKeys();
		MoveCameraForward(m_camera, deltaTime      * m_moveForwardAmount);
		MoveCameraVertically(m_camera, deltaTime *  m_moveUpAmount);
		MoveCameraHorizontally(m_camera, deltaTime *  m_moveSidewaysAmount);

		lastFrame = currentFrame;
		currentFrame = glfwGetTime();

		visibleLights.clear();
	} while (m_run);

	ShutDown();
};


void DeferredSponza::RenderDeferred(int numLights){

	m_deferredRenderer.BindFBO();
	m_deferredRenderer.PrepareGeometry();
	RenderSponza();
	//RenderCubemap();
	m_deferredRenderer.RenderLights(m_camera.projectionMatrix * m_camera.viewMatrix, m_lights, m_camera.pos, NUM_POINT_LIGHTS);
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
	scaleMatrix = scale(scaleMatrix, vec3(0.2f, 0.2f, 0.2f));

	glUseProgram(shaderProg);
		glUniformMatrix4fv(perspectiveMatrixUniform, 1, GL_FALSE, &(m_camera.projectionMatrix * m_camera.viewMatrix)[0][0]);
		glUniformMatrix4fv(scaleUniform, 1, GL_FALSE, &scaleMatrix[0][0]);
		glUniform1i(textureUniform, 0);
		glUniform1i(normalUniform, 1);
		glUniform1i(specMapUniform, 2);
		glUniform1i(alphaMapUniform, 3);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_numVisibleMeshes = 0;

	for (int mesh = 0; mesh < m_sponzaMesh.m_numMeshes; ++mesh){


		//if (TestBox(m_frustum, m_sponzaMesh.m_meshData[mesh].)
		
		m_numVisibleMeshes++;

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

void CalcSphereDistance(const PointLightData& pLight, int index){
	float maxChan = std::max(std::max(pLight.color[index].r, pLight.color[index].g), pLight.color[index].b);

	float ret = (-pLight.attData[index].linearAtt + sqrtf(pLight.attData[index].linearAtt * pLight.attData[index].linearAtt
		- 4.0f *pLight.attData[index].expAtt * (pLight.attData[index].expAtt - 256.0f * maxChan))) / (2.0f*pLight.attData[index].expAtt);

	pLight.effectiveDist[index] = ret;
}
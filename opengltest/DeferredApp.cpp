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


	if (!m_deferredShader.CreateDeferredShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!m_cubemapShader.CreateCubemapShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!InitStaticMesh(m_teapotMesh, "simpleTeapot.obj", "teapot\\", 64)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	if (!InitStaticMesh(m_sphereMesh, "sphere.obj", "meshes\\", 1)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	if (!InitStaticMesh(m_cubeMesh, "cube.obj", "meshes\\", 1)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	m_camera = CreateCamera(vec3(0.0f, 0.0f, -40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	glfwSetWindowSizeCallback(MyResize);

	m_groundPlaneBuffer = CreateGroundPlaneData();

	if (!CreateGBuffer()){
		return false;
	}

	vec3 screenQuad[6];

	screenQuad[0] = vec3(1.0f, 1.0f, 0.0f);
	screenQuad[1] = vec3(-1.0f, 1.0f, 0.0f);
	screenQuad[2] = vec3(-1.0f, -1.0f, 0.0f);
	screenQuad[3] = vec3(1.0f, 1.0f, 0.0f);
	screenQuad[4] = vec3(-1.0f, -1.0f, 0.0f);
	screenQuad[5] = vec3(1.0f, -1.0f, 0.0f);


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


	glGenBuffers(1, &m_quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 6, screenQuad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//cubemap
	std::vector<string> cubemapFilenames;
	cubemapFilenames.push_back("CubeMaps\\Park\\posx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\negx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\posy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\negy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\posz.jpg");
	cubemapFilenames.push_back("CubeMaps\\Park\\negz.jpg");
	m_cubeMap = CreateCubeMap(cubemapFilenames);

	m_material.ambient = vec3(0.1f, 0.1f, 0.1f);
	m_material.diffuse = vec3(0.5f, 0.4f, 0.5f);
	m_material.shininess = 64.0f;

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
			m_positions[index] = vec3(potCol * 15.0f - 60.0f, 0.0f, potRow * 10.0f - 40.0f);
		}
	}

	string firstStr = "ms per frame: ";
	string time = to_string(deltaTime);


	glUseProgram(m_deferredShader.GetGBufferHandle());
	mat4x4 translationMatrix = rotation;
	GBufferShaderUniforms gBufferUniforms = m_deferredShader.GetGBufferUniforms();
	glUniformMatrix3fv(gBufferUniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(gBufferUniforms.rotationMatrixUniform, 1, GL_FALSE, &translationMatrix[0][0]);
	glUseProgram(0);


	glUseProgram(m_deferredShader.GetLightPassHandle());
	vec2 screenSize((float)APP_WIDTH, (float)APP_HEIGHT);
	glUniform2fv(m_deferredShader.GetLightPassUniforms().screenSizeUniform, 1, &screenSize[0]);
	glUseProgram(0);

	glUseProgram(m_deferredShader.GetQuadPassHandle());
	glUniform2fv(m_deferredShader.GetQuadPassUniforms().screenSizeUniform, 1, &screenSize[0]);
	glUseProgram(0);


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

		RenderDeferred(movement);

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


void DeferredApp::RenderDeferred(const vec3* teapotPositions){
	
	//geometry pass
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gBuffer.fboObject);
	RenderGBuffer();
	RenderLights();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	
	QuadPassShaderUniforms uniforms = m_deferredShader.GetQuadPassUniforms();

	glUseProgram(m_deferredShader.GetQuadPassHandle());
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(uniforms.bufferUniform, 0);
	glBindTexture(GL_TEXTURE_2D, m_gBuffer.finalTexture);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(uniforms.ambTextureUniform, 1);
	glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[2]);


	vec2 screenSize((float)APP_WIDTH, (float)APP_HEIGHT);

	glUniform2fv(uniforms.screenSizeUniform, 1, &screenSize[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

}

void DeferredApp::RenderLights(){

	mat4 viewProjection = m_camera.projectionMatrix * m_camera.viewMatrix;
	mat4 worldMatrix, identity;
	MaterialUniforms matuni;

	for (int light = 0; light < NUM_POINT_LIGHTS; light++){
		glDrawBuffer(GL_NONE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 0, 0);

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		glUseProgram(m_deferredShader.GetNULLPassHandle());

		float scale = 85.0f;//CalcSphereDistance(m_lights, light);
		worldMatrix = viewProjection * translate(identity, m_lights.position[light]) * glm::scale(identity, vec3(scale, scale, scale));
		glUniformMatrix4fv(m_deferredShader.GetNULLWVPMatrix(), 1, false, &worldMatrix[0][0]);
		RenderStaticMesh(m_sphereMesh);

		glUseProgram(0);

		//lighting pass
		glDisable(GL_DEPTH_TEST);
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDrawBuffer(GL_COLOR_ATTACHMENT4);

		glUseProgram(m_deferredShader.GetLightPassHandle());
		LightPassShaderUniforms lightPassUniforms = m_deferredShader.GetLightPassUniforms();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[0]);
		glUniform1i(lightPassUniforms.posTextureUniform, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[1]);
		glUniform1i(lightPassUniforms.normTextureUniform, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[2]);
		glUniform1i(lightPassUniforms.ambTextureUniform, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.textures[3]);
		glUniform1i(lightPassUniforms.diffTextureUniform, 3);

		worldMatrix = translate(identity, m_lights.position[light]) * glm::scale(identity, vec3(scale, scale, scale));
		glUniformMatrix4fv(lightPassUniforms.wvpMatrixUniform, 1, GL_FALSE, &(viewProjection * worldMatrix)[0][0]);
		glUniform3fv(lightPassUniforms.lightPosUniform, 1, &m_lights.position[light][0]);
		glUniform3fv(lightPassUniforms.lightColUniform, 1, &m_lights.color[light][0]);
		glUniform3fv(lightPassUniforms.eyePosUniform, 1, &m_camera.pos[0]);
		RenderStaticMesh(m_sphereMesh);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_BLEND);
		glCullFace(GL_BACK);
	}
	glDisable(GL_STENCIL_TEST);
}

void DeferredApp::RenderGBuffer(){
	vec3 diff = vec3(0.4f, 0.4f, 0.4f);

	mat4x4 scaleMatrix1, cubemapScaleMatrix, identity;
	cubemapScaleMatrix = scale(scaleMatrix1, vec3(200.0f, 200.0f, 200.0f));
	
	glDrawBuffer(GL_COLOR_ATTACHMENT4);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(NUM_MRT, DrawBuffers);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_deferredShader.GetGBufferHandle());
	GBufferShaderUniforms gBufferUniforms = m_deferredShader.GetGBufferUniforms();
	glUniformMatrix4fv(gBufferUniforms.scaleMatrixUniform, 1, GL_FALSE, &identity[0][0]);
	mat4 viewProjection = m_camera.projectionMatrix * m_camera.viewMatrix;
	glUniformMatrix4fv(gBufferUniforms.cameraMatrixUniform, 1, GL_FALSE, &viewProjection[0][0]);
	RenderInstancedStaticMesh(m_teapotMesh, &m_positions[0]);

	glUniformMatrix4fv(gBufferUniforms.scaleMatrixUniform, 1, GL_FALSE, &identity[0][0]);
	glUniform3fv(gBufferUniforms.materialUniforms.diffuseUniform, 1, &diff[0]);
	glBindVertexArray(m_groundPlaneBuffer);

	glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthMask(GL_FALSE);
	glUseProgram(0);
}

bool DeferredApp::CreateGBuffer(){
	return CreateGBufferData(m_gBuffer);
}

float DeferredApp::CalcSphereDistance(const PointLightData& pLight, int index){
	float maxChan = std::max(std::max(pLight.color[index].r, pLight.color[index].g), pLight.color[index].b);

	float ret = (-pLight.linearAtt[index] + sqrtf(pLight.linearAtt[index] * pLight.linearAtt[index]
		- 4.0f *pLight.expAtt[index] * (pLight.expAtt[index] - 256.0f * maxChan))) / (2.0f*pLight.expAtt[index]);

	return ret;
}

void DeferredApp::ShutDown(){

	DestroyMesh(m_teapotMesh);
	DestroyMesh(m_sphereMesh);
	DestroyMesh(m_cubeMesh);
	glDeleteVertexArrays(1, &m_groundPlaneBuffer);
	
	glDeleteShader(m_deferredShader.GetGBufferHandle());
	glDeleteShader(m_deferredShader.GetQuadPassHandle());
	glDeleteShader(m_deferredShader.GetLightPassHandle());

	glDeleteShader(m_cubemapShader.GetHandle());
	glDeleteFramebuffers(1, &m_gBuffer.fboObject);
	glDeleteTextures(1, &m_cubeMap);
};
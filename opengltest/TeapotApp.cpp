//MainApp.cpp
#include <glm\gtc\matrix_transform.hpp>

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

bool TeapotApp::Init(){

	if (glfwInit() != GL_TRUE){
		std::cout << "glfw failed!" << std::endl;
		return false;
	}

	glfwOpenWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_VERSION_MINOR, 3);


	if (!glfwOpenWindow(APP_WIDTH, APP_HEIGHT, 8, 8, 8, 8,32, 32, GLFW_WINDOW)){
		std::cout << "cant create window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(KeyCallback);

	if (glewInit() != GLEW_OK){
		std::cout << "glew failed!" << std::endl;
		return false;
	}

	ilInit();


	//Depth states - in deffered render this is disabled
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (!m_teapotShader.CreateForwardShader()){
		cout << "failed to load shader!" << endl;
		return 1;
	}

	if (!m_deferredShader.CreateDeferredShader()){
		cout << "failed to load shader!" << endl;
		return 1;
	}
	
	InitStaticMesh(m_teapotMesh, "teapot.obj","teapot\\",64);
	InitStaticMesh(m_sphereMesh, "sphere.obj", "", 1);

	m_camera = CreateCamera(vec3(0.0f,0.0f,-40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	m_fontShader = CreateFontShader();
	InitText2D(m_counterFont,"exportedFont.bmp", "ms per frame: 0.00", 50, 50, 24,m_fontShader);

	m_groundPlaneBuffer = CreateGroundPlaneData();

	//if (!CreateGBuffer()){
	//	return false;
	//}

	vec3 screenQuad[6];

	screenQuad[0] = vec3(1.0f,1.0f,0.0f);
	screenQuad[1] = vec3(-1.0f,1.0f,0.0f);
	screenQuad[2] = vec3(-1.0f,-1.0f,0.0f);
	screenQuad[3] = vec3(1.0f,1.0f,0.0f);
	screenQuad[4] = vec3(-1.0f, -1.0f, 0.0f);
	screenQuad[5] = vec3(1.0f, -1.0f, 0.0f);

	m_lights.color[0] = vec3(0.0f, 0.0f, 1.0f);
	m_lights.color[1] = vec3(1.0f, 0.0f, 0.0f);
	m_lights.color[2] = vec3(0.0f, 1.0f, 0.0f);
	m_lights.color[3] = vec3(0.5f, 0.0f, 0.5f);
	m_lights.color[4] = vec3(0.2f, 0.6f, 0.2f);

	lightPos[0] = vec3(45.0f, 20.0f,180.0f);
	lightPos[1] = vec3(-45.0f, -20.0f, 42.0f);
	lightPos[2] = vec3(240.0f, 20.0f, -42.0f);
	lightPos[3] = vec3(-120.0f, 40.0f, -100.0f);
	lightPos[4] = vec3(0.0f, 60.0f, 50.0f);

	for (int light = 0; light < 5; light++){
		m_lights.constantAtt[light] = 0.0f;
		m_lights.linearAtt[light] = 0.0f;
		m_lights.expAtt[light] = 0.7f;
	}


	glGenBuffers(1, &m_quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 6, screenQuad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
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
			movement[index] = vec3(potCol * 60.0f - 240.0f,0.0f,potRow * 42.0f - 168.0f);
		}
	}

	string firstStr = "ms per frame: ";
	string time     = to_string(deltaTime);

	glUseProgram(m_teapotShader.GetHandle());
		ForwardShaderUniforms teapotUniforms = m_teapotShader.GetUniforms();
		glUniformMatrix4fv(teapotUniforms.perspectiveMatrixUniform, 1, GL_FALSE, &m_camera.projectionMatrix[0][0]);
		glUniformMatrix3fv(teapotUniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
		glUniform3fv(teapotUniforms.lightColUniform, 5, &m_lights.color[0][0]);
		mat4x4 translationMatrix = rotation;
		glUniformMatrix4fv(teapotUniforms.rotationUniform, 1, GL_FALSE, &translationMatrix[0][0]);
	glUseProgram(0);

	glUseProgram(m_deferredShader.GetGBufferHandle());
	GBufferShaderUniforms gBufferUniforms = m_deferredShader.GetGBufferUniforms();
	glUniformMatrix3fv(gBufferUniforms.normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(gBufferUniforms.rotationMatrixUniform, 1, GL_FALSE, &translationMatrix[0][0]);
	glUseProgram(0);

	glUseProgram(m_deferredShader.GetLightPassHandle());
	vec2 screenSize((float)APP_WIDTH,(float)APP_HEIGHT);
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

		rotationAmount += 0.02f;

		for (int light = 0; light < 5; light++){
			m_lights.position[light].x = lightPos[light].x * cosAmount;
			m_lights.position[light].z = lightPos[light].z * sinAmount;
		}

		
		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	    RenderForward(&m_lights.position[0], movement);
		//RenderDeferred(movement);
				
		time = to_string(deltaTime * 1000.0f);
		ChangeText2D(m_counterFont, (firstStr + time.substr(0,4)).c_str(), 50, 50, 24);
		PrintText2D(m_counterFont);

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

	mat4x4 scaleMatrix1, identity;
	scaleMatrix1 = scale(scaleMatrix1, vec3(0.2f, 0.2f, 0.2f));

	glUseProgram(m_teapotShader.GetHandle());
	ForwardShaderUniforms teapotUniforms = m_teapotShader.GetUniforms();
		glUniformMatrix4fv(teapotUniforms.scaleUniform, 1, GL_FALSE, &scaleMatrix1[0][0]);
		glUniformMatrix4fv(teapotUniforms.cameraMatrixUniform, 1, GL_FALSE, &m_camera.viewMatrix[0][0]);
		glUniform3fv(teapotUniforms.eyePosUniform, 1, &m_camera.pos[0]);
		glUniform3fv(teapotUniforms.lightVecUniform, 5, &lightPositions[0][0]);

		glUniform1i(teapotUniforms.instancedUniform, 1);
		RenderInstancedStaticMesh(m_teapotMesh, teapotUniforms.matUni, &teapotPositions[0]);
		glUniformMatrix4fv(teapotUniforms.scaleUniform, 1, GL_FALSE, &identity[0][0]);
		glUniform1i(teapotUniforms.instancedUniform, 0);
		glUniform3fv(teapotUniforms.matUni.diffuseUniform, 1, &diff[0]);
		glBindVertexArray(m_groundPlaneBuffer);
		glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
	glUseProgram(0);
}

void TeapotApp::RenderDeferred(const vec3* teapotPositions){
	vec3 diff = vec3(0.4f, 0.4f, 0.4f);

	mat4x4 scaleMatrix1,identity;
	scaleMatrix1 = scale(scaleMatrix1, vec3(0.2f, 0.2f, 0.2f));

	//geometry pass

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gBuffer.fboObject);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearDepth(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_BLEND);
			glUseProgram(m_deferredShader.GetGBufferHandle());
			GBufferShaderUniforms gBufferUniforms = m_deferredShader.GetGBufferUniforms();
			glUniformMatrix4fv(gBufferUniforms.scaleMatrixUniform, 1, GL_FALSE, &scaleMatrix1[0][0]);
			mat4 viewProjection = m_camera.projectionMatrix * m_camera.viewMatrix;
			glUniformMatrix4fv(gBufferUniforms.cameraMatrixUniform, 1, GL_FALSE, &viewProjection[0][0]);
			glUniform1i(gBufferUniforms.instancedUniform, 1);
			
			RenderInstancedStaticMesh(m_teapotMesh, gBufferUniforms.materialUniforms, &teapotPositions[0]);
			
			glUniformMatrix4fv(gBufferUniforms.scaleMatrixUniform, 1, GL_FALSE, &identity[0][0]);
			glUniform1i(gBufferUniforms.instancedUniform, 0);
			glUniform3fv(gBufferUniforms.materialUniforms.diffuseUniform, 1, &diff[0]);
			glBindVertexArray(m_groundPlaneBuffer);
			
			glDrawElements(GL_TRIANGLES, 9 * 9 * 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
		glUseProgram(0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//lighting pass
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE,GL_ONE);

	mat4 worldMatrix;

	glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(m_deferredShader.GetLightPassHandle());
		LightPassShaderUniforms lightPassUniforms = m_deferredShader.GetLightPassUniforms();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,m_gBuffer.textures[0]);
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

			glCullFace(GL_FRONT);

			for (int light = 0; light < 5; light++){
				float scale = CalcSphereDistance(m_lights,light);
				worldMatrix = translate(identity, m_lights.position[light]) * glm::scale(identity,vec3(scale,scale,scale));
				glUniformMatrix4fv(lightPassUniforms.wvpMatrixUniform, 1, GL_FALSE, &(viewProjection * worldMatrix)[0][0]);
				glUniform3fv(lightPassUniforms.lightPosUniform, 1, &m_lights.position[light][0]);
				glUniform3fv(lightPassUniforms.lightColUniform, 1, &m_lights.color[light][0]);
				//RenderStaticMesh(m_sphereMesh, teapotUniforms.matUni);
			}
			glUseProgram(0);

			glCullFace(GL_BACK);

			glUseProgram(m_deferredShader.GetQuadPassHandle());
			glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
			glUniform1i(m_deferredShader.GetQuadPassUniforms().ambTextureUniform, 2);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glUseProgram(0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);
		
}

bool TeapotApp::CreateGBuffer(){
	return CreateGBufferData(m_gBuffer);
}

float TeapotApp::CalcSphereDistance(const PointLightData& pLight,int index){
	float maxChan = max(max(pLight.color[index].r, pLight.color[index].g), pLight.color[index].b);

	float ret = (-pLight.linearAtt[index] + sqrtf(pLight.linearAtt[index] * pLight.linearAtt[index]
				- 4.0f *pLight.expAtt[index] * (pLight.expAtt[index] - 256.0f * maxChan))) / (2.0f*pLight.expAtt[index]);
	
	return ret;
}

void TeapotApp::ShutDown(){
	CleanupText2D(m_counterFont);
	DestroyMesh(m_teapotMesh);
	DestroyMesh(m_sphereMesh);
	glDeleteVertexArrays(1, &m_groundPlaneBuffer);
	glDeleteShader(m_teapotShader.GetHandle());
	glDeleteShader(m_deferredShader.GetGBufferHandle());
	glDeleteShader(m_deferredShader.GetQuadPassHandle());
	glDeleteShader(m_deferredShader.GetLightPassHandle());
	glDeleteFramebuffers(1, &m_gBuffer.fboObject);

};
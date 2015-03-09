//SponzaApp.cpp
#include "SponzaApp.h"


#include <gtc\matrix_transform.hpp>

#include <iostream>
#include <math.h>
#include <time.h>
#include <IL\il.h>
#include "CubeMap.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "DepthBuffer.h"
#include "VertexTypes.h"

bool SponzaApp::Init(){

	StandardInit();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_lightingShader = CreateLightingShader();

	if (!InitStaticMesh(m_sponzaMesh, "sponza.obj", "meshes\\sponza_obj\\", aiProcess_Triangulate | aiProcess_CalcTangentSpace)){
		cout << "cant open sponza mesh!" << endl;
		return false;
	}

	if (!cubemapShader.CreateCubemapShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!m_shadowShader.CreateShadowShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!InitStaticMesh(cubeMesh, "cube.obj", "meshes\\")){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	std::vector<string> cubemapFilenames;
	cubemapFilenames.push_back("CubeMaps\\Square\\posx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Square\\negx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Square\\posy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Square\\negy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Square\\posz.jpg");
	cubemapFilenames.push_back("CubeMaps\\Square\\negz.jpg");
	cubeMap = CreateCubeMap(cubemapFilenames);

	m_lightPos = vec3(0.0f, 200.0f, 0.0f);

	m_camera                  = CreateCamera(vec3(5.0f, 2.0f, 0.0f), vec3(0.0f, 2.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f),1.0f,1000.0f,APP_WIDTH,APP_HEIGHT,45.0f);

	m_shadowCamera                  = CreateCamera(vec3(50.0f, 100.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),0.0f,0.0f,0.0f,0.0f,0.0f);
	m_shadowCamera.projectionMatrix = glm::ortho(-65.0f, 65.0f, -65.0f, 65.0f, -350.0f, 350.0f);

	if (!CreateDepthTextureBasic(m_fbo, m_depthTexture)){
		return false;
	}

	return true;
}

void SponzaApp::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;

	GLuint shaderProg = m_lightingShader;

	MaterialUniforms matUni;

	GLint cameraMatrixUniform      = glGetUniformLocation(shaderProg, "cameraMatrix");
	GLint perspectiveMatrixUniform = glGetUniformLocation(shaderProg, "perspectiveMatrix");
	GLint textureUniform           = glGetUniformLocation(shaderProg, "tex");
	GLint normalUniform			   = glGetUniformLocation(shaderProg, "bumpMap");
	GLint specMapUniform		   = glGetUniformLocation(shaderProg, "specMap");
	GLint alphaMapUniform		   = glGetUniformLocation(shaderProg, "alphaMap");
	GLint eyePosUniform            = glGetUniformLocation(shaderProg, "eyePos");
	matUni.diffuseUniform          = glGetUniformLocation(shaderProg, "materialDiffuse");
	matUni.ambientUniform          = glGetUniformLocation(shaderProg, "materialAmbient");
	matUni.specularUniform         = glGetUniformLocation(shaderProg, "materialSpecular");
	matUni.shininessUniform        = glGetUniformLocation(shaderProg, "materialShininess");
	GLint scaleUniform             = glGetUniformLocation(shaderProg, "scaleMatrix");
	GLint lightDir                 = glGetUniformLocation(shaderProg, "lightDir");
	GLint lightCol				   = glGetUniformLocation(shaderProg, "lightCol");
	GLint usesNormalMap            = glGetUniformLocation(shaderProg, "useNormalMap");
	GLint usesAlphaMap			   = glGetUniformLocation(shaderProg, "useAlphaMap");
	GLint shadowMapUniform		   = glGetUniformLocation(shaderProg, "shadowTexture");
	GLint depthMatrix			   = glGetUniformLocation(shaderProg, "depthPerspectiveMatrix");

	vec3 lightVec(1.0f, -0.5f, 0.0f);
	vec3 lightColVec(1.0f, 1.0f, 1.0f);

	mat4 rotation     = rotate(mat4(), glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	mat3 normalMatrix = mat3x3(transpose(rotation));
	mat4 scaleMatrix;
	scaleMatrix = scale(scaleMatrix, vec3(0.03f, 0.03f, 0.03f));
	mat4 identity;

	Material myMaterial;

	string firstStr = "ms per frame: ";
	string time = to_string(deltaTime);

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	mat4 depthPersp = biasMatrix * m_shadowCamera.projectionMatrix * m_shadowCamera.viewMatrix;

	glUseProgram(shaderProg);
		glUniformMatrix4fv(depthMatrix, 1, GL_FALSE, &depthPersp[0][0]);
		glUniformMatrix4fv(perspectiveMatrixUniform, 1, GL_FALSE, &m_camera.projectionMatrix[0][0]);
		glUniform3fv(lightDir, 1, &lightVec[0]);
		glUniform3fv(lightCol, 1, &lightColVec[0]);
		glUniformMatrix4fv(scaleUniform, 1, GL_FALSE, &scaleMatrix[0][0]);
		glUniform1i(textureUniform, 0);
		glUniform1i(normalUniform, 1);
		glUniform1i(specMapUniform, 2);
		glUniform1i(alphaMapUniform, 3);
		glUniform1i(shadowMapUniform, 4);
	glUseProgram(0);



	currentFrame = glfwGetTime();

	m_shadowShader.SetDepthUniforms(m_shadowCamera.projectionMatrix);

	do{
		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderShadow();
		
		glUseProgram(shaderProg);


		glUniformMatrix4fv(cameraMatrixUniform, 1, GL_FALSE, &m_camera.viewMatrix[0][0]);
		glUniform3fv(eyePosUniform, 1, &m_camera.pos[0]);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);

		for (int mesh = 0; mesh < m_sponzaMesh.m_numMeshes; ++mesh){
			int materialIndex = m_sponzaMesh.m_meshData[mesh].m_materialIndex;
			MaterialInfo mat = m_sponzaMesh.m_materialData[materialIndex];

			glUniform3fv(matUni.diffuseUniform,  1, &mat.m_material.diffuse[0]);
			glUniform3fv(matUni.specularUniform, 1, &mat.m_material.specular[0]);
			glUniform3fv(matUni.ambientUniform,  1, &mat.m_material.ambient[0]);
			glUniform1f(matUni.shininessUniform, mat.m_material.shininess);

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
			}
			else{
				glUniform1i(usesAlphaMap, 0);
			}

			RenderStaticMeshComponent(m_sponzaMesh.m_meshData[mesh]);

		}

		glUseProgram(0);


		glUseProgram(cubemapShader.GetHandle());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
			glUniformMatrix4fv(cubemapShader.GetWVPMatrix(), 1, GL_FALSE, &(m_camera.projectionMatrix * m_camera.viewMatrix * translate(identity, m_camera.pos))[0][0]);
			glUniform1i(cubemapShader.GetSampler(), 0);
			glCullFace(GL_FRONT);
			RenderStaticMeshComponent(cubeMesh.m_meshData[0]);
			glCullFace(GL_BACK);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glUseProgram(0);

		time = to_string(deltaTime * 1000.0f);

		glfwSwapBuffers();

		ReadMouse();
		ReadKeys();
		MoveCameraForward(m_camera, deltaTime * m_moveForwardAmount);
		MoveCameraVertically(m_camera, deltaTime *  m_moveUpAmount);
		MoveCameraHorizontally(m_camera, deltaTime *  m_moveSidewaysAmount);

		lastFrame = currentFrame;
		currentFrame = glfwGetTime();
	} while (m_run);

};

void SponzaApp::ShutDown(){
	glDeleteShader(m_lightingShader);

	m_run = false;
};

GLuint SponzaApp::CreateLightingShader(){

	const int numAttribs = 5;
	const char* attribs[numAttribs] = { "inCoords", "inNormals","inTangents","inBitangents","inUVs" };

	return CreateShader("lightingSponza.vp", "lightingSponza.fp", attribs, numAttribs);
}

void SponzaApp::RenderShadow(){
	mat4 scaleMatrix;
	scaleMatrix = scale(scaleMatrix, vec3(0.03f, 0.03f, 0.03f));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	glCullFace(GL_FRONT);
	glUseProgram(m_shadowShader.GetDepthHandle());
		DepthShaderUniforms shadowUniforms = m_shadowShader.GetDepthUniforms();

		m_shadowShader.UpdateDepthUniforms(scaleMatrix, m_shadowCamera.viewMatrix);

		
		for (int mesh = 0; mesh < m_sponzaMesh.m_numMeshes; ++mesh){
			
			RenderStaticMeshComponent(m_sponzaMesh.m_meshData[mesh]);
		}

	glUseProgram(0);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

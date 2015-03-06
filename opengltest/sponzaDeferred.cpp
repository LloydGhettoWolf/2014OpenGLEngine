//sponzaDeferred.cpp
#include "SponzaDeferred.h"
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

//sponza mesh
StaticMesh			m_sponzaMesh;

static CubemapShader		  cubemapShader;
static StaticMesh            cubeMesh;
static GLuint				  cubeMap;

GLint cameraMatrixUniform; 
GLint perspectiveMatrixUniform; 
GLint textureUniform; 
GLint normalUniform; 
GLint specMapUniform;
GLint alphaMapUniform; 
GLint eyePosUniform; 
//matUni.diffuseUniform = glGetUniformLocation(shaderProg, "materialDiffuse");
//matUni.ambientUniform = glGetUniformLocation(shaderProg, "materialAmbient");
//matUni.specularUniform = glGetUniformLocation(shaderProg, "materialSpecular");
//matUni.shininessUniform = glGetUniformLocation(shaderProg, "materialShininess");
GLint scaleUniform;
GLint lightDir; 
GLint lightCol; 
GLint usesNormalMap; 
GLint usesAlphaMap; 
GLint shadowMapUniform; 
GLint depthMatrix; 

bool SponzaDeferred::Init(){

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

	if(!InitStaticMesh(m_sponzaMesh, "sponza.obj", "meshes\\sponza_obj\\", aiProcess_Triangulate | aiProcess_CalcTangentSpace)){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	if (!InitStaticMesh(cubeMesh, "cube.obj", "meshes\\")){
		cout << "couldn't load teapot mesh!" << endl;
		return false;
	}

	m_camera = CreateCamera(vec3(0.0f, 0.0f, -40.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	m_camera.projectionMatrix = glm::perspective(45.0f, APP_WIDTH / APP_HEIGHT, 3.0f, 500.0f);

	glfwSetWindowSizeCallback(MyResize);


	m_lights.position = new vec3[NUM_POINT_LIGHTS];
	m_lights.color = new vec3[NUM_POINT_LIGHTS];
	m_lights.constantAtt = new float[NUM_POINT_LIGHTS];
	m_lights.expAtt = new float[NUM_POINT_LIGHTS];
	m_lights.linearAtt = new float[NUM_POINT_LIGHTS];

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

	return true;
}

void SponzaDeferred::SetupUniforms(){

	//GLint shaderProg = m_deferredRenderer.
	GLint shaderProg = 0;
	cameraMatrixUniform = glGetUniformLocation(shaderProg, "cameraMatrix");
	perspectiveMatrixUniform = glGetUniformLocation(shaderProg, "perspectiveMatrix");
	textureUniform = glGetUniformLocation(shaderProg, "tex");
	normalUniform = glGetUniformLocation(shaderProg, "bumpMap");
	specMapUniform = glGetUniformLocation(shaderProg, "specMap");
	alphaMapUniform = glGetUniformLocation(shaderProg, "alphaMap");
	eyePosUniform = glGetUniformLocation(shaderProg, "eyePos");
	//matUni.diffuseUniform = glGetUniformLocation(shaderProg, "materialDiffuse");
	//matUni.ambientUniform = glGetUniformLocation(shaderProg, "materialAmbient");
	//matUni.specularUniform = glGetUniformLocation(shaderProg, "materialSpecular");
	//matUni.shininessUniform = glGetUniformLocation(shaderProg, "materialShininess");
	scaleUniform = glGetUniformLocation(shaderProg, "scaleMatrix");
	lightDir = glGetUniformLocation(shaderProg, "lightDir");
	lightCol = glGetUniformLocation(shaderProg, "lightCol");
	usesNormalMap = glGetUniformLocation(shaderProg, "useNormalMap");
	usesAlphaMap = glGetUniformLocation(shaderProg, "useAlphaMap");
	shadowMapUniform = glGetUniformLocation(shaderProg, "shadowTexture");
	depthMatrix = glGetUniformLocation(shaderProg, "depthPerspectiveMatrix");
}


void SponzaDeferred::Run(){

	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;


	mat4x4 rotation = rotate(mat4x4(), 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat3x3 normalMatrix = mat3x3(transpose(rotation));


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

		RenderDeferred(NUM_POINT_LIGHTS);

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


void SponzaDeferred::RenderDeferred(int numLights){

	//m_deferredRenderer.RenderDeferred(m_camera.projectionMatrix * m_camera.viewMatrix, m_lights.position, m_lights.color,
		//m_camera.pos, numLights, RenderGeometry, RenderCubemap);

}


void SponzaDeferred::ShutDown(){


};

void SponzaDeferred::RenderGeometry(GLint shaderHandle, mat4& viewProjection){
	
	glUseProgram(shaderHandle);


	//glUniformMatrix4fv(cameraMatrixUniform, 1, GL_FALSE, &m_camera.viewMatrix[0][0]);
	//glUniform3fv(eyePosUniform, 1, &m_camera.pos[0]);

	glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, m_depthTexture);

	for (int mesh = 0; mesh < m_sponzaMesh.m_numMeshes; ++mesh){
		int materialIndex = m_sponzaMesh.m_meshData[mesh].m_materialIndex;
		MaterialInfo mat = m_sponzaMesh.m_materialData[materialIndex];

		//glUniform3fv(matUni.diffuseUniform, 1, &mat.m_material.diffuse[0]);
		//glUniform3fv(matUni.specularUniform, 1, &mat.m_material.specular[0]);
		//glUniform3fv(matUni.ambientUniform, 1, &mat.m_material.ambient[0]);
		//glUniform1f(matUni.shininessUniform, mat.m_material.shininess);

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
		}
		else{
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


}

void SponzaDeferred::RenderCubemap(mat4& viewProjection, vec3& camPos){

	mat4 identity;

	glUseProgram(cubemapShader.GetHandle());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		glUniformMatrix4fv(cubemapShader.GetWVPMatrix(), 1, GL_FALSE, &(viewProjection * translate(identity, camPos))[0][0]);
		glUniform1i(cubemapShader.GetSampler(), 0);
		glCullFace(GL_FRONT);
		RenderStaticMeshComponent(cubeMesh.m_meshData[0]);
		glCullFace(GL_BACK);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
}
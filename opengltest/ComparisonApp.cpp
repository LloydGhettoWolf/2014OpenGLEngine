#include <iostream>
#include <gtc\matrix_transform.hpp>
#include <gtc\matrix_inverse.hpp>
#include "CubeMap.h"
#include "ComparisonApp.h"

using namespace std;

bool ComparisonApp::InitGUI(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(APP_WIDTH, APP_HEIGHT);

	BlinnGUI = TwNewBar("MaterialProperties");
	TwAddVarRW(BlinnGUI, "shininess", TW_TYPE_FLOAT, &m_material1.shininess, "min=0.0f max=2048.0f step=2.0f");
	TwDefine(" MaterialProperties size='200 300' position='20 80'");
	TwAddVarRW(BlinnGUI, "Rotation", TW_TYPE_QUAT4F, &m_model1Rotation, "");

	//define a 3 component vector for spec, ambient, diff values in the ui
	TwStructMember members[] = {
		{ "r", TW_TYPE_FLOAT, offsetof(vec3, x), "min=0.0f max=1.0f step=0.01f" },
		{ "g", TW_TYPE_FLOAT, offsetof(vec3, y), "min=0.0f max=1.0f step=0.01f" },
		{ "b", TW_TYPE_FLOAT, offsetof(vec3, z), "min=0.0f max=1.0f step=0.01f" },
	};

	TwType colType = TwDefineStruct("ColType", members, 3, sizeof(vec3), NULL, NULL);
	TwAddVarRW(BlinnGUI, "Diffuse", colType, &m_material1.diffuse[0], NULL);
	TwAddVarRW(BlinnGUI, "Specular", colType, &m_material1.specular[0], NULL);
	TwAddVarRW(BlinnGUI, "Ambient", colType, &m_material1.ambient[0], NULL);
	TwAddVarRW(BlinnGUI, "Reflection", TW_TYPE_FLOAT, &m_reflect1, "min=0.0f max=1.0f step=0.05f");
	TwAddVarRW(BlinnGUI, "Refraction", TW_TYPE_FLOAT, &refractIndex1, "min=0.0f max=5.0f step=0.02f");
	TwAddVarRW(BlinnGUI, "SolidMix", TW_TYPE_FLOAT, &solidMix1, "min=0.0f max=1.0f step=0.05f");

	BDRFGUI = TwNewBar("MaterialProperties2");
	TwDefine(" MaterialProperties2 size='200 300' position='800 80'");
	TwAddVarRW(BDRFGUI, "Rotation", TW_TYPE_QUAT4F, &m_model2Rotation, "");
	TwAddVarRW(BDRFGUI, "Fresnel",  TW_TYPE_FLOAT,	&refractiveIndex, "min=1.0f max=3.0f step=0.05f");
	TwAddVarRW(BDRFGUI, "Roughness", TW_TYPE_FLOAT, &roughness,       "min=0.0f max=1.0f step=0.02f");
	TwAddVarRW(BDRFGUI, "K",       TW_TYPE_FLOAT,   &k,		          "min=0.0f max=1.0f step=0.05f");
	

	LightDir = TwNewBar("LightDir");
	TwAddVarRW(LightDir, "light direction", TW_TYPE_DIR3F,&m_lightDir[0],"");
	TwDefine("LightDir size='200 200' position='20 400'");
	return true;

}

bool ComparisonApp::Init(){

	StandardInit();

	if (!m_modelShader.CreateComparisonShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	if (!m_ctShader.CreateCookTorranceShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	InitStaticMesh(m_exampleMesh, "buddha.obj", "meshes\\");
	InitStaticMesh(m_cubeMesh, "cube.obj", "meshes\\");

	m_centerOffset = (m_exampleMesh.m_boundingBoxMax.y - m_exampleMesh.m_boundingBoxMin.y)/2.0f;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_material1.ambient = vec3(0.1f, 0.1f, 0.1f);
	m_material1.diffuse = vec3(0.5f, 0.4f, 0.5f);
	m_material1.specular = vec3(0.6f, 0.6f, 0.6f);
	m_material1.shininess = 8.0f;


	m_lightDir.x = 1.0f;
	m_lightDir.y = 0.0f;
	m_lightDir.z = 0.0f;

	m_reflect1  = 0.0f;
	refractIndex1  = 0.2f;

	roughness = 1.0f;
	k = 1.0f;
	fresnel = 1.0f;
	float newRefractive = (1.0f - (n*n) / (1.0f + n));
	refractiveIndex = newRefractive * newRefractive;

	if (!InitGUI()){
		return false;
	}

	if (!m_cubemapShader.CreateCubemapShader()){
		cout << "failed to load shader!" << endl;
		return false;
	}

	//cubemap
	std::vector<string> cubemapFilenames;
	cubemapFilenames.push_back("CubeMaps\\Beach\\posx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Beach\\negx.jpg");
	cubemapFilenames.push_back("CubeMaps\\Beach\\posy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Beach\\negy.jpg");
	cubemapFilenames.push_back("CubeMaps\\Beach\\posz.jpg");
	cubemapFilenames.push_back("CubeMaps\\Beach\\negz.jpg");
	m_cubeMap = CreateCubeMap(cubemapFilenames);


	return true;
}

void ComparisonApp::Run(){
	float lastFrame = 0.0f;
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;


	string firstStr = "ms per frame: ";
	string time = to_string(deltaTime);


	lastFrame = currentFrame = glfwGetTime();

	m_lights.position[0] = vec3(0.0f, 0.0f, -10.0f);
	m_lights.color[0]    = vec3(1.0f, 1.0f, 1.0f);

	m_modelShader.SetUniforms(m_camera.projectionMatrix);
	m_ctShader.SetUniforms(m_camera.projectionMatrix);

	do{
		deltaTime = currentFrame - lastFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Render();

		time = to_string(deltaTime * 1000.0f);
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

}

void ComparisonApp::Render(){

	mat4x4 identity;
	mat4x4 fromQuat = mat4_cast(m_model1Rotation);
	mat4x4 worldMatrix = translate(identity, vec3(0.0f, m_centerOffset, 0.0f)) * fromQuat * rotate(identity,180.0f,vec3(0.0f,1.0f,0.0f)) * translate(identity, vec3(0.0f, -m_centerOffset, 0.0f));
	mat3x3 normalMatrix = inverseTranspose(mat3(worldMatrix));
	glDisable(GL_CULL_FACE);
	glUseProgram(m_modelShader.GetHandle());
		
		ComparisonShaderUniforms teapotUniforms = m_modelShader.GetUniforms();
		m_modelShader.UpdateUniforms(worldMatrix, normalMatrix, m_camera.viewMatrix, m_camera.pos, m_lightDir);
		glUniform3fv(teapotUniforms.matUni.diffuseUniform, 1, &m_material1.diffuse[0]);
		glUniform3fv(teapotUniforms.matUni.ambientUniform, 1, &m_material1.ambient[0]);
		glUniform3fv(teapotUniforms.matUni.specularUniform, 1, &m_material1.specular[0]);

		glUniform1f(teapotUniforms.matUni.shininessUniform, m_material1.shininess);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
		glUniform1i(teapotUniforms.cubemapUniform, 0);
		glUniform1f(teapotUniforms.reflectUniform, m_reflect1);
		glUniform1f(teapotUniforms.refractUniform, refractIndex1);
		glUniform1f(teapotUniforms.solidMixUniform, solidMix1);


		RenderStaticMeshComponent(m_exampleMesh.m_meshData[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);

		
	glUseProgram(m_ctShader.GetHandle());

		fromQuat = mat4_cast(m_model2Rotation);
		worldMatrix = translate(identity, vec3(-5.0f, m_centerOffset, 0.0f)) * fromQuat * rotate(identity, 180.0f, vec3(0.0f, 1.0f, 0.0f)) * translate(identity, vec3(0.0f, -m_centerOffset, 0.0f));
		normalMatrix = inverseTranspose(mat3(worldMatrix));

		CookTorranceShaderUniforms ctUniforms = m_ctShader.GetUniforms();

		m_ctShader.UpdateUniforms(worldMatrix, normalMatrix, m_camera.viewMatrix, m_camera.pos, m_lightDir);
		glUniform1f(ctUniforms.fresnelUniform,   refractiveIndex);
		glUniform1f(ctUniforms.kUniform,		 k);
		glUniform1f(ctUniforms.roughnessUniform, roughness);
		RenderStaticMeshComponent(m_exampleMesh.m_meshData[0]);
		
	glUseProgram(0);
	

	glEnable(GL_CULL_FACE);
	glUseProgram(m_cubemapShader.GetHandle());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
		glUniformMatrix4fv(m_cubemapShader.GetWVPMatrix(), 1, GL_FALSE, &(m_camera.projectionMatrix * m_camera.viewMatrix * translate(identity, m_camera.pos))[0][0]);
		glUniform1i(m_cubemapShader.GetSampler(), 0);
		glCullFace(GL_FRONT);
		RenderStaticMeshComponent(m_cubeMesh.m_meshData[0]);
		glCullFace(GL_BACK);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
}

void ComparisonApp::ShutDown(){
	glDeleteShader(m_modelShader.GetHandle());
	glDeleteShader(m_cubemapShader.GetHandle());
	glDeleteShader(m_ctShader.GetHandle());
}
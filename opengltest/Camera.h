//Camera.h
#pragma once
#include <glm.hpp>

using namespace glm;

struct Camera{
	vec3 lookVec;
	vec3 heading;
	vec3 upVec;
	vec3 rightVec;
	vec3 pos;

	float yRotation = -3.14129f/4.0f;

	float nearZ, farZ, width, height;

	mat4x4 viewMatrix,projectionMatrix;

	Camera(vec3& look, vec3& up, vec3& right) : lookVec(look), upVec(up), rightVec(right){};
	Camera(){};
};

Camera CreateCamera(vec3& pos, vec3& focusPoint, vec3& upVec,float nearZ,float farZ,float width,float height,float fov);
void   MoveCameraForward(Camera& cam, float amount);
void   MoveCameraHorizontally(Camera& cam,float amount);
void   MoveCameraVertically(Camera& cam, float amount);
void   MoveCameraCustom(Camera& cam, vec3& pos);
void   MoveCameraTrackPoint(Camera& cam, vec3& pos, vec3& focus);
void   ComboRotate(Camera& cam,float amountX, float amountY);
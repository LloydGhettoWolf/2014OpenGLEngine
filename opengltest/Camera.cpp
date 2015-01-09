//Camera.cpp
#include "Camera.h"
#include <gtc\matrix_transform.hpp>

Camera CreateCamera(vec3& pos, vec3& focusPoint, vec3& upVec){
	vec3 lookVec = focusPoint - pos;
	lookVec = normalize(lookVec);

	vec3 rightVec = cross(lookVec,upVec);
	rightVec = normalize(rightVec);

	upVec = cross(rightVec, lookVec);
	upVec = normalize(upVec);

	Camera newCam(lookVec,upVec,rightVec);

	newCam.pos = pos;
	newCam.viewMatrix = glm::lookAt(pos, focusPoint, upVec);
	newCam.heading = lookVec;
	return newCam;
}

void   MoveCameraHorizontally(Camera& cam, float amount){
	vec3 translationVec = amount * cam.rightVec;
	cam.pos += translationVec;
	cam.viewMatrix = translate(cam.viewMatrix, -translationVec);
}


void   MoveCameraForward(Camera& cam, float amount){
	vec3 translationVec = amount * cam.heading;
	cam.pos += translationVec;
	cam.viewMatrix = translate(cam.viewMatrix, -translationVec);
}

void   MoveCameraVertically(Camera& cam, float amount){
	vec3 translationVec = amount * cam.upVec;
	cam.pos += translationVec;
	cam.viewMatrix = translate(cam.viewMatrix, -translationVec);
}

void   MoveCameraCustom(Camera& cam, vec3& pos){
	cam.pos = pos;;
	cam.viewMatrix = glm::lookAt(pos, pos+cam.lookVec, cam.upVec);
}

void   MoveCameraTrackPoint(Camera& cam, vec3& pos,vec3& focus){
	cam.viewMatrix = glm::lookAt(pos,focus, cam.upVec);
}

void ComboRotate(Camera& cam,float amountX, float amountY){
	cam.xRotation += glm::radians(amountX);
	cam.yRotation += glm::radians(-amountY);

	if (cam.xRotation < 0.0f)
		cam.xRotation += (2.0f *  3.14129f);

	if (cam.xRotation > (2.0f *  3.14129f))
		cam.xRotation -= (2.0f *  3.14129f);

	if (cam.yRotation > 0.0f)
		cam.yRotation = 0.0f;

	if (cam.yRotation < -3.14129f / 2.0f)
		cam.yRotation =  -3.14129f / 2.0f;

	cam.lookVec.x = cam.heading.x =  cosf(cam.xRotation) - sinf(cam.xRotation);
	cam.lookVec.z = cam.heading.z =  sinf(cam.xRotation) + cosf(cam.xRotation);

	cam.lookVec = normalize(cam.lookVec);

	cam.lookVec.y = cosf(cam.yRotation) + sinf(cam.yRotation);
	cam.lookVec.z = cam.lookVec.z  * -sinf(cam.yRotation) + cam.lookVec.z * cosf(cam.yRotation);


	vec3 lookPoint  = cam.pos + glm::normalize(cam.lookVec);
	cam.rightVec = glm::cross(cam.lookVec, cam.upVec);
	cam.rightVec = glm::normalize(cam.rightVec);

	cam.viewMatrix = glm::lookAt(cam.pos, lookPoint, cam.upVec);
}
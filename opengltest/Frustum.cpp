//Frustum.cpp - 
//last modified 01/13/2013

#pragma once
#include "Frustum.h"

void	NormalizePlane(Frustum& frustum, int index);

void Extract(Frustum& frustum,const vec3& eye, const mat4x4& camMatrix, const mat4x4& projMatrix)
{
	frustum.m_pos = eye;

	mat4x4 MVPMatrix = projMatrix *  camMatrix;

	MVPMatrix = glm::transpose(MVPMatrix);
	//left and right planes
	frustum.m_planes[0] = MVPMatrix[3] - MVPMatrix[0];
	NormalizePlane(frustum,0);

	frustum.m_planes[1] = MVPMatrix[3] + MVPMatrix[0];
	NormalizePlane(frustum, 1);

	//near and far planes
	frustum.m_planes[2] = MVPMatrix[3] - MVPMatrix[2];
	NormalizePlane(frustum, 2);

	frustum.m_planes[3] = MVPMatrix[3] + MVPMatrix[2];
	NormalizePlane(frustum, 3);

	//top and bottom planes
	frustum.m_planes[4] = MVPMatrix[3] - MVPMatrix[1];
	NormalizePlane(frustum, 4);

	frustum.m_planes[5] = MVPMatrix[3] + MVPMatrix[1];
	NormalizePlane(frustum, 5);
	
}


bool TestPoint(const Frustum& frustum,const vec3& testPoint){

	for(int planeNum = 0; planeNum < 6; planeNum++){
		vec4 plane = frustum.m_planes[planeNum];
		if (plane.x * testPoint.x + plane.y * testPoint.y + plane.z * testPoint.z
			+ plane.w < 0.0f){
			return false;
		}
	}

	return true;
}


bool TestSphere(const Frustum& frustum,const vec3& center, float radius)
{
	for (int planeNum = 0; planeNum < 6; planeNum++){
		vec4 plane = frustum.m_planes[planeNum];
		float t = plane[0] * center.x +
				  plane[1] * center.y +
				  plane[2] * center.z +
				  plane[3];

		if( t < -radius)
			return false;

		if((float)fabs(t) < radius)
			return INTERSECTING;
	}
	return true;
}


bool TestBox(const Frustum& frustum,const BoundingBox& bbox,mat4x4& proj) {

	for (int planeNum = 0; planeNum < 6; ++planeNum){
		vec4 plane = frustum.m_planes[planeNum];
		int iInCount = 8;
		int iPtIn = 1;

		for(int corner = 0; corner < 8; ++corner){
			vec4 thisCorner = proj * bbox.tCorners[corner];

			float side =	plane[0] * thisCorner.x +
							plane[1] * thisCorner.y +
							plane[2] * thisCorner.z +
							plane[3];
			if(side > 0) {
				return true;
			}
		}

	}

	return false;
}


bool CheckRect(const Frustum& frustum,const vec3& testPoint, float radius){
	  
	    //2d check, so only need 4 planes and 4 points
	for (int planeNum = 0; planeNum < 4; ++planeNum){
		vec4 plane = frustum.m_planes[planeNum];
			if(glm::dot(vec3(plane), vec3((testPoint.x - radius),0,(testPoint.z - radius)))
				 + plane.w > 0.0f)
			{
				continue;
			}
		
			if(glm::dot(vec3(plane), vec3((testPoint.x + radius),0,(testPoint.z - radius)))
				+ plane.w > 0.0f)
			{
				continue;
			}

			if(glm::dot(vec3(plane), vec3((testPoint.x - radius),0, (testPoint.z + radius)))
				+ plane.w > 0.0f)
			{
				continue;
			}

			if(glm::dot(vec3(plane), vec3((testPoint.x + radius),0, (testPoint.z + radius)))
				+ plane.w > 0.0f)
			{
				continue;
			}

			return false;}
	return true;
}

void  NormalizePlane(Frustum& frustum,int index){
	vec4 plane = frustum.m_planes[index];
	float normFactor = plane[0] * plane[0] + plane[1] * plane[1] +plane[2] * plane[2];
	normFactor = sqrtf(normFactor);
	plane /= normFactor;
	frustum.m_planes[index] = plane;
}
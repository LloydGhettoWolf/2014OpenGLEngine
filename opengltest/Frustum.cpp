//Frustum.cpp - 
//last modified 01/13/2013

#pragma once
#include "Frustum.h"

Frustum* Frustum::GetInstance()
{
	if (!m_instance){
		m_instance = new Frustum;
		return m_instance;
	}

	return m_instance;
}


void Frustum::Extract(const vec3& eye,const mat4x4& camMatrix,const mat4x4& projMatrix)
{
	m_pos = eye;

	m_modView = camMatrix;
	m_projMatrix = projMatrix;
	mat4x4 MVPMatrix =  m_projMatrix *  m_modView;

	MVPMatrix = glm::transpose(MVPMatrix);
	//left and right planes
	m_planes[0] = MVPMatrix[3] - MVPMatrix[0];
	NormalizePlane(0);

	m_planes[1] = MVPMatrix[3] + MVPMatrix[0];
	NormalizePlane(1);

	//near and far planes
	m_planes[2] = MVPMatrix[3] - MVPMatrix[2];
	NormalizePlane(2);

	m_planes[3] = MVPMatrix[3] + MVPMatrix[2];
	NormalizePlane(3);

	//top and bottom planes
	m_planes[4] = MVPMatrix[3] - MVPMatrix[1];
	NormalizePlane(4);

	m_planes[5] = MVPMatrix[3] + MVPMatrix[1];
	NormalizePlane(5);
	
}




bool Frustum::TestPoint(const vec3& testPoint)const{

	for(int plane = 0; plane < 6; plane++){
		if(m_planes[plane].x * testPoint.x + m_planes[plane].y * testPoint.y + m_planes[plane].z * testPoint.z
			+ m_planes[plane].w < 0.0f){
			return false;
		}
	}

	return true;
}


bool Frustum::TestSphere(const vec3& center, float radius) const
{
	for(int plane = 0; plane < 6; plane++)
	{
		float t =	m_planes[plane][0] * center.x +
					m_planes[plane][1] * center.y +
					m_planes[plane][2] * center.z +
					m_planes[plane][3];

		if( t < -radius)
			return false;

		if((float)fabs(t) < radius)
			return INTERSECTING;
	}
	return true;
}


bool Frustum::TestBox(const BoundingBox& bbox) const
{
	vec3 tCorners[8] = {	vec3(bbox.m_min.x, bbox.m_min.y, bbox.m_min.z),
							vec3(bbox.m_max.x, bbox.m_min.y, bbox.m_min.z),
							vec3(bbox.m_min.x, bbox.m_max.y, bbox.m_min.z),
							vec3(bbox.m_min.x, bbox.m_min.y, bbox.m_max.z),
							vec3(bbox.m_max.x, bbox.m_max.y, bbox.m_min.z),
							vec3(bbox.m_min.x, bbox.m_max.y, bbox.m_max.z),
							vec3(bbox.m_max.x, bbox.m_min.y, bbox.m_max.z),
							vec3(bbox.m_max.x, bbox.m_max.y, bbox.m_max.z)	};
	int iTotalIn = 0;

	for(int plane=0; plane<6; plane++)
	{
		int iInCount = 8;
		int iPtIn = 1;

		for(int corner = 0; corner < 8; corner++)
		{
			float side =	m_planes[plane][0] * tCorners[corner].x +
							m_planes[plane][1] * tCorners[corner].y +
							m_planes[plane][2] * tCorners[corner].z +
							m_planes[plane][3];
			if(side < 0) {
				iPtIn = 0;
				iInCount--;
			}
		}

		if(iInCount == 0)
			return false;

		iTotalIn += iPtIn;
	}

	return true;

}


bool Frustum::CheckRect(const vec3& testPoint, float radius)
{
	  

	    //2d check, so only need 4 planes and 4 points
		for(int plane=0; plane<4; plane++) 
		{
			if(glm::dot(vec3(m_planes[plane]), vec3((testPoint.x - radius),0,(testPoint.z - radius)))
				 + m_planes[plane].w > 0.0f)
			{
				continue;
			}
		
			if(glm::dot(vec3(m_planes[plane]), vec3((testPoint.x + radius),0,(testPoint.z - radius)))
				+ m_planes[plane].w > 0.0f)
			{
				continue;
			}

			if(glm::dot(vec3(m_planes[plane]), vec3((testPoint.x - radius),0, (testPoint.z + radius)))
				+ m_planes[plane].w > 0.0f)
			{
				continue;
			}

			if(glm::dot(vec3(m_planes[plane]), vec3((testPoint.x + radius),0, (testPoint.z + radius)))
				+ m_planes[plane].w > 0.0f)
			{
				continue;
			}

			return false;}
	return true;
}


Frustum* Frustum::m_instance = NULL;
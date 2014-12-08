//Frustum.h - a class that implements a 3d viewing frustum for culling
//LAST MODIFIED 11/26/ - added from previous project

#pragma once
#include <glm.hpp>
#include "BoundingBox.h"


const float EPSILON = 0.0f;

using glm::mat4x4;
using glm::vec3;
using glm::vec4;

const int INTERSECTING = 1;

struct Frustum{
	vec4			m_planes[6];
	vec3			m_pos;
};


void			Extract(Frustum& frustum,const vec3& eyePoint, const mat4x4& camMatrix, const mat4x4& projMatrix);
bool			TestPoint(const Frustum& frustum,const vec3& point);
bool 			TestSphere(const Frustum& frustum, const vec3& point, float radius);
bool			TestBox(const Frustum& frustum, const BoundingBox& box);
bool			CheckCube(const Frustum& frustum, const vec3& point, float radius);
bool			CheckRect(const Frustum& frustum, const vec3& point, float radius);



		
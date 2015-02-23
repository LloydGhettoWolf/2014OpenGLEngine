//BoundingBox.h
//LAST MODIFIED: 01/13/2013


#pragma once
#include <glm.hpp>

using glm::vec4;
using glm::vec3;

//bounding box class that has 8 corners stored as 3 component vectors. max is index 7, min is 0

class BoundingBox
{
	public:
		BoundingBox(vec4& min, vec4& max){
		
			tCorners[0] = vec4(min.x, min.y, min.z, 1.0f);
			tCorners[1] = vec4(max.x, min.y, min.z, 1.0f);
			tCorners[2] = vec4(min.x, max.y, min.z, 1.0f);
			tCorners[3] = vec4(min.x, min.y, max.z, 1.0f);
			tCorners[4] = vec4(max.x, max.y, min.z, 1.0f);
			tCorners[5] = vec4(min.x, max.y, max.z, 1.0f);
			tCorners[6] = vec4(max.x, min.y, max.z, 1.0f);
			tCorners[7] = vec4(max.x, max.y, max.z, 1.0f);
		}


		bool ContainsPoint(const vec3& point)const{
			vec4 min = tCorners[0];
			vec4 max = tCorners[7];
			return (point.x >= min.x && point.y >= min.y &&
				    point.z >= min.z && point.x <= max.x && 
					point.y <= max.y && point.z <= max.z);
		}


		inline vec4			GetCenter()const{ return (tCorners[0] + tCorners[7]) / 2.0f; }
		vec4 tCorners[8]; 
};
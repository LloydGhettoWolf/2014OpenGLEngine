//BoundingBox.h
//LAST MODIFIED: 01/13/2013


#pragma once
#include <glm.hpp>

using glm::vec3;

//bounding box class that has 8 corners stored as 3 component vectors. max is index 7, min is 0

class BoundingBox
{
	public:
		BoundingBox(){};

		BoundingBox(vec3& min,vec3& max){
		
			tCorners[0] = vec3(min.x, min.y, min.z);
			tCorners[1] = vec3(max.x, min.y, min.z);
			tCorners[2] = vec3(min.x, max.y, min.z);
			tCorners[3] = vec3(min.x, min.y, max.z);
			tCorners[4] = vec3(max.x, max.y, min.z);
			tCorners[5] = vec3(min.x, max.y, max.z);
			tCorners[6] = vec3(max.x, min.y, max.z);
			tCorners[7] = vec3(max.x, max.y, max.z);
		}


		bool ContainsPoint(const vec3& point)const{
			vec3 min = tCorners[0];
			vec3 max = tCorners[7];
			return (point.x >= min.x && point.y >= min.y &&
				    point.z >= min.z && point.x <= max.x && 
					point.y <= max.y && point.z <= max.z);
		}


		inline vec3			GetCenter()const{ return (tCorners[0] + tCorners[7]) / 2.0f; }
		inline vec3         GetMax()const{ return tCorners[7]; }
		inline vec3         GetMin()const{ return tCorners[0]; }
		vec3 tCorners[8]; 
};
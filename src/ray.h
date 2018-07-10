#pragma once
#ifndef RAY_H
#define RAY_H

#include"vec3.h"

class ray {
public:
	ray(){}
	ray(const vec3& x, const vec3& y) { A=x; B=y; }
	vec3 origin() const{ return A; }
	vec3 direction()const { return B; }
	vec3 point_at_parameter(float t) const{ return A + t*B; }
	vec3 A, B;
};

#endif // !RAY_H

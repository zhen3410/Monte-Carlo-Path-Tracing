#pragma once
#include"vec3.h"
#include"ray.h"
#include"object.h"

class Camera {
public:
	Camera(){}
	void set(vec3 pos, vec3 front, vec3 up,vec3 horizontal) {
		Pos = pos;
		Front = unit_vector(front);
		Up = unit_vector(up);
		Horizontal = unit_vector(horizontal);
		float ratio = (float)width / (float)height;
		float D = tanf(fov / 2.0f*3.141592657f / 180.0f)*0.1f;
		float Dy = D / ratio;
		origin_pos = Pos + Front * 0.1f - Horizontal * D - Up * Dy;
		x_inc = Horizontal * D / width * 2;
		y_inc = Up * Dy / height * 2;
	}
	ray get_ray(int i,int j, float u, float v) {
		vec3 p = origin_pos + x_inc * (i + u) + y_inc * (j + v);
		vec3 d = unit_vector(p - Pos);
		return ray(p, d);
	}
	int width, height;

	vec3 origin_pos, x_inc, y_inc;
	float fov = 60.f;

	vec3 Pos, Front, Up, Horizontal;
};


#ifndef HITABLEH
#define HITABLEH

#include"ray.h"

struct Mtl;

struct hit_record {
	float t;
	vec3 p;
	vec3 normal;
	Mtl* mat_ptr;
};

class hitable {
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const = 0;
};

#endif // !HITABLEH

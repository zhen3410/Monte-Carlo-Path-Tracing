#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include<string>
#include<unordered_map>
#include<vector>

#include"vec3.h"
#include"material.h"
#include"ray.h"
using namespace std;

inline double erand48() {
	return (double)rand() / (double)RAND_MAX;
}

class hit_record {
public:
	bool is_hit = false;
	float t;
	float t_min;
	vec3 p;
	vec3 norm;
	mtl* mtl_ptr;
};

class Object {
public:
	Object(){}
	Object(mtl* m) { mtl_ptr = m; }
	virtual bool hit(const ray& r,hit_record& rec)const;
	void intersect_face(const ray& r, hit_record& rec)const;
	bool IntersectTriangle(const vec3& orig, const vec3& dir,const vec3& v0, const vec3& v1,const vec3& v2,float* t, float* u, float* v)const;
	void update_bounding_box(const vec3& v);
	bool is_empty = true;
	mtl* mtl_ptr;
	vector<vector<vec3> >faces;
private:
	vec3 min = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	vec3 max = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
};

class Object_list :public Object {
public:
	Object_list(){}
	Object_list(vector<Object*>& l, int n) { list.assign(l.begin(), l.end()); list_size = n; }
	virtual bool hit(hit_record& hit,const ray& r)const;
	vector<Object*> list;
	int list_size=0;

	unordered_map<string, mtl*> material;
	void loadObject(string filename);
	void loadmaterial(string filepath,string filename);

};

#endif // !OBJECT_H
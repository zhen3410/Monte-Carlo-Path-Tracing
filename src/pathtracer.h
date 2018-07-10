#pragma once
#ifndef PATHTRACER_H
#define PATHTRACER_H

#define _USE_MATH_DEFINES

#include"camera.h"
#include"object.h"

#include<iostream>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<iomanip>
using namespace std;

class pathtracer {
public:
	pathtracer(){}
	pathtracer(const Camera& c, const Object_list& ol) { cam = c; obj_list = ol; }
	Camera cam;
	Object_list obj_list;
	vec3 color(const ray& r_in,int depth,bool inside=false);
	void loadObj(string filepath);

	vec3 reflection(const vec3& V, const vec3& N);
	vec3 RandomReflection(const vec3& V, const vec3& N);

	vec3 Refracted(const vec3& V, const vec3& N, float Ni, bool& inside, bool& refraction);


	void run();

	vec3 environment_color;
	int light;
	vec3 light_color;

	int sample_num;
	GLubyte* buffer;
};

void pathtracer::loadObj(string filepath) {
	obj_list.loadObject(filepath);
	cout << "模型加载完成..." << endl;
}


inline void pathtracer::run()
{
	FILE* f=fopen("image.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n", cam.width, cam.height, 255);

	buffer = new GLubyte[cam.width*cam.height * 3];
	int C = 0;
	cout << "开始绘制..." << endl;
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < cam.width; i++) {
		for (int j = 0; j < cam.height; j++) {
			vec3 col(0, 0, 0);
			for (int k = 0; k < sample_num; k++) {
				float u = erand48();
				float v = erand48();
				ray r = cam.get_ray(i, j, u, v);
				vec3 c = color(r, 0);
				col += c;
			}
			col /= sample_num;
			
			int ir = (int)(255.99*col[0]);
			int ig = (int)(255.99*col[1]);
			int ib = (int)(255.99*col[2]);
			fprintf(f, "%d %d %d\n", ir, ig, ib);
			if (col[0] > 1) col[0] = 1;
			if (col[1] > 1) col[1] = 1;
			if (col[2] > 1) col[2] = 1;
			buffer[(j*cam.width + i) * 3] += (GLfloat)(col[0] * 255);
			buffer[(j*cam.width + i) * 3 + 1] += (GLfloat)(col[1] * 255);
			buffer[(j*cam.width + i) * 3 + 2] += (GLfloat)(col[2] * 255);
			C++;
			float now = (float)C / (float)(cam.width*cam.height)*100;
			cout << "\r" <<"绘制进度： "<< fixed << setprecision(2) << now << "%";
		}
	}
	cout << "\n绘制完成..." << endl;
}



vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0*vec3(erand48(), erand48(), erand48()) - vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0);
	return p;
}

inline vec3 pathtracer::reflection(const vec3& V, const vec3& N) {
	vec3 uv = unit_vector(V);
	vec3 reflected = 2 * dot(uv, N)*N - uv;
	//return unit_vector(reflected + random_in_unit_sphere());
	return unit_vector(reflected);
	//return V - 2 * dot(V, N)*N;
}
/*
inline vec3 pathtracer::RandomReflection(const vec3& V, const vec3& N) {
	vec3 uv = unit_vector(V);
	vec3 reflected = 2 * dot(uv, N)*N - uv;
	return unit_vector(reflected + random_in_unit_sphere());
	//return unit_vector(reflected);
	//return V - 2 * dot(V, N)*N;
}
*/
inline vec3 pathtracer::RandomReflection(const vec3& V, const vec3& N) {
	double r1 = 2 * M_PI*erand48();
	double r2 = erand48(), r2s = sqrt(r2);
	vec3 w = N;
	vec3 u = unit_vector(cross((fabs(w.e[0]) > .1 ? vec3(0, 1, 0) : vec3(1, 0, 0)), w));
	vec3 v = cross(w, u);
	vec3 d = unit_vector((u*cos(r1)*r2s + v * sin(r1)*r2s + w * sqrt(1 - r2)));
	return d;
}


inline vec3 pathtracer::Refracted(const vec3& V, const vec3& N, float Ni, bool& in_out,bool& refraction) {
	refraction = true;
	float nr = !in_out ? 1 / Ni : Ni;
	in_out = !in_out;
	vec3 out = (nr* dot(N, V) - sqrt(1 - nr * nr*(1 - pow(dot(N, V), 2))))*N - nr * V;
	return out;
}

vec3 pathtracer::color(const ray& r, int depth, bool inside) {
	if (depth >= 7) {
		return vec3(0, 0, 0);
	}

	hit_record rec;
	obj_list.hit(rec, r);

	if (!rec.is_hit) {
		return environment_color;
	}
	if (rec.mtl_ptr->is_light) {
		return light * light_color;
	}

	vec3 N;
	if (dot(rec.norm, r.direction()) > 0)N = -rec.norm;
	else N = rec.norm;

	vec3 V = -r.direction();

	float Tr = rec.mtl_ptr->Tr;
	
	bool refraction = false;

	vec3 L;
	float Ks = rec.mtl_ptr->Ks[0] + rec.mtl_ptr->Ks[1] + rec.mtl_ptr->Ks[2];
	float Kd = rec.mtl_ptr->Kd[0] + rec.mtl_ptr->Kd[1] + rec.mtl_ptr->Kd[2];
	if(Ks>1e-4&&Kd<1e-4)L = erand48() < Tr ? Refracted(V, N, rec.mtl_ptr->Ni, inside, refraction) : reflection(V, N);
	else L = erand48() < Tr ? Refracted(V, N, rec.mtl_ptr->Ni, inside, refraction) : RandomReflection(V, N);


	vec3 Ii = color(ray(rec.p, L), depth + 1, inside);

	float Ns = rec.mtl_ptr->Ns;
	vec3 Ie;
	if (!refraction) {
		vec3 Ka = rec.mtl_ptr->Ka;
		vec3 Ia = environment_color;
		vec3 Kd = rec.mtl_ptr->Kd;
		vec3 Ks = rec.mtl_ptr->Ks;
		vec3 R = 2 * N*dot(N, L) - L;
		Ie = Ka * Ia + Ii * (Kd*dot(N, L) + Ks * pow(dot(V, R), Ns));
	}
	else {
		Ie = Ii;
	}

	return Ie;
}

#endif // !PATHTRACER_H

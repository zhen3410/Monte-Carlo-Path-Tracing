#pragma once
#include<iostream>
#include<string>
using namespace std;

#include"vec3.h"

class mtl {
public:
	std::string name;
	vec3 Ka = vec3(0, 0, 0);
	vec3 Kd = vec3(0, 0, 0);
	vec3 Ks = vec3(0, 0, 0);
	float Ns=10;
	float Tr = 0, Ni = 1.0;
	int illum;
	bool is_light=false;
};
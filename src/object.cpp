#include"stdafx.h"
#include"object.h"
#include<fstream>
#include<sstream>


bool Object::hit(const ray& r, hit_record& rec)const {
	float tmin = (min[0] - r.origin()[0]) / r.direction()[0];
	float tmax = (max[0] - r.origin()[0]) / r.direction()[0];

	if (tmin > tmax)swap(tmin, tmax);

	float tymin = (min[1] - r.origin()[1]) / r.direction()[1];
	float tymax = (max[1] - r.origin()[1]) / r.direction()[1];

	if (tymin > tymax)swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))return false;

	if (tymin > tmin)tmin = tymin;
	if (tymax < tmax)tmax = tymax;

	float tzmin = (min[2] - r.origin()[2]) / r.direction()[2];
	float tzmax = (max[2] - r.origin()[2]) / r.direction()[2];

	if (tzmin > tzmax)swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))return false;

	intersect_face(r, rec);
	return false;
}


void Object::intersect_face(const ray& r, hit_record& rec)const {


	hit_record temp_rec;
	int size = faces.size();
	for (int i = 0; i < size; i++) {
		int face_size = faces[i].size();
		float t, u, v;
		if (IntersectTriangle(r.origin(), r.direction(), faces[i][0], faces[i][1], faces[i][2],&t,&u,&v)) {
			if (t < rec.t_min&&t>1e-4) {
				rec.t_min = t;
				rec.t = t;
				rec.is_hit = true;
				rec.p = r.point_at_parameter(t);
				rec.norm = unit_vector(cross((faces[i][0] - faces[i][1]), (faces[i][0] - faces[i][2])));
				rec.mtl_ptr = mtl_ptr;
			}
		}
		else if (face_size == 4 && IntersectTriangle(r.origin(), r.direction(), faces[i][0], faces[i][2], faces[i][3], &t, &u, &v)) {
			if (t < rec.t_min&&t>1e-4) {
				rec.t_min = t;
				rec.t = t;
				rec.is_hit = true;
				rec.p = r.point_at_parameter(t);
				rec.norm = unit_vector(cross((faces[i][0] - faces[i][2]), (faces[i][0] - faces[i][3])));
				rec.mtl_ptr = mtl_ptr;
			}
		}
	}
}

bool Object::IntersectTriangle(const vec3& orig, const vec3& dir,const vec3& v0, const vec3& v1, const vec3& v2,float* t, float* u, float* v)const
{
	// E1
	vec3 E1 = v1 - v0;

	// E2
	vec3 E2 = v2 - v0;

	// P
	vec3 P = cross(dir,E2);

	// determinant
	float det = dot(E1,P);

	// keep det > 0, modify T accordingly
	vec3 T;
	if (det >0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;

	// Calculate u and make sure u <= 1
	*u = dot(T,P);
	if (*u < 0.0f || *u > det)
		return false;

	// Q
	vec3 Q = cross(T,E1);

	// Calculate v and make sure u + v <= 1
	*v = dot(dir,Q);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = dot(E2,Q);

	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}


bool Object_list::hit(hit_record& hit,const ray& r) const{
	hit.t_min = FLT_MAX;
	hit.is_hit = false;
	for (int i = 0; i < list_size; i++) {
		
		list[i]->hit(r, hit);
	}
	if(hit.is_hit)return true;
	return false;
}

void Object::update_bounding_box(const vec3& v) {
	if (v[0] < min[0])min[0] = v[0];
	if (v[0] > max[0])max[0] = v[0];
	if (v[1] < min[1])min[1] = v[1];
	if (v[1] > max[1])max[1] = v[1];
	if (v[2] < min[2])min[2] = v[2];
	if (v[2] > max[2])max[2] = v[2];
}


void Object_list::loadObject(string filepath) {
	string mtl_file;
	int pos1 = filepath.find_last_of('.');
	int pos2 = filepath.find_last_of('/');
	string filename = filepath.substr(pos2+1, pos1 - pos2 - 1);
	mtl_file = filepath.substr(0,pos1)+".mtl";

	ifstream file(filepath);
	if (!file.is_open()) {
		cout << "读取模型失败!" << endl;
		exit(0);
	}
	list.clear();
	Object* object = new Object;
	vector<vec3> V;
	string line;
	while (getline(file, line)) {
		istringstream record(line);
		string info;
		record >> info;
		if (info == "mtllib") {
			loadmaterial(mtl_file,filename);
		}
		else if (info == "usemtl") {
			if (object->is_empty == false) {
				list.push_back(object);
				list_size++;
			}
			string name;
			record >> name;
			object = new Object;
			if (material.find(name) != material.end())object->mtl_ptr = material[name];
			else {
				mtl* mat = new mtl;
				if (filename == "scene01") {
					mat->is_light = false;
					mat->Ka = vec3(0, 0, 0);
					mat->Kd = vec3(0, 0, 0);
					mat->Ks = vec3(1, 1, 1);
					mat->Ni = 1;
					mat->Ns = 50;
					mat->illum = 4;
				}
				else {
					mat->is_light = false;
					mat->Ka = vec3(0, 0, 0);
					mat->Kd = vec3(0.5, 0.5, 0.5);
					mat->Ks = vec3(1, 1, 1);
					mat->Ni = 1;
					mat->Ns = 5;
					mat->illum = 4;
				}
				object->mtl_ptr = mat;
			}
			object->is_empty = false;
		}
		else {
			if (info == "v") {
				vec3 temp;
				record >> temp.e[0] >> temp.e[1] >> temp.e[2];
				V.push_back(temp);
			}
			else if (info == "f") {
				vector<vec3> F;
				string idx;
				while (record >> idx) {
					int pos = idx.find_first_of('/');
					int temp = 0;
					for (int i = 0; i < pos; i++) {
						temp = temp * 10 + (idx[i] - '0');
					}
					object->update_bounding_box(V[temp - 1]);
					F.push_back(V[temp - 1]);
				}
				object->faces.push_back(F);
			}
		}
	}
	list.push_back(object);
	list_size++;
}

void Object_list::loadmaterial(string filepath,string filename) {
	string light;
	if (filename == "scene01") {
		light = "blinn2S";
	}
	else {
		light = "lambert";
	}
	ifstream file(filepath);
	if (!file.is_open()) {
		cout << "读取材质失败!" << endl;
		exit(0);
	}
	//material.clear();
	string line;
	string name;
	mtl* temp_mtl = new mtl;
	float x, y, z;
	while (getline(file, line)) {
		istringstream record(line);
		string info;
		record >> info;
		if (info == "newmtl") {
			record >> name;
			temp_mtl = new mtl;
			temp_mtl->name = name;
			material[name] = temp_mtl;
			if (filename == "scene01"&&name.substr(0,7) == light) {
				temp_mtl->is_light = true;
			}
			if (filename == "scene02"&&name.substr(0,7) == light) {
				temp_mtl->is_light = true;
			}
		}
		else if (info == "illum") {
			record >> temp_mtl->illum;
		}
		else if (info == "Kd") {
			record >> temp_mtl->Kd.e[0] >> temp_mtl->Kd.e[1] >> temp_mtl->Kd.e[2];
		}
		else if (info == "Ka") {
			record >> temp_mtl->Ka.e[0] >> temp_mtl->Ka.e[1] >> temp_mtl->Ka.e[2];
		}
		else if (info == "Ni") {
			record >> temp_mtl->Ni;
		}
		else if (info == "Ks") {
			record >> temp_mtl->Ks.e[0] >> temp_mtl->Ks.e[1] >> temp_mtl->Ks.e[2];
		}
		else if (info == "Tr") {
			record >> temp_mtl->Tr;
		}
		else if (info == "Ns") {
			record >> temp_mtl->Ns;
		}
	}
}
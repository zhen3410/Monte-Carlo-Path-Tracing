
#include"stdafx.h"
#include<iostream>
#include<GL\glut.h>
#include"pathtracer.h"

pathtracer render;

void init(const char* name) {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(render.cam.width, render.cam.height);
	glutCreateWindow(name);
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, render.cam.width << 6, 0, render.cam.height << 6);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glRasterPos2i(0, 0);
	glDrawPixels(render.cam.width, render.cam.height, GL_RGB, GL_UNSIGNED_BYTE, render.buffer);
	glutSwapBuffers();
}


int main(int argc,char* argv[]) {
	glutInit(&argc, argv);
	render.cam.width = 600;
	render.cam.height = 600;
	cout << "***************************************************************" << endl;
	cout << "--------------------Monte Carlo Path Tracing-------------------" << endl;
	cout << "1. Scene01\n2. Scene02\n3. 结束\n请输入需要渲染的模型序号: ";
	while (1) {
		int ind,num;
		cin >> ind;
		cout << "请输入采样数： ";
		cin >> num;
		if (ind == 1) {
			render.loadObj("./scene_1_obj/scene01.obj");
			render.cam.set(vec3(0, 5, 15), vec3(0, 0, -1), vec3(0, 1, 0), vec3(1, 0, 0));
			render.environment_color = vec3(1, 1, 1);
			render.light_color = vec3(1, 1, 1);
			render.light = 40;

			render.sample_num = num;
		}
		else if (ind == 2) {
			render.loadObj("./scene_2_obj/scene02.obj");
			render.cam.set(vec3(1.5, 8.0, 19), vec3(0, -0.35, -1), vec3(0, 1, 0), vec3(1, 0, 0));
			render.environment_color = vec3(1, 1, 1);
			render.light_color = vec3(1, 1, 1);
			render.light = 15;
			render.sample_num = num;

		}
		else if (ind == 3) {
			exit(0);
		}
		init("result");
		render.run();
		glutDisplayFunc(display);
		glutMainLoop();

	}
	return 0;
}



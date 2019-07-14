#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class ClothSystem: public ParticleSystem {

public:

	ClothSystem(int grid_x, int grid_y, float ds); // to set-up cloth system
	vector<Vector3f> evalF(vector<Vector3f> state); // to evaluate force
	
	// for drawing the scene
	virtual void render_toggle() {};
	virtual void motion_toggle() {};
	void draw();

private:

	// private class variables for cloth system
	int height;
	int width;
	float spacing;
	bool render;
	vector<vector<vector<int>>> spring_indices;

	// necessary methods for cloth system
	Vector3f get_gravity ();
	Vector3f get_drag (Vector3f v);
	Vector3f get_net_force (vector<Vector3f> state, int idx);
	int get_index (int row, int col);
	void draw_cloth (int row, int col);
	void draw_line (int row1, int col1, int row2, int col2);
};


#endif



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
	ClothSystem(int num_particles);
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
	void render_toggle();
	void motion_toggle();

private:

	int grid_size;
	bool render;
	bool move;
	bool backward;
	vector<int> s_points;

	// additional required methods
	int get_index(int row, int col);
	Vector3f get_position (int row, int col, vector<Vector3f> state);
	Vector3f get_velocity (int row, int col, vector<Vector3f> state);
	Vector3f get_gravity ();
	Vector3f get_drag (Vector3f v);
	Vector3f get_spring (char type, int row1, int col1, int row2, int col2);
	Vector3f get_force (int row, int col, vector<Vector3f> state);
	Vector3f get_motion (Vector3f pos);

	void draw_line(int row1, int row2, int col1, int col2);
	void draw_cloth(int row, int col);

};


#endif



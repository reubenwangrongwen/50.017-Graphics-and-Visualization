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

	ClothSystem(int n = n0, double L = L0); // to set-up cloth system
	vector<Vector3f> evalF(vector<Vector3f> state); // to evaluate force

	// for drawing the scene
	virtual void render_toggle();
	virtual void motion_toggle();
	void draw();

	// for quantum state time-evolution
	/*vector<Vector3f> ClothSystem::state_update(vector<Vector3f> state);*/
	void set_Qstate(const cdV& State) { Qstate = State; } // setter method for the quantum system's state
	cdV get_Qstate() { return Qstate; } // getter method for the quantum system's state
	void set_Qstate_vel(const cdV& StateVel) { Qstate_vel = StateVel; } // setter method for the quantum system's state
	cdV get_Qstate_vel() { return Qstate_vel; } // getter method for the quantum system's state velocity


private:

	//// quantum state variable
	//cdV Qstate_vel = Eigen::VectorXcd::Zero(width);
	//cdV Qstate = this->ISW_eigenstate(n0, L0, x_domain, 0.); // get wavefunction;
	//double time = 0.; // time for quantum time_evolution

	// render and motion toggle booleans
	bool render;
	bool motion;

	// private class variables for cloth system
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

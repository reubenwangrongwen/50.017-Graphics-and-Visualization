// #include "particleSystem.h"
#include "ClothSystem.h"
#include <iostream>

using namespace std;

// simulation parameters
float mass = 0.25f; // mass
float g = 9.81f; // gravity
float b = 2.f; // viscous drag coefficient

float k_st = 200.f; // structural spring coefficient
float k_sh = 40.f; // shear spring coefficient
float k_f = 200.f; // flexion spring coefficient



vector<vector<int>> get_spring_indices (int grid_x, int grid_y, int i, int j) {
	/*
	Description:
		Gets the relevant spring indices to compute the spring 
		force for the current particle in question.
	Arguments:
		- grid_x: number of particles along x-direction (horizontal)
		- grid_y: number of particles along y-direction (vertical)
		- i,j: indices of current particle
	Returns:
		2D array of spring indices (vector of vectors).
	*/

	// set up all related points with struct, shear and flex springs
	vector<vector<int>> spring_idx; // vector of spring index vectors
	vector<int> st_idx; // array of indices for structural spring
	vector<int> sh_idx; // array of indices for shear spring
	vector<int> f_idx; // array of indices for flexion spring

	// getting structural spring indices
	if (j - 1 >= 0) { st_idx.push_back(i * grid_x + j - 1); }
	if (j + 1 < grid_x) { st_idx.push_back(i * grid_x + j + 1); }
	if (i - 1 >= 0) { st_idx.push_back((i - 1) * grid_x + j); }
	if (i + 1 < grid_y) { st_idx.push_back((i + 1) * grid_x + j); }

	// getting shear spring indices
	if ((j - 1 >= 0) & (i - 1 >= 0)) { sh_idx.push_back((i - 1) * grid_x + j - 1); }
	if ((j + 1 < grid_x) & (i - 1 >= 0)) { sh_idx.push_back((i - 1) * grid_x + j + 1); }
	if ((j - 1 >= 0) & (i + 1 < grid_y)) { sh_idx.push_back((i + 1) * grid_x + j - 1); }
	if ((j + 1 < grid_x) & (i + 1 < grid_y)) { sh_idx.push_back((i + 1) * grid_x + j + 1); }

	// getting flexion springs indices
	if (j - 2 >= 0) { f_idx.push_back(i * grid_x + j - 2); }
	if (j + 2 < grid_x) { f_idx.push_back(i * grid_x + j + 2); }
	if (i - 2 >= 0) { f_idx.push_back((i - 2) * grid_x + j); }
	if (i + 2 < grid_y) { f_idx.push_back((i + 2) * grid_x + j); }

	// appending spring related indices
	spring_idx.push_back(st_idx);
	spring_idx.push_back(sh_idx);
	spring_idx.push_back(f_idx);

	return spring_idx;
}


ClothSystem::ClothSystem (int n0, double L0) {

	// initializing particle cloth-grid parameters
	this->motion = false; // init motion boolean
	this->render = false; // init rendering boolean 
	
	// setting class objects
	m_numParticles = height * width; // total number of particles
	double spacing = this->get_width() / width; // grid particle spacing

	// declaring variable vector of spring index vectors
	vector<vector<int>> spring_idx; 

	// loop over vertical axis
	for (int dy = 0; dy < height; dy++) { // m: height

		// loop over horizontal axis
		for (int dx = 0; dx < width; dx++) { // n: width

			// appending position and velocity vectors
			// m_vVecState.push_back(Vector3f(spacing * (dx - (width / 2.)), - spacing * dy, 0.));
			cdV quantum_state = this->get_Qstate();
			m_vVecState.push_back(Vector3f(spacing * (dx - width / 2.), dy * quantum_state(dx).real() / height, dy * quantum_state(dx).imag() / height));
			// cout << dy * quantum_state(dx).real() / height << dy* quantum_state(dx).imag() / height << endl;
			m_vVecState.push_back(Vector3f(0, 0, 0));			
			
			// getting all indices for springs
			spring_idx = get_spring_indices(height, width, dy, dx);
			spring_indices.push_back(spring_idx);
		}
	}
}


Vector3f ClothSystem::get_gravity () {
	/*
	Description:
		Computes gravitational force.
	Arguments:
		-
	Returns:
		Gravitational force vector.
	*/

	return -mass * Vector3f(0, g, 0);
}


Vector3f ClothSystem::get_drag (Vector3f v) {
	/*
	Description:
		Computes drag force.
	Arguments:
		- v: velocity vector.
	Returns:
		Viscous drag force vector.
	*/

	return -b * v;
}


Vector3f ClothSystem::get_net_force(vector<Vector3f> state, int idx) {
	/*
	Description:
		Gets the net force acting on the current particle with index idx.
	Arguments:
		- state: current state-space vector for cloth-particle system.
		- idx: particle index.
	Returns:
		Net force acting on current particle (Vector3f).
	*/

	// declaring variables 
	float l_e; // spring equilibrium length
	Vector3f del_x; // spring displacement
	Vector3f F_s;
	Vector3f F_N = Vector3f(0.f, 0.f, 0.f); // init net force vector
	vector<int> st_idx = spring_indices[idx / 2][0];
	vector<int> sh_idx = spring_indices[idx / 2][1];
	vector<int> f_idx = spring_indices[idx / 2][2];
	double spacing = this->get_width() / width; // grid particle spacing

	// adding drag and gravitational forces
	F_N += get_gravity(); // gravitational force
	F_N += get_drag(state[idx + 1]); // drag force

	// adding forces for various springs
	for (int n = 0; n < st_idx.size(); n++) { // structural springs
		
		// getting current spring force
		l_e = spacing; // structural spring equilibrium length
		del_x = state[idx] - state[2 * st_idx[n]]; // spring displacement
		if (del_x.abs() == 0.) {
			F_s = Vector3f::ZERO;
		}
		else {
			F_s = -k_st * (del_x.abs() - l_e) * del_x / del_x.abs(); // spring force
		}
		
		// adding to net force
		F_N += F_s;
	}
	for (int n = 0; n < sh_idx.size(); n++) { // shear springs

		// getting current spring force
		l_e = spacing / sqrt(2.); // shear spring equilibrium length
		del_x = state[idx] - state[2 * sh_idx[n]]; // spring displacement
		if (del_x.abs() == 0.) {
			F_s = Vector3f::ZERO;
		}
		else {
			F_s = -k_sh * (del_x.abs() - l_e) * del_x / del_x.abs(); // spring force
		}
		
		// adding to net force
		F_N += F_s;
	}
	for (int n = 0; n < f_idx.size(); n++) { // flexion springs

		// getting current spring force
		l_e = spacing * 2.; // flexion spring equilibrium length
		del_x = state[idx] - state[2 * f_idx[n]]; // spring displacement
		if (del_x.abs() == 0.) {
			F_s = Vector3f::ZERO;
		}
		else {
			F_s = -k_f * (del_x.abs() - l_e) * del_x / del_x.abs(); // spring force
		}
		
		// adding to net force
		F_N += F_s;
	}

	if ((idx >= 0) || (idx <= (width - 1) * 2)) {
		return Vector3f::ZERO;
	}

	return F_N;
}


void ClothSystem::motion_toggle() {
	this->motion = !this->motion;
}

// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state) {
	/*
	Description:
		Evaluates forces for current state.
	Arguments:
		- state: current state-space vectors.
	Returns:
		Array of force vectors.
	*/

	// declaring variables
	Vector3f net_force;
	vector<Vector3f> force;

	// loop over particles
	for (unsigned idx = 0; idx < state.size(); idx += 2) {

		if ((idx >= 0) && (idx <= (width - 1) * 2)) { // boundary particles
		// if ((idx == 0) || (idx == (width - 1) * 2)) { // boundary particles
			Vector3f edge_force = Vector3f::ZERO; // cloth is stationary
			force.push_back (state[idx + 1]);
			force.push_back(edge_force);
		}
		else { // non-boundary particles 
			force.push_back (state[idx + 1]);
			net_force = get_net_force (state, idx);
			force.push_back(net_force);
			// cout << net_force[0] << net_force[1] << net_force[2] << endl;
		}
	}

	return force;
}


int ClothSystem::get_index(int row, int col) {
	/*
	Description:
		Takes in current  2D array index and converts it to a flattened array index.
	Arguments:
		- row: input row index
		- col: input column index
	Returns:
		Equivalent index in the flattened array.
	*/

	return row * (width) + col;
}


void ClothSystem::render_toggle() {
	this->render = !this->render;
}

void ClothSystem::draw_cloth(int row, int col) {

	// declaring variables
	vector<Vector3f> current_state;
	Vector3f p1, p2, p3, p4; // position vectors
	Vector3f n1, n2, n3, n4, n5, n6; // normal vectors

	current_state = this->getState(); // init current state
	
	// getting vertices
	p1 = current_state[get_index(row, col) * 2]; 
	p2 = current_state[get_index(row, col + 1) * 2];
	p3 = current_state[get_index(row + 1, col) * 2];
	p4 = current_state[get_index(row + 1, col + 1) * 2];

	// computing normals
	n1 = Vector3f::cross(p3 - p1, p2 - p1); n1.normalize();
	n2 = Vector3f::cross(p2 - p3, p1 - p3); n2.normalize();
	n3 = Vector3f::cross(p1 - p2, p3 - p2); n3.normalize();
	n4 = Vector3f::cross(p3 - p2, p4 - p2); n4.normalize();
	n5 = Vector3f::cross(p4 - p3, p2 - p3); n5.normalize();
	n6 = Vector3f::cross(p2 - p4, p3 - p4); n6.normalize();

	// loading normals and vertices
	glBegin(GL_TRIANGLES);
	glColor3f(abs(n1[0]), abs(n1[1]), abs(n1[2])); // coloring triangles
	glNormal3f(n1[0], n1[1], n1[2]); glVertex3f(p1[0], p1[1], p1[2]);
	glNormal3f(n2[0], n2[1], n2[2]); glVertex3f(p3[0], p3[1], p3[2]);
	glNormal3f(n3[0], n3[1], n3[2]); glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(abs(n4[0]), abs(n4[1]), abs(n4[2])); // coloring triangles
	glNormal3f(n4[0], n4[1], n4[2]); glVertex3f(p2[0], p2[1], p2[2]);
	glNormal3f(n5[0], n5[1], n5[2]); glVertex3f(p3[0], p3[1], p3[2]);
	glNormal3f(n6[0], n6[1], n6[2]); glVertex3f(p4[0], p4[1], p4[2]);
	glEnd();
}


void ClothSystem::draw_line(int row1, int col1, int row2, int col2) {

	// declare variables
	Vector3f p1, p2;

	if (row1 < 0 || row1 >= height || row2 < 0 || row2 >= height ||
		col1 < 0 || col1 >= width || col2 < 0 || col2 >= width) {
		return;
	}

	// getting positions
	p1 = (this->getState())[get_index(row1, col1) * 2];
	p2 = (this->getState())[get_index(row2, col2) * 2];

	glLineWidth(2.5); // setting interpolation line width
	glBegin(GL_LINES);
	glVertex3f(p1[0], p1[1], p1[2]);
	glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();
}


void ClothSystem::draw()
{
	// declaring variable
	Vector3f pos;

	// checking render toggle boolean variable
	if (render) { // draw smooth cloth 
		for (int i = 0; i < height - 1; i++) {
			for (int j = 0; j < width - 1; j++) {
				draw_cloth (i, j); 
			}
		}
	}
	else { // draw grid
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				
				pos = (this->getState())[get_index(i, j) * 2];

				glPushMatrix();
				glTranslatef(pos[0], pos[1], pos[2]);
				/*glutSolidSphere(0.075f, 10.0f, 10.0f);*/
				glPopMatrix();
				draw_line (i, j, i - 1, j);
				draw_line (i, j, i + 1, j);
				draw_line (i, j, i, j - 1);
				draw_line (i, j, i, j + 1);
			}
		}
	}
}

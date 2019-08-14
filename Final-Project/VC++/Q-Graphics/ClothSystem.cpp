#include "ClothSystem.h"
#include <iostream>


using namespace std;


// simulation parameters
float mass = 0.2f; // mass
float g = 0.5f; // gravity
float b = 0.01f; // viscous drag coefficient

float k_st = 400.f; // structural spring coefficient
float k_sh = 80.f; // shear spring coefficient
float k_f = 400.f; // flexion spring coefficient



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
		l_e = 0.8 * spacing; // structural spring equilibrium length
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
		l_e = 0.8 * spacing / sqrt(2.); // shear spring equilibrium length
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
		l_e = 0.8 * spacing * 2.; // flexion spring equilibrium length
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
	
	if (((idx >= 0) && (idx <= (width - 1) * 2)) || ((idx >= state.size() - (width - 1) * 2) && (idx < state.size()))) {
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
	Vector3f net_force, edge_force;
	vector<Vector3f> force;
	float a_c; // centripetal acceleration

	// getting class instances and parameters for quantum state
	double L = this->get_width(); // width of ISW
	int n = this->get_level(); // quantization index
	cdV psi(width), psi_new(width), wf_vel(width), wf_force(width); // quantum state vectors
	double spacing = this->get_width() / width; // grid particle spacing

	// ---------------------------- for cloth boundary (quantum state) ----------------------------
	// getting time
	double t = this->get_time();

	// init model parameters
	psi = this->get_Qstate(); // get wavefunction

	// computing next wavefunction
	this->set_time(t + dt); // updating time for quantum time-evolution
	psi_new = this->ISW_eigenstate(n, L, x_domain, t); // computing time-evolved state
	for (unsigned n_i = n + 1; n_i < n + 5; n_i++) {
		psi_new += this->ISW_eigenstate(n_i, L, x_domain, t); // computing time-evolved state
	}
	psi_new.normalize();
	this->set_Qstate(psi_new); // updating quantum state

	// computing quantum state 'rope' velocity and force
	wf_vel = (psi_new - psi) / dt; // computing quantum state current velocity
	wf_force = (wf_vel - this->get_Qstate_vel()) / dt; // computing quantum state force
	this->set_Qstate_vel(wf_vel); // updating quantum state velocity
	// --------------------------------------------------------------------------------------------

	// loop over particles
	for (unsigned idx = 0; idx < state.size(); idx += 2) {

		if ((idx >= state.size() - (width - 1) * 2) && (idx < state.size())) { // boundary particles
			
			//force.push_back(state[idx + 1]);

			int j = idx - (state.size() - (width - 1) * 2); 
			float scale = float(j) / float(state.size());

			edge_force = Vector3f(0., scale * wf_force[j / 2].real(), scale * wf_force[j / 2].imag());
			if (edge_force.abs() != 0.) { edge_force.normalize(); } // normalizing quantum state force

			force.push_back(5 * edge_force);

			edge_force = Vector3f(0., scale * wf_force[j / 2 + 1].real(), scale * wf_force[j / 2 + 1].imag());
			if (edge_force.abs() != 0.) { edge_force.normalize(); } // normalizing quantum state force

			force.push_back(5 * edge_force);
		}
		else { // non-boundary particles 
			force.push_back (state[idx + 1]);
			net_force = get_net_force (state, idx);
			force.push_back(net_force);
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

	// --------------- the other side of the cloth --------------- 
	// getting vertices
	p1 = -p1;
	p2 = -p2;
	p3 = -p3;
	p4 = -p4;

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
	float red, green, blue;

	if (row1 < 0 || row1 >= height || row2 < 0 || row2 >= height ||
		col1 < 0 || col1 >= width || col2 < 0 || col2 >= width) {
		return;
	}

	// getting positions
	p1 = (this->getState())[get_index(row1, col1) * 2];
	p2 = (this->getState())[get_index(row2, col2) * 2];

	// getting colors 
	red = (float)rand() / (float)RAND_MAX;
	green = (float)rand() / (float)RAND_MAX;
	blue = (float)rand() / (float)RAND_MAX;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glLineWidth(2.5); // setting interpolation line width
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3f(red, green, blue);
	glVertex3f(p1[0], p1[1], p1[2]);
	glVertex3f(p2[0], p2[1], p2[2]);
	glVertex3f(-p1[0], -p1[1], -p1[2]);
	glVertex3f(-p2[0], -p2[1], -p2[2]);
	glEnd();
	glPopMatrix();
	glPopAttrib();

	//glLineWidth(2.5); // setting interpolation line width
	//glBegin(GL_LINES);
	//glVertex3f(p1[0], p1[1], p1[2]);
	//glVertex3f(p2[0], p2[1], p2[2]);
	//glEnd();
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

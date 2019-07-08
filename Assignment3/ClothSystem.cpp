#include "ClothSystem.h"


// simulation parameters
float m = 0.005f; // mass
float g = 9.81f; // gravity
float b = 0.1f; // viscous drag

// spring constants
float k_st = 2.0f;
float k_sh = 2.0f;
float k_f = 2.0f;

// equilibrium lengths
float l_st = 0.5f;
float l_sh = 0.75f;
float l_f = 1.0f;


ClothSystem::ClothSystem(int grid_size) {

	// declaring variables
	Vector3f pos, vel;

	// init class attributes
	this->grid_size = grid_size;
	this->render = false;
	this->move = false;
	this->backward = true;

	// loop over the grid
	for (int i = 0; i < grid_size; i++) {
		for (int j = 0; j < grid_size; j++) {

			pos = Vector3f(j * 0.5, 0, -i * 0.5);
			vel = Vector3f(0, 0, 0);

			this->m_vVecState.push_back(pos); // appending positions 
			this->m_vVecState.push_back(vel); // appending velocities
		}
	}

	// updating class points attribute
	this->s_points.push_back(get_index(0, 0) * 2);
	this->s_points.push_back(get_index(0, grid_size - 1) * 2);
}

int ClothSystem::get_index (int row, int col) {
	return row * (this->grid_size) + col;
}

Vector3f ClothSystem::get_position (int row, int col, vector<Vector3f> state) {
	return state[get_index(row, col) * 2];

}
Vector3f ClothSystem::get_velocity (int row, int col, vector<Vector3f> state) {
	return state[get_index(row, col) * 2 + 1];
}

Vector3f ClothSystem::get_gravity () {
	return m * Vector3f(0, -g, 0);
}

Vector3f ClothSystem::get_drag (Vector3f v) {
	return -b * v;
}

Vector3f ClothSystem::get_spring (char type, int row1, int col1, int row2, int col2) {

	if (row1 < 0 || row1 >= this->grid_size || row2 < 0 || row2 >= this->grid_size ||
		col1 < 0 || col1 >= this->grid_size || col2 < 0 || col2 >= this->grid_size) {
		return Vector3f::ZERO;
	}

	Vector3f p1 = get_position (row1, col1, this->getState());
	Vector3f p2 = get_position (row2, col2, this->getState());
	
	// choosing spring type
	if (type == 'st') {
		return -k_st * ((p1 - p2).abs() - l_st) * ((p1 - p2).normalized());
	}
	else if (type == 'sh') {
		return -k_sh* ((p1 - p2).abs() - l_sh) * ((p1 - p2).normalized());
	}
	else if (type == 'f') {
		return -k_f * ((p1 - p2).abs() - l_f) * ((p1 - p2).normalized());
	}
	else {
		return Vector3f::ZERO;
	}
}

Vector3f ClothSystem::get_force(int row, int col, vector<Vector3f> state) {
	
	// declare variables
	Vector3f v, struct_spring, shear_spring, flex_spring;

	// structual spring force
	struct_spring = get_spring ('st', row, col, row - 1, col) +
					get_spring ('st', row, col, row + 1, col) + 
					get_spring ('st', row, col, row, col - 1) + 
					get_spring ('st', row, col, row, col + 1);
	// shear spring force
	shear_spring = get_spring ('sh', row, col, row - 1, col - 1) +
					get_spring ('sh', row, col, row - 1, col + 1) + 
					get_spring ('sh', row, col, row + 1, col - 1) + 
					get_spring ('sh', row, col, row + 1, col + 1);
	// flex spring force
	flex_spring = get_spring ('f', row, col, row - 2, col) +
					get_spring ('f', row, col, row + 2, col) + 
					get_spring ('f', row, col, row, col - 2) + 
					get_spring ('f', row, col, row, col + 2);
	
	v = get_velocity(row, col, state); // velocities

	return get_gravity () + get_drag (v) + (struct_spring + shear_spring + flex_spring);
}

Vector3f ClothSystem::get_motion (Vector3f pos) {

	if (pos.z() < -5) {
		this->backward = false;
		return Vector3f(0, 0, 1);
	}
	else if (pos.z() >= 0) {
		this->backward = true;
		return Vector3f(0, 0, -1);
	}
	else {
		if (backward) {
			return Vector3f(0, 0, -1);
		}
		else {
			return Vector3f(0, 0, 1);
		}
	}
}

// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	for (int i = 0; i < this->grid_size; i++) {
		for (int j = 0; j < this->grid_size; j++) {
			f.push_back(get_velocity (i, j, state));
			f.push_back(get_force(i, j, state) / m);
		}
	}
	for (int i = 0; i < this->s_points.size(); i++) {
		int index = this->s_points[i];
		if (move) {
			f[index] = get_motion (state[index]);
		}
		else {
			f[index] = Vector3f::ZERO;
		}
		f[index + 1] = Vector3f::ZERO;
	}
	return f;
}

void ClothSystem::draw_line (int row1, int col1, int row2, int col2) {
	
	// declare variables
	Vector3f p1, p2;

	if (row1 < 0 || row1 >= this->grid_size || row2 < 0 || row2 >= this->grid_size ||
		col1 < 0 || col1 >= this->grid_size || col2 < 0 || col2 >= this->grid_size) {
		return;
	}

	// getting positions
	p1 = get_position (row1, col1, this->getState());
	p2 = get_position (row2, col2, this->getState());
	
	glLineWidth(2.5); // setting interpolation line width
	glBegin(GL_LINES);
	glVertex3f(p1[0], p1[1], p1[2]); 
	glVertex3f(p2[0], p2[1], p2[2]);
	glEnd();
}

void ClothSystem::render_toggle () {
	this->render = !this->render;
}
void ClothSystem::motion_toggle () {
	this->move = !this->move;
}

void ClothSystem::draw_cloth(int row, int col) {

	// declaring variables
	vector<Vector3f> current_state;
	Vector3f p1, p2, p3, p4; // position vectors
	Vector3f n1, n2, n3, n4, n5, n6; // normal vectors

	current_state = this->getState(); // init current state
	
	// getting vertices
	p1 = get_position (row, col, current_state);
	p2 = get_position (row, col + 1, current_state);
	p3 = get_position (row + 1, col, current_state);
	p4 = get_position (row + 1, col + 1, current_state);

	// computing normals
	n1 = Vector3f::cross(p3 - p1, p2 - p1); n1.normalize();
	n2 = Vector3f::cross(p2 - p3, p1 - p3); n2.normalize();
	n3 = Vector3f::cross(p1 - p2, p3 - p2); n3.normalize();
	n4 = Vector3f::cross(p3 - p2, p4 - p2); n4.normalize();
	n5 = Vector3f::cross(p4 - p3, p2 - p3); n5.normalize();
	n6 = Vector3f::cross(p2 - p4, p3 - p4); n6.normalize();
	
	// loading normals and vertices
	glBegin(GL_TRIANGLES);
	glNormal3f(n1[0], n1[1], n1[2]); glVertex3f(p1[0], p1[1], p1[2]);
	glNormal3f(n2[0], n2[1], n2[2]); glVertex3f(p2[0], p2[1], p2[2]);
	glNormal3f(n3[0], n3[1], n3[2]); glVertex3f(p3[0], p3[1], p3[2]); 
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3f(n4[0], n4[1], n4[2]); glVertex3f(p2[0], p2[1], p2[2]);
	glNormal3f(n5[0], n5[1], n5[2]); glVertex3f(p3[0], p3[1], p3[2]);
	glNormal3f(n6[0], n6[1], n6[2]); glVertex3f(p4[0], p4[1], p4[2]);
	glEnd();
}


void ClothSystem::draw()
{
	if (render) {
		for (int i = 0; i < this->grid_size - 1; i++) {
			for (int j = 0; j < this->grid_size - 1; j++) {
				draw_cloth (i, j);
			}
		}
	}
	else {
		for (int i = 0; i < this->grid_size; i++) {
			for (int j = 0; j < this->grid_size; j++) {
				Vector3f pos = get_position (i, j, this->getState());

				glPushMatrix();
				glTranslatef(pos[0], pos[1], pos[2]);
				glutSolidSphere(0.075f, 10.0f, 10.0f);
				glPopMatrix();
				draw_line (i, j, i - 1, j);
				draw_line (i, j, i + 1, j);
				draw_line (i, j, i, j - 1);
				draw_line (i, j, i, j + 1);
			}
		}
	}
}



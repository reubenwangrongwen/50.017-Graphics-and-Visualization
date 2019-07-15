#include "pendulumSystem.h"



PendulumSystem::PendulumSystem(int numParticles) :ParticleSystem(numParticles)
{
	m_numParticles = static_cast<int>(numParticles / 2.);
	this->m_vVecState.push_back(Vector3f(0., 0., 0.));
	this->m_vVecState.push_back(Vector3f(0., 0., 0.));

	// loop over numner of particles
	for (unsigned i = 1; i < m_numParticles; i++) {
		
		// appending vectors to compute velocity and force
		this->m_vVecState.push_back(Vector3f(pow(-1, i) * 0.5, -0.5 * i, 0.)); // init positions to be alternating
		this->m_vVecState.push_back(Vector3f(0., 0., 0.));
	}
}

vector<Vector3f> get_position (vector<Vector3f> state) {
	/*
	Description:
		Gets the positions from the current state.
	Arguments:
		- state: current state vector of particles.
	Return:
		- vector of positions at the current state.
	*/

	vector<Vector3f> pos; // decalring vector of positions

	// loop over state-space dimensions (even-spaced for positions)
	for (unsigned i = 0; i < state.size(); i += 2) {
		pos.push_back(state[i]);
	}
	return pos;
}

vector<Vector3f> get_velocity (vector<Vector3f> state) {
	/*
	Description:
		Gets the velocities from the current state.

	Arguments:
		- state: current state vector of particles.

	Return:
		- vector of velocities at the current state. 
	*/

	vector<Vector3f> vel; // decalring vector of velocities
	
	// loop over state-space dimensions (even-spaced for velocities)
	for (unsigned i = 1; i < state.size(); i += 2) {
		vel.push_back(state[i]); // appending to velocity array
	}
	return vel;
}

Vector3f get_gravity (float mass) {
	return mass * Vector3f(0., -9.81, 0.);
}

Vector3f get_drag (Vector3f v, float b) {
	/*
	Description:
		Computes the drag force.

	Arguments:
		- v: velocity vector.
		- b: viscous drag factor.

	Return:
		- Viscous drag force (Vector3f).
	*/

	return -b * v;
}

Vector3f get_spring_diffeq (vector<Vector3f> x, int i, int j, float k, float l_e) {
	/*
	Description:
		Computes the elastic force between 2 particles (i, j).

	Arguments:
		- x: vector of positions.
		- i, j: particle indices.
		- k: spring constant.
		- l_e: equilibrium distance.

	Return:
		- Spring force between 2 particles (Vector3f).
	*/

	// declaring variables
	Vector3f pos_i, pos_j, del_x;

	// checking we do not exceed array boundaries
	if ( i < 0 || i >= x.size() || j < 0 || j >= x.size()) {
		return Vector3f::ZERO;
	}

	pos_i = x[i]; pos_j = x[j]; // adjacent particle positions
	del_x = pos_i - pos_j; // relative displacement between particles

	return -k * (del_x.abs() - l_e) * del_x.normalized();
}



// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state) {
	/*
	Description:
		Computes the accelerations of all the system's particles.

	Arguments:
		- state: current state.

	Return:
		- Vector of accelerations for each particle (vector<Vector3f>).
	*/

	// declaring variables
	float m, k, b, r;
	vector<Vector3f> acc, current_pos, current_vel;
	Vector3f spring_diffeq, net_force;
	
	// init parameters and states
	m = 0.05; b = 0.05; k = 1.5; r = 0.4;
	acc.push_back(Vector3f::ZERO);
	acc.push_back(Vector3f::ZERO);
	current_pos = get_position(state);
	current_vel = get_velocity(state);

	// loop over all particles
	for (unsigned i = 1; i < m_numParticles; i++) {
		
		acc.push_back(current_vel[i]); // appending current velocity
		
		// computing spring differential equation terms
		spring_diffeq = get_spring_diffeq (current_pos, i, i - 1, k, r ) +
						get_spring_diffeq (current_pos, i, i + 1 , k, r) +
						get_spring_diffeq (current_pos, i, i - 2, k, 2. * r) +
						get_spring_diffeq (current_pos, i, i + 2, k, 2. * r);
		net_force = get_gravity(m) + get_drag(current_vel[i], b) + spring_diffeq; // computing net force
		acc.push_back(net_force / m); // appending acceleration
	}

	return acc;
}

void PendulumSystem::draw()
{
	vector<Vector3f> positions = get_position(this->getState());
	for (unsigned i = 0; i < m_numParticles; i++) {
		Vector3f pos = positions[i]; //  position of particle i. 

		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		glutSolidSphere(0.075f, 10.0f, 10.0f);
		glPopMatrix();
	}
}


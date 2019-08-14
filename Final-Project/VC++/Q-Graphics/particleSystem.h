#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <vecmath.h>

#include "wavefunction.h"

using namespace std;

class ParticleSystem: public wavefunction {

public:

	ParticleSystem(int num_particles = 0);

	int m_numParticles;
	
	// for a given state, evaluate derivative f(X,t)
	virtual vector<Vector3f> evalF(vector<Vector3f> state) = 0;

	// getter method for the system's state
	vector<Vector3f> getState(){ return m_vVecState; };
	
	// setter method for the system's state
	void setState(const vector<Vector3f> & newState) { m_vVecState = newState; };

	void set_time(double t) { time = t; } // set time for quantum time-evolution
	double get_time() { return time; } // get time for quantum time-evolution
	void ParticleSystem::state_update();
	
	virtual void render_toggle() = 0;
	virtual void motion_toggle() = 0;
	virtual void draw() = 0;
	
protected:

	// quantum state variable
	cdV Qstate_vel = Eigen::VectorXcd::Zero(width);
	//cdV Qstate = this->ISW_eigenstate(n0, L0, x_domain, 0.); // get wavefunction;
	cdV Qstate = this->ISW_uniform_state(n0, L0, x_domain, 0., 5); // get wavefunction
	//cdV Qstate = this->ISW_exp_state(n0, L0, x_domain, 0., 5);
	double time = 0.; // time for quantum time_evolution

	vector<Vector3f> m_vVecState;
	
};

#endif

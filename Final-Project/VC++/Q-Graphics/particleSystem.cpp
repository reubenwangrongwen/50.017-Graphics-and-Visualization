#include "particleSystem.h"

ParticleSystem::ParticleSystem(int nParticles):m_numParticles(nParticles){
}

void ParticleSystem::state_update() {

	double spacing = this->get_width() / width; // grid particle spacing
	double L = this->get_width(); // width of ISW
	int n = this->get_level(); // quantization index
	double t = this->get_time();
	
	cdV psi_new = this->ISW_eigenstate(n, L, x_domain, t); // computing time-evolved state

	// loop over vertical axis
	for (int dy = 0; dy < height; dy += 2) { // m: height
		// loop over horizontal axis
		for (int dx = 0; dx < width; dx += 2) { // n: width

			if ((dy * (width)+dx >= m_vVecState.size() - (width - 1) * 2) && (dy * (width)+dx < m_vVecState.size())) {
				// updating position vectors
				m_vVecState[dy * (width)+dx] = Vector3f(spacing * (dx - width / 2.), psi_new(dx).real(), psi_new(dx).imag());
			}
		}
	}

	this->set_time(t + dt);
}

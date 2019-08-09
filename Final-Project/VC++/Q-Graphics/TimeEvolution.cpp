#include "TimeEvolution.h"


void TimeEvolution::tEvolve(double dt) {
	
	double L = this->get_width(); // width of ISW
	int n = this->get_level(); // quantization index
	
	// init vectors
	cdV psi(width); // state vectors

	// getting time
	double t = this->get_time();

	// init model parameters
	psi = this->ISW_eigenstate(n, L, x_domain, t); // get wavefunction

	// updating quantum state
	this->set_Qstate(psi); 

	// setting time
	this->set_time(t + dt);
}
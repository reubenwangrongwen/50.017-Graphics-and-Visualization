#include "wavefunction.h"


cdV wavefunction::ISW_eigenstate(int n, double L, cdV x, double t) {
	/*
	Description:
		Function that computes the infinite-square well eigenstate given parameters.
	Argumnents :
		- x_c : square - well center
		- n : quantization index
		- L : width of square - well
		- x : x coordinate vector
		- t : current time
	Return :
		Eigenstate (complex vector).
	*/

	// declaring variables
	cplex phase;
	cdV psi(x.size());

	// computing time-evolution phase, exp(-iEt)
	phase = exp(-i * std::pow(n, 2.) * std::pow(pi, 2.) / (2. * m * L) * t);

	// loop over eigenstate vector entries
	for (unsigned i = 0; i < psi.size(); i++) {
		if ((std::abs(x(i)) < x_c - L / 2.) || (std::abs(x(i)) > x_c + L / 2.)) { // checking if wavefunction is within well
			psi(i) = 0.;
		}
		else {
			psi(i) = sqrt(2. / L) * sin((n * pi / L) * (x(i) - x_c + L / 2.)) * phase;
		}
	}

	return psi;
}


cdV wavefunction::ISW_uniform_state(int n, double L, cdV x, double t, int num_states = 5) {
	/*
	Description:
		Function that computes a uniform linear combination of infinite-square well eigenstate given parameters.
	Argumnents :
		- x_c : square - well center
		- n : quantization index
		- L : width of square - well
		- x : x coordinate vector
		- t : current time
		- n : number of states for superposition
	Return :
		ISW state (complex vector).
	*/

	cdV psi;
	psi = ISW_eigenstate(n, L, x_domain, t); // computing time-evolved state
	for (unsigned n_i = n + 1; n_i < n + num_states; n_i++) {
		psi += ISW_eigenstate(n_i, L, x_domain, t); // computing time-evolved state
	}
	psi.normalize();

	return psi;
}


cdV wavefunction::ISW_exp_state(int n, double L, cdV x, double t, int num_states = 5) {
	/*
	Description:
		Function that computes a decaying exponential linear combination of infinite-square well eigenstate given parameters.
	Argumnents :
		- x_c : square - well center
		- n : quantization index
		- L : width of square - well
		- x : x coordinate vector
		- t : current time
		- n : number of states for superposition
	Return :
		ISW state (complex vector).
	*/

	cdV psi;
	psi = exp(-n) * ISW_eigenstate(n, L, x_domain, t); // computing time-evolved state
	for (unsigned n_i = n + 1; n_i < n + num_states; n_i++) {
		psi += exp(-n_i) * ISW_eigenstate(n_i, L, x_domain, t); // computing time-evolved state
	}
	psi.normalize();

	return psi;
}


cplex wavefunction::coeff(int n1, double L1, int n, double L, double dx, cdV x, double t) {
	/*
	Description:
		Function that computes the eigenstate coefficient.
	Argumnents :
		- m : particle mass
		- x_c : square - well center
		- n0 : prior quantization index
		- L0 : prior width of square - well
		- n : posterior quantization index
		- L : posterior width of square - well
		- dx : numerical step size
		- x : x coordinate vector
		- t : current time
	Return :
		Eigenstate coefficient.
	*/

	cdV integral, eigenstate_old, eigenstate_new;

	eigenstate_old = ISW_eigenstate(n1, L1, x, t);
	eigenstate_new = ISW_eigenstate(n, L, x, t);

	integral = eigenstate_old.cwiseProduct(eigenstate_new);
	return  integral.sum() * dx;
}


cdV wavefunction::wavefun(wavefunction* wf, cdV coeffs, cdM wavefunctions) {
	/*
	Description:
		Function that computes the linear combination of eigenstate.
	Argumnents :
		-c_i : vector of coefficients
		- wavefunctions : matrix of wavefunctions
	Return :
		The wavefunction vector.
	*/

	//this->set_state(wavefunctions * coeffs);

	return wavefunctions * coeffs;
}

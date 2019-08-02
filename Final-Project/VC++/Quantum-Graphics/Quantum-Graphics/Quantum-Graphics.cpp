// Quantum-Graphics.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Header.h"
#include "wavefunction.h"
#include <iostream>

using namespace std;

int main() {
	
	// simulation numerical constants
	int dim = 100; // relevant size of Hilbert space
	int num_t = 10; // number of dt steps
	int F_end = 25; // number of Fourier series terms

	// model parameters
	int n0 = 1;
	double x_min = -5., x_max = 5.; // domain 
	double dx; // numerical step sizes
	double L0, L_new = 2.; // width of ISW

	// simulation variables
	wavefunction ket; // wavefunction class instance
	cdV x(dim); // coordinate space vector
	cdV c(F_end); // complex vector of eigenstate coefficients
	cdV psi_0(dim), psi(dim), wf(dim); // state vectors
	cdM wfs(dim, F_end); // matrix of Fourier term eigenstates 

	// init model parameters
	dx = (x_max - x_min) / double(dim); // x step size
	x.setLinSpaced(dim, x_min, x_max); // init x-coordinate domain vector
	L0 = 1.; // init starting width of ISW

	
	psi_0 = ket.ISW_eigenstate (m, x_c, n0, L0, x, 0); // init wavefunction
	cout << psi_0 << endl; 

	// loop over times
	for (unsigned t_i = 0; t_i < num_t; t_i) {
		
		// progressively increasing width of square - well
		if ((t_i + 1) % 20 == 0) {
			L0 = L_new;
			L_new = L_new + 0.5;
			// F_end = F_end + 50; cout << psi << endl;
		}

		// loop over Fourier expansion terms
		for (unsigned i = 0; i < F_end; i++) {
			
			// coefficients for linear combination
			c(i) = ket.coeff (m, x_c, n0, L0, i + 1, L_new, dx, x, (t_i + 1) * dt);
			
			// eigenstates for linear combination
			wf = ket.ISW_eigenstate(m, x_c, i + 1, L_new, x, (t_i + 1) * dt);
			wfs.block(0, i, dim, 1) = wf;
		}

		psi = ket.wavefun(c, wfs); // new state
	}
	
	cout << psi << endl;
}


#ifndef WAVEFUNCTION_H
#define WAVEFUNCTION_H

#include "Header.h"

class wavefunction {

public:
	
	wavefunction () {} // constructor
	~wavefunction () {} // destructor

	cdV ISW_eigenstate ( const double m, const double x_c, int n, double L, cdV x, double t ) {

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

	cplex coeff(double m, double x_c, int n0, double L0, int n, double L, double dx, cdV x, double t) {
		/*
		Description:
			Function that computes the eigenstate coefficient.
		Argumnents :
			- m : particle mass
			- x_c : square - well center
			- n0 : initial quantization index
			- L0 : initial width of square - well
			- n : new quantization index
			- L : new width of square - well
			- dx : numerical step size
			- x : x coordinate vector
			- t : current time
		Return :
			Eigenstate coefficient.
		*/

		cdV integral, eigenstate_old, eigenstate_new; 
		
		eigenstate_old = ISW_eigenstate(m, x_c, n0, L0, x, t);
		eigenstate_new = ISW_eigenstate(m, x_c, n, L, x, t);

		integral = eigenstate_old.cwiseProduct(eigenstate_new);
		return  integral.sum() * dx;
	}

	cdV wavefun(cdV coeffs, cdM wavefunctions) {
		/*
		Description:
			Function that computes the linear combination of eigenstate.
		Argumnents :
			-c_i : vector of coefficients
			- wavefunctions : matrix of wavefunctions
		Return :
			The wavefunction vector.
		*/
		
		return wavefunctions * coeffs;
	}
	
	
private:

	
};





#endif // WAVEFUNCTION_H
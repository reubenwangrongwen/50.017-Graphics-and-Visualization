#ifndef WAVEFUNCTION_H
#define WAVEFUNCTION_H

#include "Header.h"

class wavefunction {

public:

	wavefunction() {} // constructor
	/*
	Arguments:
		- n: quantization index
		- x_min: lower bound of coordinate-space domain
		- x_max: lower bound of coordinate-space domain
		- L0: initial width of ISW
	*/

	~wavefunction() {} // destructor

	// method declarations
	cdV ISW_eigenstate(int n, double L, cdV x, double t);
	cplex coeff(int n1, double L1, int n, double L, double dx, cdV x, double t);
	cdV wavefun(wavefunction* wf, cdV coeffs, cdM wavefunctions);

	// method definitions
	//void set_params(int n, double min_x, double max_x, double _L) { n0 = n; x_min = min_x; x_max = max_x; L0 = _L; }
	//std::vector <double> get_params() {

	//	std::vector<double> temp;
	//	temp.push_back(m);
	//	temp.push_back(x_min);
	//	temp.push_back(x_max);
	//	temp.push_back(L0);
	//	temp.push_back(n0);

	//	return temp;
	//}

	void set_width(double ISW_width) { L = ISW_width; } // sets width of ISW to argument value
	double get_width() { return L; } // returns width of the ISW


	void set_level(int level) { n = level; } // sets quantization index to argument value
	int get_level() { return n; } // returns quantization index
	

	//void set_state(const cdV& State) { Qstate = State; } // setter method for the quantum system's state
	//cdV get_state() { return Qstate; } // setter method for the quantum system's state

	//
	//cdV Qstate;


private:

	//double m = 0.1; // mass of quantum particle
	//double x_min = -5.;
	//double x_max = 5.;
	//double L0 = 1.;
	//int n0 = 1;

	// width variables
	int n; // quantization index
	double L = L0;

};





#endif // WAVEFUNCTION_H
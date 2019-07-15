#include "TimeStepper.hpp"
#include <iostream>

using namespace std;


void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize) {
	/*
	Description:
		Running the fowar Euler numerical integration ODE solver.
	Arguments:
		- particleSystem: pointer to ParticleSystem class.
		- stepSize: input step size for numerical integration
	Return:
		-
	*/

	// declaring variables
	vector<Vector3f> current_states, next_states, f; // vector of state-vectors
	Vector3f state; // temporary state 

	current_states = particleSystem->getState(); // init current state
	f = particleSystem->evalF(current_states); // init EOM-state for integration

	// loop over all state-vectors (numerical integration)
	for (int i = 0; i < current_states.size(); i++) {
		state = current_states[i] + stepSize * f[i]; // forward Euler integration
		next_states.push_back(state); // appending state for next iteration
	}

	particleSystem->setState(next_states); // resetting state
}



void Trapezoidal::takeStep(ParticleSystem* particleSystem, float stepSize) {
	/*
	Description:
		Running the Trapezoidal numerical integration ODE solver.
	Arguments:
		- particleSystem: pointer to ParticleSystem class.
		- stepSize: input step size for numerical integration
	Return:
		-
	*/

	// declaring variables
	vector<Vector3f> current_state, next_state_0, next_state_1, f_0, f_1;
	Vector3f state;

	// initializing states and EOM-state for trapezoidal solver
	current_state = particleSystem->getState();
	f_0 = particleSystem->evalF(current_state);
	
	// loop for numerical ODE integration
	for (int i = 0; i < current_state.size(); i++) {
		state = current_state[i] + stepSize * f_0[i];
		next_state_0.push_back(state);
	}

	f_1 = particleSystem->evalF(next_state_0); 

	// loop for numerical ODE integration
	for (int i = 0; i < current_state.size(); i++) {
		state = current_state[i] + (stepSize / 2.) * (f_0[i] + f_1[i]);
		next_state_1.push_back(state);
	}

	particleSystem->setState(next_state_1);
}




void RK4::takeStep(ParticleSystem* particleSystem, float stepSize) {
	/*
	Description:
		Running the Runge-Kutta 4th order numerical integration ODE solver.
	Arguments:
		- particleSystem: pointer to ParticleSystem class.
		- stepSize: input step size for numerical integration
	Return:
		- Optimal step size for the current iteration (float).
	*/

	// declaring variables
	vector<Vector3f> current_state, next_state1, next_state2, next_state3, final_state, K1, K2, K3, K4;
	Vector3f state;

	
	// computing the 4 terms for the Runge-Kutta solver
	current_state = particleSystem->getState(); // initializing states for RK 4th order solver	
	K1 = particleSystem->evalF(current_state); // RK 4th order term 1

	for (int i = 0; i < current_state.size(); i++) { // loop for RK 4th order term 2
		state = current_state[i] + (stepSize / 2.) * K1[i];
		next_state1.push_back(state);
	}
	K2 = particleSystem->evalF(next_state1);
	
	for (int i = 0; i < current_state.size(); i++) { // loop for RK 4th order term 3
		state = current_state[i] + (stepSize / 2.) * K2[i];
		next_state2.push_back(state);
	}
	K3 = particleSystem->evalF(next_state2);
	
	for (int i = 0; i < current_state.size(); i++) { // loop for RK 4th order term 4
		state = current_state[i] + stepSize * K3[i];
		next_state3.push_back(state);
	}
	K4 = particleSystem->evalF(next_state3);
	
	for (int i = 0; i < current_state.size(); i++) { // final iterative sequence for RK4 numerical integration
		state = current_state[i] + (stepSize / 6.) * (K1[i] + 2.*K2[i] + 2.*K3[i] + K4[i]);
		final_state.push_back(state);
	}

	particleSystem->setState(final_state);

}



// medium extra credit component
float optimal_step_size(ParticleSystem* particleSystem, float stepSize, float tol_h) {
	/*
	Description:
		Computes the optimal step size for the Runge-Kutta-Fehlberg numerical integration ODE solver.
	Arguments:
		- particleSystem: pointer to ParticleSystem class.
		- stepSize: input step size for numerical integration
		- tol_h: comparative scalar tolerance value. 
	Return:
		- 
	*/

	// declaring variables
	vector<Vector3f> current_state, next_state1, next_state2, next_state3, next_state4, next_state5;
	vector<Vector3f> K1, K2, K3, K4, K5, K6;
	Vector3f state, state1, state2;
	float denom = 0.; // for comparative factor

	// computing the 6 terms for the Runge-Kutta-Fehlberg solver
	current_state = particleSystem->getState(); // initializing states for RKF45 solver
	K1 = particleSystem->evalF(current_state); // RKF45 term 1

	for (int i = 0; i < current_state.size(); i++) { // loop for RKF45 term 2
		state = current_state[i] + (stepSize / 4.) * K1[i];
		next_state1.push_back(state);
	}
	K2 = particleSystem->evalF(next_state1);

	for (int i = 0; i < current_state.size(); i++) { // loop for RKF45 term 3
		state = current_state[i] + (3. * stepSize / 32.) * K1[i]
			+ (9. * stepSize / 32.) * K2[i];
		next_state2.push_back(state);
	}
	K3 = particleSystem->evalF(next_state2);

	for (int i = 0; i < current_state.size(); i++) { // loop for RKF45 term 4
		state = current_state[i] + (1932. * stepSize / 2197.) * K1[i]
			+ (7200. * stepSize / 2197.) * K2[i]
			+ (7296. * stepSize / 2197.) * K3[i];
		next_state3.push_back(state);
	}
	K4 = particleSystem->evalF(next_state3);

	for (int i = 0; i < current_state.size(); i++) { // loop for RKF45 term 5
		state = current_state[i] + (439. * stepSize / 216.) * K1[i]
			- (8. * stepSize) * K2[i]
			+ (3680. * stepSize / 513.) * K3[i]
			- (845. * stepSize / 4104.) * K4[i];
		next_state4.push_back(state);
	}
	K5 = particleSystem->evalF(next_state4);

	for (int i = 0; i < current_state.size(); i++) { // loop for RKF45 term 6
		state = current_state[i] - (8. * stepSize / 27.) * K1[i]
			+ (2. * stepSize) * K2[i]
			- (3544. * stepSize / 2565.) * K3[i]
			+ (1859. * stepSize / 4104.) * K4[i]
			- (11. * stepSize / 40.) * K5[i];
		next_state5.push_back(state);
	}
	K6 = particleSystem->evalF(next_state5);

	// evaluating absolute difference in approximate solutions (4th and 5th order RK)
	for (int i = 0; i < current_state.size(); i++) { // final iterative sequence for RK4 numerical integration

		state1 = current_state[i] + (25. * stepSize / 216.) * K1[i]
			+ (1408. * stepSize / 2565.) * K3[i]
			+ (2197. * stepSize / 4101.) * K4[i]
			- (1. * stepSize / 5.) * K5[i];

		state2 = current_state[i] + (16. * stepSize / 135.) * K1[i]
			+ (6656. * stepSize / 12825.) * K3[i]
			+ (28561. * stepSize / 56430.) * K4[i]
			- (9. * stepSize / 50.) * K5[i]
			+ (2. * stepSize / 55.) * K4[i];

		denom += 2. * (state2 - state1).abs(); // computing term in the comparative scalar denominator
	}

	// cout << stepSize * pow(tol_h / denom, 1. / 4.) << endl; // printing step sizes
	
	return stepSize * pow(tol_h / denom, 1./4.);
}


// medium extra credit component
void RKF45::takeStep(ParticleSystem* particleSystem, float stepSize) {
	/*
	Description:
		Running the Runge-Kutta-Fehlberg numerical integration ODE solver.
	Arguments:
		- particleSystem: pointer to ParticleSystem class.
		- stepSize: input step size for numerical integration
	Return:
		-
	*/

	// declaring variables
	vector<Vector3f> c_state, n_state1, n_state2, n_state3, f_state; // declaring necessary variables
	vector<Vector3f> K1, K2, K3, K4;
	float sh; // comparative factor
	Vector3f state;

	sh = optimal_step_size(particleSystem, stepSize, 2e-5); // getting optimal step sizes

	// using new step size to compute the 4 terms for the Runge-Kutta 4th order solver
	c_state = particleSystem->getState(); // initializing states for RK 4th order solver	
	K1 = particleSystem->evalF(c_state); // RK 4th order term 1

	for (int i = 0; i < c_state.size(); i++) { // loop for RK 4th order term 2
		state = c_state[i] + (sh / 2.) * K1[i];
		n_state1.push_back(state);
	}
	K2 = particleSystem->evalF(n_state1);

	for (int i = 0; i < c_state.size(); i++) { // loop for RK 4th order term 3
		state = c_state[i] + (sh / 2.) * K2[i];
		n_state2.push_back(state);
	}
	K3 = particleSystem->evalF(n_state2);

	for (int i = 0; i < c_state.size(); i++) { // loop for RK 4th order term 4
		state = c_state[i] + sh * K3[i];
		n_state3.push_back(state);
	}
	K4 = particleSystem->evalF(n_state3);

	for (int i = 0; i < c_state.size(); i++) { // final iterative sequence for RK4 numerical integration
		state = c_state[i] + (sh / 6.) * (K1[i] + 2. * K2[i] + 2. * K3[i] + K4[i]);
		f_state.push_back(state);
	}

	particleSystem->setState(f_state);

}
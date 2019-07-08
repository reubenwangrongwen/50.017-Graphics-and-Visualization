#include "TimeStepper.hpp"



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
		-
	*/

	// declaring variables
	vector<Vector3f> current_state, next_state1, next_state2, next_state3, final_state, K1, K2, K3, K4;
	Vector3f state;

	// initializing states for RK 4th order solver
	current_state = particleSystem->getState(); // RK 4th order term 1
	
	// computing the 4 terms for the Runge-Kutta solver
	K1 = particleSystem->evalF(current_state);
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
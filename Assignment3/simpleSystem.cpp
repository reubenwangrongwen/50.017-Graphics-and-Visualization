
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem() {

	// declaring variables
	Vector3f state_i; // initial state
	vector<Vector3f> states; // vector of states

	state_i = Vector3f(-1, -1, 0);
	states.push_back(state_i);
	this->setState(states);
}


vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state) {
	/*
	Description:
		Evaluate the function value f(X,t), for a given state X.  
	
	Arguments:
		- state: vector of X vectors.

	Return:
		Vector of evaluated ODE function vectors at the given state. 
	*/
	
	// declaring variables
	Vector3f dXdt; // function derivatve
	vector<Vector3f> f; // vector of function values
	
	// loop over states
	for (int i = 0; i < state.size(); i++) {
		dXdt = Vector3f(-state[i].y(), state[i].x(), 0); // computing 
		f.push_back(dXdt);
	}

	return f;
}

void SimpleSystem::draw() {

	/*
	Description:
		Renders the system (i.e. draw the particles).

	Arguments:
		- 

	Return:
		-
	*/

	// declaring variables
	Vector3f pos; // declare particle position
	vector<Vector3f> current_state = this->getState(); // 

	// loop over multi-particle system
	for (int i = 0; i < current_state.size(); i++) {
		
		pos = current_state[i]; // your particle position
		
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		glutSolidSphere(0.075f, 10.0f, 10.0f);
		glPopMatrix();
	}
}

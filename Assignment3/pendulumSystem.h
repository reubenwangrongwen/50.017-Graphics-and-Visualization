//#ifndef PENDULUMSYSTEM_H
//#define PENDULUMSYSTEM_H
//
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif
//
//#include "particleSystem.h"
//
//class PendulumSystem : public ParticleSystem
//{
//public:
//	PendulumSystem(int num_particles);
//	
//	vector<Vector3f> evalF(vector<Vector3f> state); // EOM evaluation function
//
//	// for rendering the scene
//	virtual void render_toggle() {};
//	virtual void motion_toggle() {};
//	void draw();
//
//};
//
//#endif

#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vector>
#include <math.h> 

#include "particleSystem.h"

class PendulumSystem : public ParticleSystem
{
public:
	PendulumSystem(int numParticles);

	vector<Vector3f> evalF(vector<Vector3f> state);
	
	// drawing the scene
	virtual void render_toggle() {};
	virtual void motion_toggle() {};
	void draw();
private:
	
};

#endif

#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "vecmath.h"
#include <vector>
#include "particleSystem.h"

class TimeStepper
{
public:
	virtual void takeStep(ParticleSystem* particleSystem,float stepSize)=0;
};

//IMPLEMENT YOUR TIMESTEPPERS

class ForwardEuler:public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

class Trapezoidal:public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

class RK4:public TimeStepper
{
	void takeStep(ParticleSystem* particleSystem, float stepSize);
};

/////////////////////////

//Provided
//class RK4:public TimeStepper
//{
  //void takeStep(ParticleSystem* particleSystem, float stepSize);
//};

#endif

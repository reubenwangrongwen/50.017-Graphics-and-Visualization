#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere() : sph_center(Vector3f(0., 0., 0.)), sph_radius(1.0f) {}

	Sphere( Vector3f center , float radius , Material* material ) : Object3D(material),  sph_center(Vector3f(0.,0.,0.)), sph_radius(1.0f) {}
	
	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		return false;
	}

protected:
	Vector3f sph_center; // sphere center coordinate
	float sph_radius; // sphere radius

};


#endif

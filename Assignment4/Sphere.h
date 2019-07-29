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
	Sphere() : center(Vector3f(0., 0., 0.)), radius(1.0f) {}

	Sphere( Vector3f center , float radius , Material* material ) : Object3D(material) {
		this->center = center;
		this->radius = radius;
	}
	
	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		
		// declaring variables
		double a, b, c, discriminant, t;
		Vector3f r_o, r_d, normal;

		// computing vectors
		r_o = r.getOrigin() - this->center;
		r_d = r.getDirection(); r_d.normalize(); // ensure r_d is normalized

		// computing root finding parameters
		a = r_d.absSquared();
		b = 2. * Vector3f::dot(r_d, r_o);
		c = r_o.absSquared() - pow(this->radius, 2.);
		discriminant = pow(b, 2.) - (4. * a * c);

		// checking the discriminant
		if (discriminant >= 0.) { 

			t = (-b + sqrt(discriminant)) / (2. * a); // computing root (+)
			if (t >= tmin && t <= h.getT()) {

				// computing normal
				normal = (r.getOrigin() + t * r_d - this->center);
				normal.normalized();

				h.set(t, this->material, normal); 
				return true;
			}

			t = (-b - sqrt(discriminant)) / (2. * a); // computing root (-)
			if (t >= tmin && t <= h.getT()) {
				
				// computing normal
				normal = (r.getOrigin() + t * r_d - this->center);
				normal.normalized();
				
				h.set(t, this->material, normal);
				return true;
			}
		}
		else {
			return false;
		}
	}

protected:
	Vector3f center; // sphere center coordinate
	float radius; // sphere radius

};


#endif

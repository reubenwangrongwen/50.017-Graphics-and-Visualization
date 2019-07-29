#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m){
		this->_normal = normal.normalized();
		this->_d = d;
	} // constructor
	~Plane(){} // destructor

	virtual bool intersect( const Ray& r , Hit& h , float tmin){

		// declaring variables
		Vector3f r_o;
		Vector3f r_d;
		float n_r_d, n_r_o, t;

		// computing vectors and dot products
		r_o = r.getOrigin();
		r_d = r.getDirection().normalized();
		n_r_d = Vector3f::dot(this->_normal, r_d); // dot product between n and r_d
		n_r_o = Vector3f::dot(this->_normal, r_o); // dot product between n and  r_o

		if (n_r_d == 0.) { // checking for orthogonal rays (grazing rays)
			return false; 
		}

		t = - (n_r_o - this->_d) / (n_r_d); // computing ray parameter
		if (t > tmin && t < h.getT()) {
			h.set(t, this->material, this->_normal);
			return true;
		}
		else {
			return false;
		}
	}

protected:

	Vector3f _normal;
	float _d;

};
#endif //PLANE_H
		


#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
	// Transform(){} 
	Transform( const Matrix4f& m, Object3D* obj ) : o(obj) {
		this->matrix = m;
		this->o = obj;
	} // constructor
	~Transform(){} // destructor

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		
		// declare variables
		Vector3f r_o_trans3, r_d_trans3;
		Vector4f r_o, r_d, r_o_trans4, r_d_trans4;
		
		// computing transformed ray
		r_o = Vector4f(r.getOrigin(), 1.); r_d = Vector4f(r.getDirection(), 0.); // original points
		r_o_trans4 = this->matrix.inverse() * r_o; r_d_trans4 = this->matrix.inverse() * r_d; // transformed points
		r_o_trans3 = r_o_trans4.xyz(); r_d_trans3 = r_d_trans4.xyz(); // transformed vectors
		Ray ray(r_o_trans3, r_d_trans3); // new (transformed) ray
		

		if (this->o->intersect(ray, h, tmin)) {

			// declaring variables
			Vector4f normal_trans4, normal4;
			Vector3f normal_trans3;
			
			normal4 = Vector4f(h.getNormal(), 0.); // original normal as a point
			normal_trans4 = (this->matrix.inverse().transposed() * normal4).normalized(); // transformed normal point
			normal_trans3 = normal_trans4.xyz(); // transformed normal vector
			h.set(h.getT(), h.getMaterial(), normal_trans3); // setting new normal
			
			return true;
		}
		else {
			return false;
		}
		//return o->intersect( r , h , tmin);
	}

 protected:
	Object3D* o; // un-transformed object	
	Matrix4f matrix;
};

#endif //TRANSFORM_H

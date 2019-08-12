#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D {

protected:
	Vector3f a;
	Vector3f b;
	Vector3f c;

public:

	// declaring variables
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];

    //@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		this->a = a;
		this->b = b;
		this->c = c;
        hasTex = false;
	} // constructor

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin) {
		
		// declaring variables
		double alpha, beta, gamma, t, detA;
		Vector3f r_o, r_d;
		Matrix3f A, A_1, A_2, A_3;

		// init ray vectors
		r_o = ray.getOrigin(); // ray origin
		r_d = ray.getDirection(); // ray direction

		// barycentric matrices
		A = Matrix3f(this->a.x() - this->b.x(), this->a.x() - this->c.x(), r_d.x(),
					this->a.y() - this->b.y(), this->a.y() - this->c.y(), r_d.y(),
					this->a.z() - this->b.z(), this->a.z() - this->c.z(), r_d.z());
		A_1 = Matrix3f(this->a.x() - r_o.x(), this->a.x() - this->c.x(), r_d.x(),
						this->a.y() - r_o.y(), this->a.y() - this->c.y(), r_d.y(),
						this->a.z() - r_o.z(), this->a.z() - this->c.z(), r_d.z());
		A_2 = Matrix3f(this->a.x() - this->b.x(), this->a.x() - r_o.x(), r_d.x(),
						this->a.y() - this->b.y(), this->a.y() - r_o.y(), r_d.y(),
						this->a.z() - this->b.z(), this->a.z() - r_o.z(), r_d.z());
		A_3 = Matrix3f(this->a.x() - this->b.x(), this->a.x() - this->c.x(), this->a.x() - r_o.x(),
			this->a.y() - this->b.y(), this->a.y() - this->c.y(), this->a.y() - r_o.y(),
			this->a.z() - this->b.z(), this->a.z() - this->c.z(), this->a.z() - r_o.z());

		// computing barycentric coordinates and ray parameter
		detA = A.determinant(); // determinant of A
		beta = A_1.determinant() / detA;
		gamma = A_2.determinant() / detA;
		alpha = 1. - beta - gamma;
		t = A_3.determinant() / detA;

		// ignoring undefined instances
		if (beta < 0. || gamma < 0. || (beta + gamma) > 1.) { return false; }

		if (t > tmin && t < hit.getT()) {

			// declaring variables
			Vector3f normal;
			Vector2f texture;

			normal = (alpha * this->normals[0] + beta * this->normals[1] + gamma * this->normals[2]).normalized();
			hit.set(t, this->material, normal);
			texture = (alpha * this->texCoords[0] + beta * this->texCoords[1] + gamma * this->texCoords[2]);
			hit.setTexCoord(texture);

			return true;
		}
		return false;
	}

	void setTex(const Vector2f& uva, const Vector2f& uvb, const Vector2f& uvc) {
		texCoords[0] = uva;
		texCoords[1] = uvb;
		texCoords[2] = uvc;
		hasTex = true;
	}
};

#endif //TRIANGLE_H

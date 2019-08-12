#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include <iostream>

using namespace std;


class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ; 
	
	virtual float getTMin() const = 0 ; 
	virtual ~Camera(){}
protected:
	Vector3f center; 
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle) {
		
		this->center = center;
		this->w = direction;
		this->u = Vector3f::cross(w, up);
		this->v = Vector3f::cross(u, w);
		this->_angle = angle;
		
	}

	virtual Ray generateRay( const Vector2f& point) {

		// declaring variables
		Vector3f r; 
		float dist;
		
		// computing ray direction
		dist = 1. / tan(_angle / 2.);
		r = v * point.x() + u * point.y() + dist * w; 
		r.normalize(); 
		
		return Ray(this->center, r);
	}

	virtual float getTMin() const { 
		return 0.0f;
	}

private:

	Vector3f u, v, w;
	float _angle;

};

#endif //CAMERA_H

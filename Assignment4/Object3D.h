#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "Ray.h"
#include "Hit.h"
#include "Material.h"
#include<iostream>

using namespace std;

class Object3D
{
public:
	Object3D() { material = NULL; }
	virtual ~Object3D() {}
	Object3D(Material* material) { this->material = material; }
	virtual bool intersect(const Ray& r, Hit& h, float tmin) = 0;
	char* type;

protected:
	Material* material;
};

#endif


/*class Object3D
{
public:
	Object3D() {
		material = NULL;
	}
	virtual ~Object3D(){}

	Object3D( Material* material) {
		this->material = material ;
	}

	const std::string& get_type() const {
		return type;
	}

	Material* get_material() const {
		return material;
	}

	virtual bool intersect( const Ray& r , Hit& h, float tmin) = 0;


	char* type;
protected:

	string type;
	Material* material;
};*/


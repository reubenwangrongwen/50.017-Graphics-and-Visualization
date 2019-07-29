#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
	
	Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0) : diffuseColor( d_color), specularColor(s_color), shininess(s) {} // constructor
	virtual ~Material() {} // destructor

	virtual Vector3f getDiffuseColor() const { 
		return  diffuseColor;
	}

	Vector3f Shade( const Ray& ray, const Hit& hit, const Vector3f& dirToLight, const Vector3f& lightColor ) {
	
		// declaring variables
		Vector3f N, R; // vectors
		Vector3f col_diff, col_spec; // diffused and specular color
		float L_N, L_R; // dot product variables

		N = hit.getNormal(); N.normalized(); // effective normal vector
		L_N = Vector3f::dot(N, dirToLight); if (L_N < 0.) { L_N = 0.; } // clamped (N dot L)
		
		// checking for texture validity
		if (t.valid()) { 
			Vector3f k_d = t(hit.texCoord[0], hit.texCoord[1]);
			col_diff = L_N * lightColor * k_d;
			diffuseColor = k_d;
		}
		else {
			col_diff = L_N * diffuseColor * lightColor;
		}
		
		R = - dirToLight + (2. * L_N * N); R.normalized(); // direction vector
		L_R = Vector3f::dot(dirToLight, R); if (L_R < 0.) { L_R = 0.; } // clamped (v dot r)
	
		col_spec = pow(L_R, shininess) * lightColor * specularColor;
		return col_diff + col_spec;
		}


		void loadTexture(const char * filename){
			t.load(filename);
		}


 protected:

	Vector3f diffuseColor;
	Vector3f specularColor;
	float shininess;
	Texture t;
};



#endif // MATERIAL_H

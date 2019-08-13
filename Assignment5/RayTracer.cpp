#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.01

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming) {

	Vector3f dir; // declaring varible for unnormalized direction
	dir = incoming - 2 * (Vector3f::dot(incoming, normal)) * (normal); // computing unnormalized direction

	return dir.normalized();
}

bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, float index_n, float index_nt, Vector3f& transmitted) {
	
	// declaring variables
	float refrac_ratio, n_I, sqrt_arg;

	// computing necessary quantities
	refrac_ratio = index_n / index_nt; // index_n, index_nt: refractive indices of the current object and the object the ray is going into
	n_I = Vector3f::dot(normal, incoming);
	sqrt_arg = 1. - pow(refrac_ratio, 2) * (1. - pow(n_I, 2));
	
	// checking for pure reflection
	if (sqrt_arg < 0) { 
		return false; 
	}
	
	// computing direction of refraction
	transmitted = refrac_ratio * (incoming - normal * n_I) - normal * sqrt(sqrt_arg); 
	transmitted.normalize();

	return true;
	
}

//more arguments if you need...
RayTracer::RayTracer( SceneParser* scene, int max_bounces, bool shadow_tog) : m_scene(scene) {
  g = scene->getGroup();
  m_maxBounces = max_bounces;
  shadow_toggle = shadow_tog;

}

RayTracer::~RayTracer() {}

Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces, float refr_index, Hit& hit ) const {
    
	// declaring variable
	float R = 1.f;
	Vector3f color; 

	// checking if ray intersects group
	if (g->intersect(ray, hit, tmin)) {
		
		// ambient lighting 
		color = (m_scene->getAmbientLight()) * (hit.getMaterial()->getDiffuseColor());

		// loop over number of lights
		for (int i = 0; i < m_scene->getNumLights(); i++) {

			// declaring variables for shadow effect
			Light* light;
			Vector3f dir2light, light_col, hit_pt;
			float distanceToLight;

			light = m_scene->getLight(i);
			light->getIllumination(ray.pointAtParameter(hit.getT()), dir2light, light_col, distanceToLight);
			hit_pt = ray.pointAtParameter(hit.getT());

			// for shadow rendering
			if (shadow_toggle) {

				// declaring variables
				Hit shadowHit;
				bool groupIntersect;

				// init variables
				Ray shadowRay(hit_pt, dir2light);
				shadowHit = Hit(distanceToLight, NULL, NULL);
				groupIntersect = g->intersect(shadowRay, shadowHit, EPSILON);

				// for diffused lighting (no intersection => no shadow)
				if (shadowHit.getT() >= distanceToLight) {
					color += hit.getMaterial()->Shade(ray, hit, dir2light, light_col);
				}
			}
			else {
				// only diffuse lighting 
				color += hit.getMaterial()->Shade(ray, hit, dir2light, light_col);
			}
		}

		// declare color variables
		Vector3f reflection_color;
		Vector3f refraction_color;

		// ray tracing recursion
		if (bounces > m_maxBounces - 1) { return color; } // check for over-dense recursion
		if (bounces > 0) { 

			// ------------------------------- computing reflection ------------------------------- 
			// init variables
			Vector3f reflection_dir = mirrorDirection(hit.getNormal().normalized(), ray.getDirection().normalized());
			Vector3f intersection_pt = ray.pointAtParameter(hit.getT());
			Ray reflect_ray = Ray(intersection_pt, reflection_dir); // reflection_ray originates from the pt of intersection 
			Hit reflectionHit = Hit(FLT_MAX, NULL, Vector3f::ZERO);
			
			reflection_color = traceRay(reflect_ray, EPSILON, refr_index, bounces - 1, reflectionHit) * hit.getMaterial()->getSpecularColor();
			// ------------------------------------------------------------------------------------

			// ------------------------------- computing refraction -------------------------------
			// init variables
			Vector3f refraction_dir;
			Vector3f normal = hit.getNormal().normalized();
			float index_nt = hit.getMaterial()->getRefractionIndex(); 
			float R_0;
			if (Vector3f::dot(ray.getDirection(), normal) > 0) {
				normal *= -1.f; // change normal to be positive if the ray is currently in the object
				index_nt = 1.f;
			}
			
			bool refraction = transmittedDirection(normal, ray.getDirection(), refr_index, index_nt, refraction_dir); // check for pure reflection
			if (refraction) {
				
				// init variables
				float c;
				Ray refraction_ray = Ray(intersection_pt, refraction_dir);
				Hit refraction_hit = Hit();

				refraction_color = traceRay(refraction_ray, EPSILON, index_nt, bounces - 1, refraction_hit) * hit.getMaterial()->getSpecularColor();

				// Fresnel’s equation (blending reflection and refraction)
				if (refr_index <= index_nt) { c = abs(Vector3f::dot(ray.getDirection(), normal)); }
				else { c = abs(Vector3f::dot(refraction_dir, normal)); }
				R_0 = pow(((index_nt - refr_index) / (index_nt + refr_index)), 2);
				R = R_0 + (1 - R_0) * pow((1 - c), 5);
			}
			// ------------------------------------------------------------------------------------
		}
		//weighting of reflection and refraction
		color += R * reflection_color + (1 - R) * refraction_color;
	}
	else {
		color = m_scene->getBackgroundColor(ray.getDirection());
	}

	return color;
}

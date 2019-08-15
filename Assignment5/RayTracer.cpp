#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.01

//IMPLEMENT THESE FUNCTIONS
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming) {
	/*
	Description:
		Computes the direction of the reflected ray using Lambertian (cosine) technique.
	Arguments:
		- normal: normal vector of surface.
		- incoming: direction of incoming ray. 
	Return:
		direction of reflected ray. 
	*/

	Vector3f dir; // declaring varible for unnormalized direction
	dir = incoming - 2 * (Vector3f::dot(incoming, normal)) * (normal); // computing unnormalized direction

	return dir.normalized();
}

bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, float index_n, float index_nt, Vector3f& transmitted) {
	/*
	Description:
		Computes the direction of the refracted ray.
	Arguments:
		- normal: normal vector of surface.
		- incoming: direction of incoming ray.
		- index_n: refractive index of incoming medium.
		- index_nt: refractive index of outgoing medium. 
		- transmitted: direction of ray.
	Return:
		direction of refracted ray.
	*/

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
	/*
	Description:
		Performs ray tracing based on specified number of ray bounces.
	Arguments:
		- ray: Ray class instance.
		- tmin: span parameter value for intersection checking. 
		- bounces: number of ray tracing bounces.
		- refr_index: refractive index of material.
		- hit: Hit class instance.
	Return:
		effective color of the pixel after ray tracing.
	*/

	hit = Hit(FLT_MAX, NULL, Vector3f::ZERO);

	if (m_scene->getGroup()->intersect(ray, hit, m_scene->getCamera()->getTMin())) {
		
		// declare variables
		Light* light;
		Vector3f light_dir;
		Vector3f light_col;
		Vector3f pix_col;
		Vector3f intersect;
		float dist2light;

		// init vectors
		pix_col = Vector3f::ZERO;
		intersect = ray.getOrigin() + ray.getDirection() * hit.getT();

		// for loop to get diffuse and specular colors
		for (int idx = 0; idx < m_scene->getNumLights(); idx++) {
			
			// setting light objects
			light = m_scene->getLight(idx);
			light->getIllumination(ray.pointAtParameter(hit.getT()), light_dir, light_col, dist2light);

			// getting shadows
			if (shadow_toggle) {
				Ray ray_shadow(intersect + light_dir * EPSILON, light_dir);
				Hit hit_shadow(dist2light, NULL, NULL);

				// checking for ray intersection
				if (!m_scene->getGroup()->intersect(ray_shadow, hit_shadow, tmin)) {
					Vector3f shading_col = hit.getMaterial()->Shade(ray, hit, light_dir, light_col);
					pix_col += shading_col;
				}
			}

		}
		pix_col += hit.getMaterial()->getDiffuseColor() * m_scene->getAmbientLight(); // adding ambient color

		if (bounces > 0) { // checking if there are ray reflections/refractions

			// -------------------------- reflection --------------------------
			// declare ray items
			Vector3f reflect_dir;
			Hit hit_refl;
			Vector3f reflect_col;

			// init ray items
			reflect_dir = mirrorDirection(hit.getNormal().normalized(), ray.getDirection());
			Ray ray_refl = Ray(intersect + reflect_dir * EPSILON, reflect_dir);
			hit_refl = Hit(FLT_MAX, NULL, Vector3f::ZERO);
			reflect_col = traceRay(ray_refl, 0, bounces - 1, refr_index, hit_refl);
			// ----------------------------------------------------------------

			// -------------------------- refraction --------------------------
			// init ray items
			float refr_index_new = hit.getMaterial()->getRefractionIndex();
			Vector3f normal = (hit.getNormal()).normalized();
			if (Vector3f::dot(ray.getDirection(), normal) > 0.) { // checking if normal needs to be negated
				refr_index_new = 1.f; // new refractive index
				normal = -normal; // negating normal
			}
			Vector3f refract_dir(0., 0., 0.); // init refraction direction (updated below)

			// init boolean variable to check for refraction
			bool refract_on = transmittedDirection(normal, ray.getDirection(), refr_index, refr_index_new, refract_dir);
			if (refract_on) {

				// declare ray items
				Hit hit_refr;
				Vector3f refractColor;

				// init ray items
				Ray ray_refr = Ray(intersect + refract_dir * EPSILON, refract_dir);
				hit_refr = Hit(FLT_MAX, NULL, Vector3f::ZERO);
				refractColor = traceRay(ray_refr, 0, bounces - 1, refr_index_new, hit_refr);

				// Schlick's approximation
				float c, R_0, R; 
				if (refr_index <= refr_index_new) { c = abs(Vector3f::dot(ray.getDirection(), normal)); } 
				else { c = abs(Vector3f::dot(refract_dir, normal)); }
				R_0 = pow(((refr_index_new - refr_index) / (refr_index_new + refr_index)), 2); 
				R = R_0 + (1. - R_0) * pow(1. - c, 5); 
				pix_col += (1. - R) * hit.getMaterial()->getSpecularColor() * refractColor + R* reflect_col * hit.getMaterial()->getSpecularColor();
			}
			else {
				pix_col += reflect_col * hit.getMaterial()->getSpecularColor(); // only reflection
			}
			// ----------------------------------------------------------------
		}

		return pix_col;
	}
	else return m_scene->getBackgroundColor(ray.getDirection());
}

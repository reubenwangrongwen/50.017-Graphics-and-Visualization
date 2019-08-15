#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>
#include "RayTracer.h"

using namespace std;


#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
	// Report help usage if no args specified.
	if (argc == 1) {
		cout << "Usage: a4 "
			<< "-input <scene> -size <width> <height> -output <image.png> -depth <depth_min> <depth_max> <depth_image.png> [-normals <normals_image.png>]\n";
		return 1;
	}

	// declaring variables for argument parsing
	char* scene_filename;
	char* output_filename;
	char* depth_filename;
	char* normal_filename;
	int width, height;
	float depth_min, depth_max;
	bool depth_toggle, normal_toggle; // for depth and normal vis
	bool jitter, filter;
	int max_bounces;
	bool shadow_toggle;

	// init parameters
	width = 0; height = 0;
	depth_min = 0.; depth_max = 0.;
	depth_toggle = false; normal_toggle = false;
	jitter = false; 
	max_bounces = 0;
	shadow_toggle = false;

	// This loop loops over each of the input arguments.
	for (int argNum = 1; argNum < argc; ++argNum) {

		/*argNum is initialized to 1 because the first "argument"
		provided to the program is actually the name of the
		executable (in our case, "a4").*/
		
		cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;

		if (strcmp(argv[argNum], "-input") == 0) {
			scene_filename = argv[argNum + 1];
		}
		if (strcmp(argv[argNum], "-size") == 0) {
			width = atoi(argv[argNum + 1]);
			height = atoi(argv[argNum + 2]);
		}
		if (strcmp(argv[argNum], "-output") == 0) {
			output_filename = argv[argNum + 1];
		}
		if (strcmp(argv[argNum], "-depth") == 0) {
			depth_toggle = true;
			depth_min = atof(argv[argNum + 1]);
			depth_max = atof(argv[argNum + 2]);
			depth_filename = argv[argNum + 3];
		}
		if (strcmp(argv[argNum], "-normal") == 0) {
			normal_toggle = true;
			normal_filename = argv[argNum + 1];
		}
		if (strcmp(argv[argNum], "-shadows") == 0) {
			shadow_toggle = true;
		}
		if (strcmp(argv[argNum], "-bounces") == 0) {
			max_bounces = atoi(argv[argNum + 1]);
		}
		if (strcmp(argv[argNum], "-jitter") == 0) {
			jitter = true;
		}
	}
	
	// declaring variables for scene rendering
	Light* light;
	Hit hit;
	Vector2f coordinate;
	Vector3f pix_col;
	Vector3f light_dir, light_col, shading_col;
	float light_dist;

	// init classes
	SceneParser scene(scene_filename); // First, parse the scene using SceneParser.
	Image img(width, height); // init image
	Image img_depth(width, height); // init depth image 
	Image img_normals(width, height); // init normal image 
	RayTracer ray_tracer = RayTracer::RayTracer(&scene, max_bounces, shadow_toggle);

	img.SetAllPixels( scene.getBackgroundColor(Vector3f::ZERO) ); // init scene pixels
	img_depth.SetAllPixels( Vector3f::ZERO ); // init depth scene pixels
	img_normals.SetAllPixels( Vector3f::ZERO ); // init normal scene pixels

	// init for supersampling (jitter and Gaussian blurring)
	int ss_width = width * 3, ss_height = height * 3; // supersampling heights and widths
	Image ss_img = Image(width * 3, height * 3);
	ss_img.SetAllPixels(scene.getBackgroundColor(Vector3f::ZERO));

	if (jitter) {
		width = ss_width;
		height = ss_height;
	}

	// loops over scene view width and height
	for (unsigned i = 0; i < width; i++) {
		for (unsigned j = 0; j < height; j++) {

			if (jitter) { // checking jitter toggle
				float jit = -0.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.5 - (-0.5)))); // jitter perturbation
				float ii = i + jit, jj = j + jit; // updating ray casting points with jitter
				coordinate = Vector2f(2. * float(ii) / (float(width) - 1.) - 1.,
					2. * float(jj) / (float(height) - 1.) - 1.); // mapping coordinates to scene pixel-grid
			}
			else {
				coordinate = Vector2f(2. * float(i) / (float(width) - 1.) - 1.,
					2. * float(j) / (float(height) - 1.) - 1.); // mapping coordinates to scene pixel-grid
			}
			
			hit = Hit(FLT_MAX, NULL, Vector3f::ZERO); // init hit variable
			Ray ray = scene.getCamera()->generateRay(coordinate); // init ray for ray casting

			// ------------------------- performing ray tracing -------------------------
			pix_col = ray_tracer.traceRay(ray, scene.getCamera()->getTMin(), max_bounces, 1.f, hit);
			if (jitter) { ss_img.SetPixel(j, i, pix_col); } // setting jitter pixels to color 
			else { img.SetPixel(j, i, pix_col); } // setting unjittered pixels to color 
			// --------------------------------------------------------------------------

			if (scene.getGroup()->intersect(ray, hit, scene.getCamera()->getTMin())) {

				// ------------------------- getting depth image -------------------------
				if (depth_toggle) {

					if (hit.getT() < depth_min) {
						img_depth.SetPixel(j, i, Vector3f(1., 1., 1.));
					}
					else if (hit.getT() > depth_max) {
						img_depth.SetPixel(j, i, Vector3f::ZERO);
					}
					else {
						float depths = (depth_max - hit.getT()) / (depth_max - depth_min);
						img_depth.SetPixel(j, i, depths * Vector3f(1., 1., 1.));
					}
				}
				// -----------------------------------------------------------------------

				// ------------------------- getting normal image -------------------------
				if (normal_toggle) {

					// declaring color variable
					Vector3f col_norm;

					// getting coloring as normal vector
					col_norm = hit.getNormal();
					col_norm.normalized();

					// ensuring positive definite entries
					for (int k = 0; k < 3; k++) {
						col_norm[k] = pow(-1., (col_norm[k] < 0.)) * col_norm[k];
					}
					img_normals.SetPixel(j, i, col_norm);
				}
				// ------------------------------------------------------------------------
			}
		}
	}

	if (jitter) {
		// ------------------------------------ Gaussian blurring ------------------------------------
		// declare variables
		int n;
		Vector3f pixel;

		// Gaussian convolutional kernel values
		float kernel[5] = { 0.1201, 0.2339, 0.2931, 0.2339, 0.1201 }; 
		
		// horizontal Gaussian blurring
		Image blur_img1 = Image(ss_width, ss_height);
		blur_img1.SetAllPixels(scene.getBackgroundColor(Vector3f::ZERO));
		
		for (int i = 0; i < ss_width; i++) {
			for (int j = 0; j < ss_height; j++) {

				pixel = Vector3f::ZERO; // reinit pixel value
				// loop over kernel values
				for (int k = 0; k < 5; k++) {
					int n = i - 2 + k;
					if (n < 0) { n = 0; }
					if (n >= ss_width) { n = ss_width - 1; }
					pixel += kernel[k] * ss_img.GetPixel(j, n);
				}
				blur_img1.SetPixel(j, i, pixel);
			}
		}
		// vertical Gaussian blurring
		Image blur_img2 = Image(ss_width, ss_height);
		blur_img2.SetAllPixels(scene.getBackgroundColor(Vector3f::ZERO));
		for (int i = 0; i < ss_width; i++) {
			for (int j = 0; j < ss_height; j++) {
				
				pixel = Vector3f::ZERO; // reinit pixel value
				// loop over kernel values
				for (int k = 0; k < 5; k++) {
					int n = j - 2 + k;
					if (n < 0) { n = 0; }
					if (n >= ss_height) { n = ss_height - 1; }
					pixel += kernel[k] * blur_img1.GetPixel(n, i);
				}
				blur_img2.SetPixel(j, i, pixel);
			}
		}
		// -------------------------------------------------------------------------------------------

		// ------------------------------------ downsampling ------------------------------------
		for (int i = 0; i < int(width / 3); i++) {
			for (int j = 0; j < int(height / 3); j++) {
				pixel = blur_img2.GetPixel(3 * j + 0, 3 * i + 0) + blur_img2.GetPixel(3 * j + 0, 3 * i + 1) + blur_img2.GetPixel(3 * j + 0, 3 * i + 2) +
						blur_img2.GetPixel(3 * j + 1, 3 * i + 0) + blur_img2.GetPixel(3 * j + 1, 3 * i + 1) + blur_img2.GetPixel(3 * j + 1, 3 * i + 2) +
						blur_img2.GetPixel(3 * j + 2, 3 * i + 0) + blur_img2.GetPixel(3 * j + 2, 3 * i + 1) + blur_img2.GetPixel(3 * j + 2, 3 * i + 2);
				pixel = pixel / 9.f;
				img.SetPixel(j, i, pixel);
			}
		}
		// -------------------------------------------------------------------------------------
	}


	img.SaveBMP(output_filename);
	if (depth_toggle) { img_depth.SaveBMP(depth_filename); }
	if (normal_toggle) { img_normals.SaveBMP(normal_filename); }

	
	return 0;
}

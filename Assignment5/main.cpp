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

	// init parameters
	width = 0; height = 0;
	depth_min = 0.; depth_max = 0.;
	depth_toggle = false; normal_toggle = false;

	// This loop loops over each of the input arguments.
	for( int argNum = 1; argNum < argc; ++argNum ) { 
		/*argNum is initialized to 1 because the first "argument" 
		provided to the program is actually the name of the 
		executable (in our case, "a4").*/
		
		cout << "Argument " << argNum << " is: " << argv[argNum] << endl;

		if (strcmp(argv[argNum], "-input") == 0) {
			scene_filename = argv[argNum + 1];
		}
		else if (strcmp(argv[argNum], "-size") == 0) {
			width = atoi(argv[argNum + 1]);
			height = atoi(argv[argNum + 2]);
		}
		else if (strcmp(argv[argNum], "-output") == 0) {
			output_filename = argv[argNum + 1];
		}
		else if (strcmp(argv[argNum], "-depth") == 0) {
			depth_toggle = true;
			depth_min= atof(argv[argNum + 1]);
			depth_max = atof(argv[argNum + 2]);
			depth_filename = argv[argNum + 3];
		}
		else if (strcmp(argv[argNum], "-normal") == 0) {
			normal_toggle = true;
			normal_filename = argv[argNum + 1];
		}
	}
	
	// declaring variables for scene rendering
	Light* light;
	Hit hit;
	Vector2f coordinate;
	Vector3f pix_col;
	Vector3f light_dir, light_col, shading_col;
	float light_dist;

	// init variables
	SceneParser scene(scene_filename); // First, parse the scene using SceneParser.
	Image img(width, height); // init image
	Image img_depth(width, height); // init depth image 
	Image img_normals(width, height); // init normal image 
	
	
	/* Then loop over each pixel in the image, shooting a ray
	 through that pixel and finding its intersection with
	 the scene.  Write the color at the intersection to that
	 pixel in your output image.*/

	img.SetAllPixels( scene.getBackgroundColor(Vector3f(0.0f, 0.0f, 0.0f)) ); // init scene pixels
	img_depth.SetAllPixels( Vector3f::ZERO ); // init depth scene pixels
	img_normals.SetAllPixels( Vector3f::ZERO ); // init normal scene pixels

	// loops over scene view width and height
	for (unsigned i = 0; i < width; i++) {
		for (unsigned j = 0; j < height; j++) {

			coordinate = Vector2f(2. * float(i) / (float(width) - 1.) - 1., 
								2. * float(j) / (float(height) - 1.) - 1.); // mapping coordinates to scene pixel-grid
			hit = Hit(FLT_MAX, NULL, Vector3f::ZERO); // init hit variable
			Ray ray = scene.getCamera()->generateRay(coordinate); // init ray for ray casting

			if (scene.getGroup()->intersect(ray, hit, scene.getCamera()->getTMin())) {

				pix_col = Vector3f::ZERO; // re-init to zero

				// loop over lights for diffused lighting
				for (unsigned i = 0; i < scene.getNumLights(); i++) {

					light = scene.getLight(i); // light specification
					light->getIllumination(ray.pointAtParameter(hit.getT()), light_dir, light_col, light_dist);

					shading_col = hit.getMaterial()->Shade(ray, hit, light_dir, light_col); // material specification
					pix_col = pix_col + shading_col; // adding shading color for next iteration
				}

				pix_col = pix_col + hit.getMaterial()->getDiffuseColor() * scene.getAmbientLight(); // copmuting ambient color
				img.SetPixel(j, i, pix_col); // setting pixels to color


				// getting depth image 
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

				// getting normal image 
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
			}
		}
	}
	img.SaveBMP(output_filename);
	if (depth_toggle) { img_depth.SaveBMP(depth_filename); }
	if (normal_toggle) { img_normals.SaveBMP(normal_filename); }

	
	return 0;
}

#include "Headers/surf.h"
#include "Headers/extra.h"
using namespace std;

float PI = 3.141592653589793; // vlaue of pi

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

// creating surface through revolution
Surface makeSurfRev(const Curve &profile, unsigned steps)
{
	/*
	Description:
		Function creates sweep surfaces via a surface of revolution. 

	Variables:
		profile: curve profile to be swept.
		steps: the number of iterations that constitute a sweep.

	Output:
		returns a surface.
	*/

    Surface surface; // struct instance that contains vertices, normals, and faces.
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

	double theta = 2 * PI / steps; // angular steps over which surface is revolved
	Matrix3f M_T = Matrix3f(cos(theta), 0., -sin(theta),
						  0.,		  1., 0.,
						  sin(theta), 0., cos(theta)); // transposed rotation matrix about y-axis
	Vector3f rot_vertex, rot_normal; // declaring vector variables (vertex and normal vectors)
	Curve curve = profile; // initialize the curve over which to swept
	Curve new_curve; // initialize curves for (i+1)_th iteration
	CurvePoint curve_point, rot_point; // declaring struct variables for iteration
	unsigned num_points = curve.size(); // number of points of input curve
	
	// for loop over input curves
	for (unsigned s = 0; s < steps; s++) {

		// for loop over points on curve
		for (unsigned i = 0; i < num_points; i++) {
			// iteratively adding vertices and normals to surface
			curve_point = curve[i]; // current point on current curve
			surface.VV.push_back(curve_point.V); // appending vertex to surface
			surface.VN.push_back(-curve_point.N); // appending normal to surface
			
			// iteraitvely sweeping curve
			rot_vertex = M_T * curve_point.V; // rotating vertices
			rot_normal = M_T * curve_point.N; // rotating normals
			rot_normal.normalize(); // normalizing normal
			
			rot_point = { rot_vertex, curve_point.T, rot_normal, curve_point.B };
			new_curve.push_back(rot_point); // appending to new rotated vertices
		}

		curve = new_curve; // reinitializing curve for next iteration
		new_curve.clear(); // clearing new_curve for next iteration 
		num_points = curve.size(); // number of points of input curve
	}

	// accounting for curve closure (topological circle)
	unsigned nxt_ind; // declaring index variable
	for (unsigned s = 0; s < steps; s++) {

		if (s == steps - 1) { nxt_ind = 0; }
		else { nxt_ind = s + 1; }

		for (unsigned i = 0; i < num_points - 1; i++) {
			// ensuring face indices are consistent
			surface.VF.push_back(Tup3u(s * num_points + i, nxt_ind * num_points + i, nxt_ind * num_points + i + 1));
			surface.VF.push_back(Tup3u(s * num_points + i, nxt_ind * num_points + i + 1, s * num_points + i + 1));
		}
	}

    cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface; // returning revolved surface
}


// function for triangle meshing (only appends faces)
void triangle_mesh (Surface& surface, unsigned num_points, int k) {
	
	/*
	Description:
		Function that appends faces of the triangle mesh to the surface struct.

	Variables:
		surface: surface struct
		num_points: number of points on the curve
		j: the iteration index

	Output:
		returns a surface.
	*/
	
	for (unsigned i = 0; i < num_points - 1; i++) {
		surface.VF.push_back(Tup3u(k * num_points + i, k * num_points + i + 1, k * num_points + num_points + i));
		surface.VF.push_back(Tup3u(k * num_points + i + 1, k * num_points + num_points + i + 1, k * num_points + num_points + i));
	}
	
	return;
}

// function that generates generalized cylinders
Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
	/*
	Description:
		Function creates sweep surfaces via a generalized cylinder.

	Variables:
		profile: curve profile to be swept.
		steps: the number of iterations that constitute a sweep.

	Output:
		returns a surface.
	*/

    Surface surface; // struct instance

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

	Curve curve = profile; // profile curve 
	Curve sweep_curve = sweep; // sweep curve
	Curve new_curve;
	CurvePoint point, new_point; // declaring points along curve

	Matrix4f transform, transform_inv_T; // declaring transformation and its inverse transpose
	Vector4f vert, norm; // vectors (4f) for iteration
	Vector3f new_vert, new_norm; // vectors (3f) for iteration
	vector<Curve> curve_array; // vector of curves

	// loop over sweep curve steps
	for (unsigned s = 0; s < sweep.size(); s++) {
		point = sweep_curve[s];

		// computing the transformation
		transform = Matrix4f(Vector4f(point.N, 0),
							 Vector4f(point.B, 0),
							 Vector4f(point.T, 0),
							 Vector4f(point.V, 1));
		
		// computing inverse transpose of the tranformation
		transform_inv_T = transform.inverse();
		transform_inv_T.transpose();

		new_curve.clear(); // clearing memory

		// loop over trajectory curve
		for (unsigned j = 0; j < curve.size(); j++) {

			vert = transform * Vector4f(curve[j].V, 1);
			norm = transform_inv_T * Vector4f(curve[j].N, 1);

			new_vert = Vector3f(vert[0], vert[1], vert[2]);
			new_norm = Vector3f(-norm[0], -norm[1], -norm[2]);

			new_point = { new_vert, curve[j].T,new_norm, curve[j].B };
			new_curve.push_back(new_point);
		}

		curve_array.push_back(new_curve); // appending new curve
	}

	// loop that updates the mesh
	for (unsigned k = 0; k < curve_array.size() - 1; k++) {

		//
		for (unsigned i = 0; i < curve_array[k].size(); i++) {
			surface.VV.push_back((curve_array[k])[i].V);
			surface.VN.push_back((curve_array[k])[i].N);
		}
		triangle_mesh(surface, curve_array[k].size(), k);
	}

	// appending last 2 iterations
	for (unsigned i = 0; i < (curve_array[curve_array.size() - 1]).size(); i++) {
		surface.VV.push_back((curve_array[curve_array.size() - 1])[i].V);
		surface.VN.push_back((curve_array[curve_array.size() - 1])[i].N);
	}
	for (unsigned i = 0; i < curve_array[0].size(); i++) {
		surface.VV.push_back((curve_array[0])[i].V);
		surface.VN.push_back((curve_array[0])[i].N);
	}
	triangle_mesh(surface, curve_array[curve_array.size() - 1].size(), curve_array.size() - 1);

    cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}



void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}

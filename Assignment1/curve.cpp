#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}


// monomial basis (1, t, t^2, t^3)
Vector4f mono_basis (float t) {
	return Vector4f (1., t, pow(t, 2), pow(t, 3));
}

// derivative of monomial basis w.r.t parameter t
Vector4f mono_basis_prime (float t) {
	return Vector4f (0., 1., 2. * t, 3. * pow(t, 2));
}

// computing the coordinates for given a basis
Vector3f new_coordinates (Vector4f basis, Vector4f x, Vector4f y, Vector4f z) {
	return Vector3f (Vector4f::dot(basis, x), Vector4f::dot(basis, y), Vector4f::dot(basis, z));
}

// function that computes the curve given a specific spline basis
Curve get_curve (const vector< Vector3f >& P, unsigned steps, Curve curve, Matrix4f spline_basis) {

	/*
	Description:
		Function computes all the appropriate Vector3fs for each CurvePoint ( V, T, N, B ) for a given basis.
		(Assume all curves received have G1 continuity. TNB will not be be defined otherwise.)
		** This iterative method of getting the Frenet-Serret vectors may lead to misalignmnet in closed curves **

	Variables:
		P: vector of points.
		steps:  the number of points to generate on each piece of the spline.
		curve: a Curve (struct) instance that defines the spline
		spline_basis: a 4x4 matrix that defines the spline basis (e.g. Bezier basis)

	Output:
		returns a Curve (e.g., a vector< CurvePoint >).
	*/

	float dt = 1. / steps; // differential parameter steps
	Vector4f x, y, z, basis, basis_prime; // points
	Vector3f V, B, N, T; // vectors
	CurvePoint point; // declaring curve points 

	// loop over control point "segments"
	for (unsigned i = 3; i < P.size(); i++) {

		// computing vertex entries for current iteration
		x = Vector4f(P[i - 3][0], P[i - 2][0], P[i - 1][0], P[i][0]);
		y = Vector4f(P[i - 3][1], P[i - 2][1], P[i - 1][1], P[i][1]);
		z = Vector4f(P[i - 3][2], P[i - 2][2], P[i - 1][2], P[i][2]);

		// looking at last point of current segment and first point of next segment
		if (i == 3) {
			B = Vector3f(0., 0., 1.); // initi binormal vector
			T = new_coordinates(spline_basis * mono_basis_prime(0), x, y, z); // init tangent vector

			// checking for T != B and initilizing another binormal
			if (1. - Vector3f::dot(B, T) < 1e-8f) { // 1e-8 is just arbitrary numerical tolerance value
				B = Vector3f(0., 1., 0.); 
			}
		}

		// loop over discretized parameter values
		for (unsigned t_i = 0; t_i <= steps; t_i++) {
			basis = spline_basis * mono_basis(t_i * dt); // Bezier basis
			basis_prime = spline_basis * mono_basis_prime(t_i * dt); // Bezier differentiated basis

			// computing Frenet-Serret vectors
			V = new_coordinates(basis, x, y, z);
			T = new_coordinates(basis_prime, x, y, z);
			N = Vector3f::cross(B, T);
			B = Vector3f::cross(T, N);

			// normalizing vectors
			T.normalize(); 
			N.normalize(); 
			B.normalize();

			// computing Frenet-Serret vectors 
			point = { V, T, N, B };
			curve.push_back(point); // appending vectors to struct
		}
	}

	return curve; // returns curve struct
}
    
// function that generates points of Bezier spline given input control points
Curve evalBezier( const vector< Vector3f >& P, unsigned steps ) {

	/* 
	Description:
		Function computes all the appropriate Vector3fs for each CurvePoint (V,T,N,B) for the Bezier basis.
		(Assume all Bezier curves received have G1 continuity. TNB will not be be defined otherwise.)
		
	Variables:
		P: vector of points.
		steps:  the number of points to generate on each piece of the spline.

	Output:
		returns a Curve (e.g., a vector< CurvePoint >).
	*/

    // error checks for vector size (homogeneous coordinate)
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

	// printing input points
    cerr << "\t>>> evalBezier has been called with the following input:" << endl;
    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i ){ cerr << "\t>>> " << P[i] << endl; }
    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;

	Curve curve; // declaring curve struct

	// Bezier basis
	Matrix4f Bezier_basis = Matrix4f(1., -3., 3., -1.,
		0., 3., -6., 3.,
		0., 0., 3., -3.,
		0., 0., 0., 1.);

	// returns curve struct
	return get_curve( P, steps, curve, Bezier_basis); 
}

// function that check the direction of rotation (easy extra credit)
float get_parity (Curve curve, int num_pts) {

	/*
	Description:
		Function checks the necessary direction for rotation based on the anticlockwise convention.
		(Does so by rotating as per convention then checking if misalignment is still present.)

	Variables:
		curve: vector of curvepoint structs
		num_pts: the number of curve points.

	Output:
		returns +1 if rotation required is anticlockwise, -1 otherwise.
	*/

	float theta; // angular error in radians
	Vector4f quaternion; // vector of quaternion entries 
	Matrix3f R; // 3D rotation matrix

	// computing rotation matrix necessary for correction
	theta = acos(Vector3f::dot(curve[0].N, curve[num_pts - 1].N)); // computing angular error
	quaternion = Vector4f(cos(theta / 2),
		sin(theta / 2) * (curve[num_pts - 1].T)[0],
		sin(theta / 2) * (curve[num_pts - 1].T)[1],
		sin(theta / 2) * (curve[num_pts - 1].T)[2]); // computing quaternion
	quaternion.normalize(); // normalizing
	R = Matrix3f::rotation(quaternion); // rotation matrix

	// rotating n-th Frenet-Serret frame
	curve[num_pts - 1].N = R * curve[num_pts - 1].N; // rotating last frame normal

	// checking alignment 
	if (1. - Vector3f::dot(curve[0].N, curve[num_pts - 1].N) > 1e-6f) {
		return -1.;
	}
	else {
		return 1.; 
	}
}

// function that generates points of B-spline curve given input control points
Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
	/*
	Description:
		Function computes all the appropriate Vector3fs for each CurvePoint for a B-spline (V,T,N,B).

	Variables:
		P: vector of control points.
		steps: the number of points to generate on each piece of the spline.

	Output:
		returns a Curve (e.g., a vector< CurvePoint >).
	*/

	// Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;

	Curve curve;
	Vector4f quaternion; // vector of quaternion entries 
	float theta, dtheta; // for closed-curve angular interpolation error
	int num_pts; // declaring variable to store number of curve points

	// defining the B-spline basis as a matrix
	Matrix4f Bspline_basis;
	Matrix3f R_e; // rotation matrix to interpolate angular error 
	Bspline_basis = Matrix4f(1./6., -0.5, 0.5, -1. / 6.,
		2./3., 0, -1., 0.5,
		1. / 6., 0.5, 0.5, -0.5,
		0, 0, 0, 1. / 6.);

	curve = get_curve(P, steps, curve, Bspline_basis); // final unchecked curve
	num_pts = curve.size(); // number of curve points

	// checking if start and end normals of closed-curve align (easy extra credit)
	if (1. - Vector3f::dot(curve[0].N, curve[num_pts-1].N) > 1e-6f) { // 1e-6 is an arbitrary numerical tolerance
		theta = get_parity(curve, num_pts) * acos(Vector3f::dot(curve[0].N, curve[num_pts - 1].N)); // angular error in radians
		dtheta = theta / num_pts; // rotation for each Frenet-Serret frame

		// loop over all points on the curve
		for (int i = 1; i < num_pts; i++) { // index starts from 1 to leave init frame invariant
			
			// computing rotation matrix necessary for correction
			quaternion = Vector4f(cos(dtheta /2), 
				sin(dtheta / 2) * (curve[i].T)[0],
				sin(dtheta / 2) * (curve[i].T)[1],
				sin(dtheta / 2) * (curve[i].T)[2]); // computing quaternion
			quaternion.normalize(); // normalizing
			R_e = Matrix3f::rotation(quaternion); // rotation matrix

			// rotating i-th Frenet-Serret frame
			curve[i].N = R_e * curve[i].N; // rotating frame normals
			curve[i].B = Vector3f::cross(curve[i].T, curve[i].N); // re-computing frame binormals
			
			dtheta += theta / num_pts; // increment rotation angle for next frame
		}
	}
	
	return curve;
}


// function that computes the knots t_j
float t_j(float t_i, Vector3f P_i, Vector3f P_j) {
	/*
	Description:
		Function computes the j-th knot for a centripetal Catmull-Rom spline.

	Variables:
		t_i: the previous knot (t_i = t_{j-1})
		P_i: the root point 
		P_j: the subsequent point (P_j = P_{i+1}).

	Output:
		returns a float of the knot value.
	*/
	
	float alpha = 0.5; // knot paramterization
	return (pow(pow(P_j[0] - P_i[0], 2) + pow(P_j[1] - P_i[1], 2), 0.5 * alpha))  + t_i;
}


Curve evalCatmullRom(Vector3f P_0, Vector3f P_1, Vector3f P_2, Vector3f P_3, unsigned steps) {
	/*
	Description:
		Function evaluates a centripetal Catmull-Rom spline given 4 control points.

	Variables:
		P_0, P_1, P_2, P_3: the control points.
		steps: the number of discretized parameter values.

	Output:
		returns a curve (vector <CurvePoint>) of the Catmull-Rom spline.
	*/

	float t_0, t_1, t_2, t_3, t; // knot variables
	Vector3f A_1, A_2, A_3, B_1, B_2, C; // Catmull–Rom spline vectors
	CurvePoint point;
	Curve curve; // declaring curve variable

	// computing knots (t0 to t4)
	t_0 = 0;
	t_1 = t_j(t_0, P_0, P_1);
	t_2 = t_j(t_1, P_1, P_2);
	t_3 = t_j(t_2, P_2, P_3);

	// loop over interpolated parameter values
	t = t_1;
	for (float i = 0; i < steps; i++) {
		// computing vertices of the curve
		A_1 = (t_1 - t) / (t_1 - t_0) * P_0 + (t - t_0) / (t_1 - t_0) * P_1;
		A_2 = (t_2 - t) / (t_2 - t_1) * P_1 + (t - t_1) / (t_2 - t_1) * P_2;
		A_3 = (t_3 - t) / (t_3 - t_2) * P_2 + (t - t_2) / (t_3 - t_2) * P_3;

		B_1 = (t_2 - t) / (t_2 - t_0) * A_1 + (t - t_0) / (t_2 - t_0) * A_2;
		B_2 = (t_3 - t) / (t_3 - t_1) * A_2 + (t - t_1) / (t_3 - t_1) * A_3;

		C = (t_2 - t) / (t_2 - t_1) * B_1 + (t - t_1) / (t_2 - t_1) * B_2;

		point = { C, Vector3f(0., 0., 0.), Vector3f(0., 0., 0.), Vector3f(0., 0., 0.) };
		curve.push_back(point);

		t += (t_2 - t_1) / steps; // updating parameter for next iteration
	}
	
	return curve;
}


Curve evalCatmullRomChain(const vector< Vector3f >& P, unsigned steps) {
	/*
	Description:
		Function evaluates a chain of centripetal Catmull-Rom splines.

	Variables:
		P: the control points.
		steps: the number of discretized parameter values for each 4 point segment.

	Output:
		returns the ombined curve (vector <CurvePoint>) of the "stitched" Catmull-Rom spline.
	*/

	int num_pts = P.size();
	CurvePoint point; // curve point variable
	Curve c; // indiv segments
	Curve C; // combined curve variables

	// loop over control points
	for (int i = 0; i < num_pts - 3; i++) {
		c = evalCatmullRom(P[i], P[i + 1], P[i + 2], P[i + 3], steps);

		// loop over the individual Catmull-Rom splines (parameter values)
		for (int t = 0; t < c.size(); t++) {
			point = { c[t].V,
			Vector3f(0., 0., 0.),
			Vector3f(0., 0., 0.),
			Vector3f(0., 0., 0.) };
			C.push_back(point);
		}
		
	}

	// computing Frenet-Serret frame vectors
	float dt = 1. / C.size();
	Vector3f B, T, N;
	B = Vector3f(0., 0., 1.); // initi binormal vector
	T = (C[1].V - C[0].V) / dt; T.normalize(); // init tangent vector

	// checking for T != B and initilizing another binormal
	if (1. - Vector3f::dot(B, T) < 1e-8f) { // 1e-8 is just arbitrary numerical tolerance value
		B = Vector3f(0., 1., 0.);
	}
	

	// loop over discretized parameter values
	for (unsigned t_i = 1; t_i < C.size(); t_i++) {
		// computing Frenet-Serret vectors
		T = (C[t_i].V - C[t_i - 1].V) / dt; 
		N = Vector3f::cross(B, T);
		B = Vector3f::cross(T, N);

		// normalizing vectors
		T.normalize();
		N.normalize();
		B.normalize();

		// assigning Frenet-Serret vectors 
		C[t_i].T = T;
		C[t_i].N = -N;
		C[t_i].B = -B;
	}

	return C;
}


Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}


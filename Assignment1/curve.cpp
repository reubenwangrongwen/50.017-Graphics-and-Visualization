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
    
// function that generates points of Bezier spline given input control points
Curve evalBezier( const vector< Vector3f >& P, unsigned steps ) {

	/* 
	Description:
		Function computes all the appropriate Vector3fs for each CurvePoint: V,T,N,B.
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

	Curve curve; // vector of 4 vectors (vertex, tangent, normal, binormal)

	// iterating over t parameter values (entire curve)
	for (float t = 0; t < 1.0; t += 1.0 / steps) {
		float x = 1 - t; // variable for convenience
		CurvePoint point; // current point along spline

		/* === closed forms ===
			P'(t) = -3(1-t)^2 * P0 + 3(1-t)^2 * P1 - 6t(1-t) * P1 - 3t^2 * P2 + 6t(1-t) * P2 + 3t^2 * P3
			P''(t) = 6(1-t) * P0 + 6(3t-2) * P1 + 6(1-3t) * P2 + 6t * P3
		*/
		
		point.V = pow(x,3) * P[0]
				+ 3 * t * pow(x,2) * P[1]
				+ 3 * pow(t,2) * x * P[2]
				+ pow(t,3) * P[3]; // value of vertices (Bezier)

		point.T = -3 * pow(x,2) * P[0]
				+ (3 * pow(x,2) - 6 * pow(t,2)) * P[1]
				+ (6 * t - 9 * pow(t,2)) * P[2]
				+ 3 * pow(t, 2) * P[3]; // derivative of vertices
		point.T.normalize(); // tangent at point

		point.N = 6 * x * P[0] 
				+ 6 * (3*t - 2) * P[1] 
				+ 6 * (1 - 3*t) * P[2] 
				+ 6 *t * P[3]; // second derivative of vertices
		point.N.normalize(); // normal at point

		point.B = Vector3f::cross(point.T, point.N);
		point.B.normalize(); // binormal at point

		curve.push_back(point);
	}

    return Curve(); // returns the curve
}

// function that generates points of B-spline curve given input control points
Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    
	/*
	Description:
		Function computes all the appropriate Vector3fs for each CurvePoint for a B-spline: V,T,N,B.

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

	Curve curve; // vector of 4 vectors (vertex, tangent, normal, binormal)

	for (unsigned int i = 0; i < P.size() - 3; i += 1) {
		Vector3f P0, P1, P2, P3; // points for each B-spline segment
		if (i == P.size() - 3) {
			P0 = P[i]; P1 = P[i + 1]; P2 = P[i + 2]; P3 = P[i + 2];
		}
		else {
			P0 = P[i]; P1 = P[i + 1]; P2 = P[i + 2]; P3 = P[i + 3];
		}

		for (float t = 0; t < 1.0; t += 1.0 / steps) {
			float x = 1 - t;
			CurvePoint point;
			point.V = 1. / 6. * pow(x,3) * P0
					+ 1. / 6. * (3 * pow(t,3) - 6 * pow(t,2) + 4) * P1
					+ 1. / 6. * (-3 * pow(t,3) + 3 * pow(t,2) + 3 * t + 1) * P2
					+ 1. / 6. * pow(t,3) * P3;

			point.T = -0.5 * pow(x,2) * P0
					+ 0.5 * (3 * pow(t,2) - 4 * t) * P1
					+ 0.5 * (-3 * pow(t,2) + 2 * t + 1) * P2
					+ 0.5 * pow(t,2) * P3;
			point.T.normalize();

			point.N = -(1 - t) * P0
					+ (3 * t - 2) * P1
					+ (-3 * t + 1) * P2
					+ t * P3;
			point.N.normalize();

			point.B = Vector3f::cross(point.T, point.N);
			point.B.normalize();

			curve.push_back(point);
		}
	}

	cerr << "\t>>> Steps (type steps): " << steps << endl;
	cerr << "\t>>> Returning bsp curve." << endl;

	return curve;
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


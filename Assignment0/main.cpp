#include "zero_header.h"

using namespace std;

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc( keyboardFunc ); // Handles "normal" ascii symbols
    glutSpecialFunc( specialFunc );   // Handles "special" keyboard keys

	// Set up callbacks for mouse functionality (medium extra credit)
	glutMouseWheelFunc( mouseWheel );
	glutMouseFunc( mouse_click );
	glutMotionFunc( mouse_motion );
	
     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

	// timer for rotation (easy extra credit)
	glutTimerFunc(0, timer, 0);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <sstream>
#include <vecmath.h>
#include "camera.h"


#include "TimeStepper.hpp"
#include "simpleSystem.h"
#include "pendulumSystem.h"
#include "ClothSystem.h"

using namespace std;

// Globals here.
namespace
{
	// class pointer instances
    ParticleSystem *system;
    TimeStepper * timeStepper;
	
	// declare variables
	char sim_system; // variable to take simulated system from user input
	float h; // variable to take step size from user input
	int num_particles; // variable for number of particles
	int particle_idx = -1; // variable to store particle index 

  // initialize your particle systems
  void initSystem(int argc, char * argv[]) {
	  
	int time_step = 0; // init time stepper argument variable
	
	// seed the random number generator with the current time
    srand( time( NULL ) );	

	// check if right number of arguments have been provided
	if (argc < 4) {
		cout << "Error: too few arguments have been parsed!" << endl << endl;
		
		cout << "To run: a3.exe s e/t/r h" << endl;
		cout << "To run: a3.exe p num_particles particle_idx e/t/r h" << endl;
		cout << "To run: a3.exe c grid_size e/t/r h" << endl << endl;
		
		// running default simulation instead
		cout << "Default: Running simpleSystem with the RK4 ODE solver..." << endl;
		system = new SimpleSystem();
		sim_system = 's'; // update system type char variable
		time_step = 2; // update time stepper argument variable
		timeStepper = new RK4();
		// exit(0);
	}

	// if correct number of arguments have been parsed...
	else { 
		// for selecting the type of system to load (simple, pendulum, cloth)
		if (*argv[1] == 's') { // simple system 

			system = new SimpleSystem(); // simple system class 
			sim_system = 's'; // update system type char variable
			time_step = 2; // update time stepper argument variable
		}
		else if (*argv[1] == 'p') { // pendulum system

			// checking the right number of argumets for the pendulum system
			if (argc < 6) {
				cout << "To run: a3.exe p num_particles particle_idx e/t/r h" << endl;
				exit(0);
			}

			num_particles = atoi(argv[2]); // parse the number of particles 
			particle_idx = atoi(argv[3]); // parse index of particle to render

			system = new PendulumSystem(num_particles); // pendulum system class 

			if (particle_idx >= num_particles) {
				cout << "Error: particle_idx is larger than num_particles!" << endl;
				exit(0);
			}

			sim_system = 'p'; // update char variable
			time_step = 4; // update time stepper argument variable
		}
		else if (*argv[1] == 'c') { // cloth system 

			num_particles = atoi(argv[2]); // parse the number of particles 
			system = new ClothSystem(num_particles, num_particles, 0.1f);
			//system = new ClothSystem(num_particles);
			time_step = 3; // update time stepper argument variable
			sim_system = 'c'; // update system type char variable
		}
		else { 
			cout << "Error: Please input an appropriate argument (s/p/c) !" << endl;
			exit(0);
		}

		// getting numerical step size
		h = atof(argv[time_step + 1]); 

		// for selecting type of ODE solver
		if (*argv[time_step] == 'e') { // forward Euler ODE solver
			timeStepper = new ForwardEuler();
		}
		else if (*argv[time_step] == 't') { // trapezoidal ODE solver
			timeStepper = new Trapezoidal();
		}
		else if (*argv[time_step] == 'r') { // RK4 ODE solver
			timeStepper = new RK4();
		}
		else {
			cout << "Error: Please input an appropriate argument (e/t/r) !" << endl;
		}
	}
  }

  // Take a step forward for the particle shower
  ///TODO: Optional. modify this function to display various particle systems
  ///and switch between different timeSteppers
  void stepSystem()
  {
      ///TODO The stepsize should change according to commandline arguments
    const float h = 0.04f;
    if(timeStepper!=0){
      timeStepper->takeStep(system,h);
    }
  }

  // Draw the current particle positions
  void drawSystem()
  {
    
    // Base material colors (they don't change)
    GLfloat particleColor[] = {0.4f, 0.7f, 1.0f, 1.0f};
    GLfloat floorColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, particleColor);
    
    glutSolidSphere(0.1f,10.0f,10.0f);
    
    system->draw();
    
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorColor);
    glPushMatrix();
    glTranslatef(0.0f,-5.0f,0.0f);
    glScaled(50.0f,0.01f,50.0f);
    glutSolidCube(1);
    glPopMatrix();
    
  }
        

    //-------------------------------------------------------------------
    
        
    // This is the camera
    Camera camera;

    // These are state variables for the UI
    bool g_mousePressed = false;

    // Declarations of functions whose implementations occur later.
    void arcballRotation(int endX, int endY);
    void keyboardFunc( unsigned char key, int x, int y);
    void specialFunc( int key, int x, int y );
    void mouseFunc(int button, int state, int x, int y);
    void motionFunc(int x, int y);
    void reshapeFunc(int w, int h);
    void drawScene(void);
    void initRendering();

    // This function is called whenever a "Normal" key press is
    // received.
    void keyboardFunc( unsigned char key, int x, int y )
    {
        switch ( key )
        {
        case 27: // Escape key
            exit(0);
            break;
        case ' ':
        {
            Matrix4f eye = Matrix4f::identity();
            camera.SetRotation( eye );
            camera.SetCenter( Vector3f::ZERO );
            break;
        }
		case 'r':
		{
			if (sim_system == 'c') {
				cout << "rendering..." << endl;
				system->render_toggle ();
			}
			else {
				cout << "parse 'c' with the executable to begin cloth system simulation." << endl;
			}
			break;
		}
		case 's':
		{
			if (sim_system == 'c') {
				cout << "commencing motion..." << endl;
				system->motion_toggle ();
			}
			else {
				cout << "parse 'c' with the executable to begin cloth system simulation." << endl;
			}
			break;
		}
        default:
            cout << "Unhandled key press " << key << "." << endl;        
        }

        glutPostRedisplay();
    }

    // This function is called whenever a "Special" key press is
    // received.  Right now, it's handling the arrow keys.
    void specialFunc( int key, int x, int y )
    {
        switch ( key )
        {

        }
        //glutPostRedisplay();
    }

    //  Called when mouse button is pressed.
    void mouseFunc(int button, int state, int x, int y)
    {
        if (state == GLUT_DOWN)
        {
            g_mousePressed = true;
            
            switch (button)
            {
            case GLUT_LEFT_BUTTON:
                camera.MouseClick(Camera::LEFT, x, y);
                break;
            case GLUT_MIDDLE_BUTTON:
                camera.MouseClick(Camera::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                camera.MouseClick(Camera::RIGHT, x,y);
            default:
                break;
            }                       
        }
        else
        {
            camera.MouseRelease(x,y);
            g_mousePressed = false;
        }
        glutPostRedisplay();
    }

    // Called when mouse is moved while button pressed.
    void motionFunc(int x, int y)
    {
        camera.MouseDrag(x,y);        
    
        glutPostRedisplay();
    }

    // Called when the window is resized
    // w, h - width and height of the window in pixels.
    void reshapeFunc(int w, int h)
    {
        camera.SetDimensions(w,h);

        camera.SetViewport(0,0,w,h);
        camera.ApplyViewport();

        // Set up a perspective view, with square aspect ratio
        glMatrixMode(GL_PROJECTION);

        camera.SetPerspective(50);
        glLoadMatrixf( camera.projectionMatrix() );
    }

    // Initialize OpenGL's rendering modes
    void initRendering()
    {
        glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
        glEnable(GL_LIGHTING);     // Enable lighting calculations
        glEnable(GL_LIGHT0);       // Turn on light #0.

        glEnable(GL_NORMALIZE);

        // Setup polygon drawing
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

        // Clear to black
        glClearColor(0,0,0,1);
    }

    // This function is responsible for displaying the object.
    void drawScene(void)
    {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_MODELVIEW );  
        glLoadIdentity();              

        // Light color (RGBA)
        GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
        GLfloat Lt0pos[] = {3.0,3.0,5.0,1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
        glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

        glLoadMatrixf( camera.viewMatrix() );

        // THIS IS WHERE THE DRAW CODE GOES.

        drawSystem();

        // This draws the coordinate axes when you're rotating, to
        // keep yourself oriented.
        if( g_mousePressed )
        {
            glPushMatrix();
            Vector3f eye = camera.GetCenter();
            glTranslatef( eye[0], eye[1], eye[2] );

            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // This is to draw the axes when the mouse button is down
            glDisable(GL_LIGHTING);
            glLineWidth(3);
            glPushMatrix();
            glScaled(5.0,5.0,5.0);
            glBegin(GL_LINES);
            glColor4f(1,0.5,0.5,1); glVertex3f(0,0,0); glVertex3f(1,0,0);
            glColor4f(0.5,1,0.5,1); glVertex3f(0,0,0); glVertex3f(0,1,0);
            glColor4f(0.5,0.5,1,1); glVertex3f(0,0,0); glVertex3f(0,0,1);

            glColor4f(0.5,0.5,0.5,1);
            glVertex3f(0,0,0); glVertex3f(-1,0,0);
            glVertex3f(0,0,0); glVertex3f(0,-1,0);
            glVertex3f(0,0,0); glVertex3f(0,0,-1);

            glEnd();
            glPopMatrix();

            glPopAttrib();
            glPopMatrix();
        }
                 
        // Dump the image to the screen.
        glutSwapBuffers();
    }

    void timerFunc(int t)
    {
        stepSystem();

        glutPostRedisplay();

        glutTimerFunc(t, &timerFunc, t);
    }

    

    
    
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char* argv[] )
{
    glutInit( &argc, argv );

    // We're going to animate it, so double buffer 
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 600, 600 );
    
    camera.SetDimensions( 600, 600 );

    camera.SetDistance( 10 );
    camera.SetCenter( Vector3f::ZERO );
    
    glutCreateWindow("Assignment 4");

    // Initialize OpenGL parameters.
    initRendering();

    // Setup particle system
    initSystem(argc,argv);

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Trigger timerFunc every 20 msec
    glutTimerFunc(20, timerFunc, 20);

        
    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return 0;	// This line is never reached.
}

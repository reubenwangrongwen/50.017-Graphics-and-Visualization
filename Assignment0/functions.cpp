#include "zero_header.h"

using namespace std;

// -------------------------- Globals --------------------------//
// for color toggling
int color_idx = 0; // init colour index
float prevColors[4] = { 0.5, 0.5, 0.9, 1.0 }; // init color
GLfloat Lt0pos[4] = { 1.0f, 1.0f, 5.0f, 1.0f }; // init lighting position

// for 'r' press continuous rotation
bool r_keyRot = false; // boolean variable to start rotation
int ref_mill = 1.0; // millisecond refresh interval
GLfloat angle = 0.0f; // angle for rotation

// for mouse functionality
int gl_Button; // for mouse clicks 
GLfloat xScale = 1.0f; // init x scale
GLfloat yScale = 1.0f; // init y scale
int width, height; // window widths and heights
int  click_x, click_y; // mouse click variables
bool mouse_clicked; // mouse click checker
Matrix4f rotatn = Matrix4f::identity(); // active rotation
Matrix4f currentRot = Matrix4f::identity(); // storing the current rotation status

// for openGL display list
GLuint index; // display list

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned>> vecf;
// -------------------------------------------------------------//




// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f& a){ glVertex3fv(a); }

inline void glNormal(const Vector3f& a){ glNormal3fv(a); }


// timer function 
void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(ref_mill, timer, 0);
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc(unsigned char key, int x, int y) {
	switch (key)
	{
	case 27: // handling escape key
		exit(0);
		break;
	case 'c': // for color change
		color_idx = (color_idx + 1) % 4; // toggling color index upon 'c' press
		cout << "Color has been toggled " << color_idx << "." << endl;
		break;
	case 'r': // for rotation (easy extra credit)
		if (r_keyRot == true) {
			r_keyRot = false;
		}
		else {
			r_keyRot = true;
		}
		cout << "Part rotation: " << r_keyRot << endl;
		break;
	default: // handles all other keys
		cout << "Unhandled key press " << key << "." << endl;
	}

	// this will refresh the screen so that the user sees the color change
	glutPostRedisplay();
}


// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Lt0pos[0] -= 0.5;
		cout << "x light position toggled to " << Lt0pos[0] << "." << endl;
		break;
	case GLUT_KEY_RIGHT:
		Lt0pos[0] += 0.5;
		cout << "x light position toggled to " << Lt0pos[0] << "." << endl;
		break;
	case GLUT_KEY_UP:
		Lt0pos[1] += 0.5;
		cout << "y light position toggled to " << Lt0pos[1] << "." << endl;
		break;
	case GLUT_KEY_DOWN:
		Lt0pos[1] -= 0.5;
		cout << "y l ight position toggled to " << Lt0pos[1] << "." << endl;
		break;
	}

	// this will refresh the screen so that the user sees the light position
	glutPostRedisplay();
}

// function that allows mouse scrollwheel controls (medium extra credit)
void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		// zooming out
		xScale -= 0.01f;//increment x scale
		yScale -= 0.01f;//increment y scale
	}
	else
	{
		// zooming in
		xScale += 0.01f;//increment x scale
		yScale += 0.01f;//increment y scale
	}

	return;
}

// function that allows mouse (left/right) click controls (medium extra credit)
void mouse_click(int button, int state, int x, int y) {

	click_x = x;
	click_y = y;
	gl_Button = button;

	if (state == GLUT_DOWN) {
		mouse_clicked = true;
		switch (button) {
		case GLUT_LEFT_BUTTON:
			currentRot = rotatn;
			break;
		case GLUT_MIDDLE_BUTTON:

		case GLUT_RIGHT_BUTTON:

		default:
			break;
		}
	}
	else {
		rotatn = currentRot;
		mouse_clicked = false;
	}
}

// function that governs mouse-based rotations (medium extra credit)
void unit_sphere_rotation(int x, int y)
{
	Vector3f org; // orginal vector components
	Vector3f rot; // rotated (new) vector components
	float win_scale, org_norm, rot_norm, dot_prod, theta; // contractions

	// defining center of the windows as origin (-y ensures mouse up is up)
	org[0] = click_x - width / 2.0; org[1] = -(click_y - height / 2.0);
	rot[0] = x - width / 2.0; rot[1] = -(y - height / 2.0);

	// scaling vectors to the window size
	if (width > height) { win_scale = 1. / height; }
	else { win_scale = 1. / width; }
	org[0] = org[0] * win_scale; org[1] = org[1] * win_scale;
	rot[0] = rot[0] * win_scale; rot[1] = rot[1] * win_scale;

	// computing vector norms
	org_norm = sqrt(org[0] * org[0] + org[1] * org[1]);
	rot_norm = sqrt(rot[0] * rot[0] + rot[1] * rot[1]);

	// normalizing the vector components
	if (org_norm > 1.0) { org.normalize(); org_norm = 1.; }
	if (rot_norm > 1.0) { rot.normalize(); rot_norm = 1.; }

	// computing z-components on unit-sphere
	org[2] = sqrt(1.0 - org_norm * org_norm);
	rot[2] = sqrt(1.0 - rot_norm * rot_norm);

	dot_prod = org[0] * rot[0] + org[1] * rot[1] + org[2] * rot[2];

	// checking for linearly independent (parallel) vectors
	if (dot_prod != 1.) {

		// rotation axis is the cross product btwn the original and rotated vectors
		Vector3f rot_axis(org[1] * rot[2] - rot[1] * org[2], org[2] * rot[0] - rot[2] * org[0], org[0] * rot[1] - rot[0] * org[1]);

		rot_axis.normalize(); // normalize vector
		theta = acos(dot_prod); // angle between original and rotated vectors
		currentRot = Matrix4f::rotation(rot_axis, theta) * rotatn; // left multiplying rotation matrix
	}
	else {
		currentRot = rotatn;
	}
}


// function that checks mouse-click status (medium extra credit)
void mouse_motion(int x, int y)
{
	switch (gl_Button) {
	case GLUT_LEFT_BUTTON:
		unit_sphere_rotation(x, y);
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	default:
		break;
	}
	glutPostRedisplay();
}




// function that gets the vertices and normals of the triangles to openGL
void getVertNorm(void) {

	index = glGenLists(1); // create one display list (easy extra credit)
	glNewList(index, GL_COMPILE); // for openGL display higher performance

	// porting vertices and normals pairs to openGL
	for (int j = 0; j < vecf.size(); j++) {

		unsigned a, d, g, c, f, i; // defining triangle storage variables 
		glBegin(GL_TRIANGLES);

		a = vecf[j][0]; d = vecf[j][1]; g = vecf[j][2]; // vertices
		c = vecf[j][3]; f = vecf[j][4]; i = vecf[j][5]; // normals

		glNormal(vecn[c - 1]); glVertex(vecv[a - 1]);
		glNormal(vecn[f - 1]); glVertex(vecv[d - 1]);
		glNormal(vecn[i - 1]); glVertex(vecv[g - 1]);

		glEnd();
	}

	glEndList(); // for openGL display higher performance

	return;
}



// This function is responsible for displaying the object.
void drawScene(void)
{
	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);  // Current matrix affects objects positions
	glLoadIdentity(); // Initialize to the identity

	// Position the camera at [0,0,5], looking at [0,0,0],
	// with [0,1,0] as the up direction.

	// rotating the camera 
	/*gluLookAt(5. * sin(angle * PI / 180.), 0., 5. * cos(angle * PI / 180.),
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);*/ 
	
	// stationary camera
	gluLookAt( 0.0, 0.0, 5.0,
			   0.0, 0.0, 0.0,
			   0.0, 1.0, 0.0 );

	// Set material properties of object

	// Here are some colors you might use - feel free to add more
	GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
								 {0.9, 0.5, 0.5, 1.0},
								 {0.5, 0.9, 0.3, 1.0},
								 {0.3, 0.8, 0.9, 1.0} };

	// Here we use the first color entry as the diffuse color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[color_idx]);

	// Define specular color and shininess
	GLfloat specColor[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat shininess[] = { 100.0 };

	// Note that the specular color and shininess can stay constant
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	// Set light properties

	// Light color (RGBA)
	GLfloat Lt0diff[] = { 1.0,1.0,1.0,1.0 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
	glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// glutSolidTeapot(1.0); // default teapot sample
	
	// scaling of object (medium extra credit)
	glScalef(xScale, yScale, 1.0f);

	// This GLUT method draws object (w/ image rotation).	
	// mouse click rotation must be applied first, then continuous y rotation
	glPushMatrix(); // prevent disturbance to light position
	glMultMatrixf(currentRot); // applying mouse click rotation
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	getVertNorm();

	if (r_keyRot == true) {

		glCallList(index); // openGL display executable (easy extra credit)
		glPopMatrix();
		angle += 0.2; // for continuous rotation

	}
	else { glCallList(index); glPopMatrix(); }

	// Dump the image to the screen.
	glutSwapBuffers();
}

// Initialize OpenGL's rendering modes
void initRendering()
{
	glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
	glEnable(GL_LIGHTING);     // Enable lighting calculations
	glEnable(GL_LIGHT0);       // Turn on light #0.

}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
	// Always use the largest square viewport possible
	if (w > h) {
		glViewport((w - h) / 2, 0, h, h);
	}
	else {
		glViewport(0, (h - w) / 2, w, w);
	}
	width = w; height = h; // storing window width and height 

	// Set up a perspective view, with square aspect ratio
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 50 degree fov, uniform aspect ratio, near = 1, far = 100
	gluPerspective(50.0, 1.0, 1.0, 100.0);
}

// function that loads OBJ file
void loadInput()
{
	char buffer[MAX_BUFFER_SIZE];
	while (cin.getline(buffer, MAX_BUFFER_SIZE)) {

		stringstream ss(buffer);
		Vector3f v; string s;
		ss >> s;

		if (s == "v") {
			ss >> v[0] >> v[1] >> v[2];
			vecv.push_back(v);
		}
		else if (s == "vn") {
			ss >> v[0] >> v[1] >> v[2];
			vecn.push_back(v);
		}
		else if (s == "f") {

			int a, b, c, d, e, f, g, h, i;
			char ch;

			// loading from obj file into appropariate variables
			vector<unsigned> vec;
			ss >> a >> ch >> b >> ch >> c;
			ss >> d >> ch >> e >> ch >> f;
			ss >> g >> ch >> h >> ch >> i;

			vec.push_back(a); vec.push_back(d); vec.push_back(g); // vertex indices
			vec.push_back(c); vec.push_back(f); vec.push_back(i); // normal indices
			vec.push_back(b); vec.push_back(e); vec.push_back(h); // ignore for assignment 0

			vecf.push_back(vec);
		}
		else { continue; }
	};
	return;
}
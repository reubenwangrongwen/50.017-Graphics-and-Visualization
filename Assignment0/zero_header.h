#pragma once

// include libraries
#include "GL/freeglut.h"
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <vecmath.h>
#include <numeric>
#include <string> 


// declaring constants 
const float PI = 3.14159265;
const unsigned MAX_BUFFER_SIZE = 256; // max size for OBJ file 

// declaring functions 
inline void glVertex(const Vector3f& );
inline void glNormal(const Vector3f& ); 
void timer(int );
void keyboardFunc(unsigned char, int, int); 
void specialFunc(int, int, int);
void mouseWheel(int, int, int, int); 
void mouse_click(int, int, int, int);
void unit_sphere_rotation(int, int);
void mouse_motion(int, int);
void getVertNorm(void);
void drawScene(void);
void initRendering();
void reshapeFunc(int, int);
void loadInput();




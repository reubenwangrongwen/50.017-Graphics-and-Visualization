#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	/*
	Description:
		Function that loads the skin mesh file.

	Arguments:
		- filename: name of the mesh data file to be read.

	Return:
		void
	*/

	// declaring variables
	fstream fskel; // file stream object
	string str, s; // for file data
	Tuple3u vf; // tuple variable
	Vector3f v; // vector to store file data

	fskel.open(filename, fstream::in); // opening file

	// checking if file is open
	if (fskel.is_open() == true) {

		// loop over file data
		while (getline(fskel, str)) {
			stringstream stream(str); // stringstream object reading the file string
			stream >> s; // storing file data to string variable

			// checking type of vector being read
			if (s == "v") {
				stream >> v[0] >> v[1] >> v[2];
				bindVertices.push_back(v);
			}
			else if (s == "f") {
				stream >> vf[0] >> vf[1] >> vf[2];
				faces.push_back(vf);
			}
		}

		fskel.close(); // closing read file
	}
	else {
		cout << "Error: File could not be opened!" << endl;
	}

	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	/*
	Description:
		Function that draws the skin over the skeleton.
		(We have per-triangle normals rather than the analytical normals so the appearance is "faceted".)

	Arguments:
		- 

	Return:
		void
	*/

	Vector3f vertex_1, vertex_2, vertex_3;
	Vector3f normal;

	for (unsigned i = 0; i < faces.size(); i++) {
		// mesh vertices
		vertex_1 = currentVertices[faces[i][0] - 1];
		vertex_2 = currentVertices[faces[i][1] - 1];
		vertex_3 = currentVertices[faces[i][2] - 1];
		
		// computing normals
		normal = Vector3f::cross(vertex_2 - vertex_1, vertex_3 - vertex_1);
		normal.normalize(); 

		glBegin(GL_TRIANGLES);

		// storing normal
		glNormal3f(normal[0], normal[1], normal[2]);

		// storing vertices
		glVertex3f(vertex_1[0], vertex_1[1], vertex_1[2]); 
		glVertex3f(vertex_2[0], vertex_2[1], vertex_2[2]); 
		glVertex3f(vertex_3[0], vertex_3[1], vertex_3[2]);
		glEnd();
	}
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	/*
	Description:
		Function that loads the per-vertex attachment weights.
		(This method updates m_mesh.attachments.)

	Arguments:
		- filename: name of the mesh data file to be read.
		- numJoints: number of joints.

	Return:
		void
	*/

	fstream fskel; // declaring file stream variable
	string str; // declaring string variable for reading data

	fskel.open(filename, fstream::in); // opening file
	if (fskel.is_open() == true) {
		
		while (getline(fskel, str)) {
			stringstream stream(str); // stringstream object
			vector<float> weights; // vector of weights
			
			weights.push_back(0); // appending initial trivial weight

			// loop over joints
			for (int i = 0; i < numJoints - 1; i++) {
				float weight; // declaring joint weight variable
				stream >> weight; // storing file data to float variable
				weights.push_back(weight); // appending weight to weights vector
			}
			attachments.push_back(weights); // appending weights 
		}
		fskel.close(); // closing file
	}

}

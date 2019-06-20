#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// declaring variables
	fstream fskel; // file stream object
	string str, s; // for file data
	Vector3f v; // vector to store file data

	fskel.open(filename, fstream::in); // opening file

	// checking if file is open
	if (fskel.is_open() == true) {
		while (getline(fskel, str)) {
			stringstream stream(str); // stringstream object reading the file string
			stream >> s; // storing file data to string variable

			// checking type of vector being read
			if (s == "v") {
				stream >> v[0] >> v[1] >> v[2];
				bindVertices.push_back(v);
			}
			else if (s == "f") {
				Tuple3u vf;
				stream >> vf[0] >> vf[1] >> vf[2];
				faces.push_back(vf);
			}
			else {}
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
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".

	Vector3f vertex_1, vertex_2, vertex_3;
	Vector3f normal_1, normal_2, normal_3;

	for (unsigned i = 0; i < faces.size(); i++) {
		// mesh vertices
		vertex_1 = currentVertices[faces[i][0] - 1];
		vertex_2 = currentVertices[faces[i][1] - 1];
		vertex_3 = currentVertices[faces[i][2] - 1];
		
		// computing normals
		normal_1 = Vector3f::cross(vertex_2 - vertex_1, vertex_3 - vertex_1);
		normal_2 = Vector3f::cross(vertex_3 - vertex_2, vertex_1 - vertex_2);
		normal_3 = Vector3f::cross(vertex_1 - vertex_3, vertex_2 - vertex_3);
		normal_1.normalize(); normal_2.normalize(); normal_3.normalize();// normalizing 

		glBegin(GL_TRIANGLES);
		// storing vertices
		glVertex3f(vertex_1[0], vertex_1[1], vertex_1[2]); 
		glVertex3f(vertex_2[0], vertex_2[1], vertex_2[2]); 
		glVertex3f(vertex_3[0], vertex_3[1], vertex_3[2]);

		// storing normal
		glNormal3f(normal_1[0], normal_1[1], normal_1[2]);
		glNormal3f(normal_2[0], normal_2[1], normal_2[2]);
		glNormal3f(normal_3[0], normal_3[1], normal_3[2]);
		glEnd();
	}
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments

	ifstream file(filename); // opening file

	if (file.is_open() == true) {
		string str; // declaring string variable

		while (getline(file, str)) {
			istringstream iss(str); // stringstream object
			vector<float> weights; // vector of weights 
			float weight; // weight variable

			for (string s; iss >> weight; )
				weights.push_back(weight);
			attachments.push_back(weights);
		}
	}

}

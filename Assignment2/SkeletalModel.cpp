#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char* skeletonFile, const char* meshFile, const char* attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if (skeletonVisible)
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
		m_matrixStack.pop();
	}
}


void SkeletalModel::loadSkeleton(const char* filename)
{	
	/*
	Description:
		Function that parses the skeleton file into c++.

	Arguments:
		- filename: name of the file (char).

	Return:
		void
	*/

	// declaring variables
	fstream fskel; // file stream variable
	string data; // variable for reading line in input file 
	Vector3f v; // vector of transformation coordinates
	int idx; // joint index in the hierarchy

	// opening input file
	fskel.open(filename, fstream::in);

	if (fskel.is_open() == true) {
		while (getline(fskel, data)) {
			stringstream ss(data); // stringstream object to load .skel file data
			ss >> v[0] >> v[1] >> v[2] >> idx; // getting .skel file input into variables

			Joint* joint = new Joint; // assigning pointer to Joint class
			joint->transform = Matrix4f::translation(v); // assigning transform in Joint class 

			if (idx == -1) {
				m_rootJoint = joint; // assigning root pointer to current pointer
				m_joints.push_back(joint); // appending to vector of root joints (pointers)
			}
			else if ((idx != -1) && (idx < m_joints.size())) {
				Joint* parentJoint = m_joints[idx]; // parent pointer to a pointer entry of m_joints 
				parentJoint->children.push_back(joint); // assigning parent pointer to child
				m_joints.push_back(joint); // appending current joint to joints
			}
			else {
				cerr << "Error: skeleton file has wrong parent!" << endl;
			}
		}
		fskel.close();
	}
	else {
		cout << "Error: File could not be opened!" << endl;
	}
}


void get_joint (Joint* joint, MatrixStack& stack) {
	/*
	Description:
		Function that draws sphere at each joint by traversing joint hierarchy through recursion.

	Arguments:
		- joint: pointer to root joint.
		- stack: matrix stack.

	Return:
		void
	*/

	stack.push(joint->transform);

	// checking number of available joints
	if (joint->children.size() != 0) {
		// loop over children (joints)
		for (int i = 0; i < joint->children.size(); i++) {
			get_joint(joint->children[i], stack);
		}
	}

	glLoadMatrixf(stack.top()); // loading matrix from top of the stack
	glutSolidSphere(0.025f, 12, 12); // ball joint
	stack.pop();
}

void SkeletalModel::drawJoints() { get_joint (m_rootJoint, m_matrixStack); }



void get_bone (Joint* joint, MatrixStack& stack) {
	/*
	Description:
		Function that draws blocks (bones) by traversing joint hierarchy through recursion.

	Arguments:
		- joint: pointer to root joint.
		- stack: matrix stack.

	Return:
		void
	*/
	
	// declaring variables
	Matrix4f T, R, S;
	Vector3f vect, z_axis, normal;

	GLfloat len;
	
	stack.push(joint->transform);
	
	// checking number of available joints
	if (joint->children.size() != 0) {

		// loop over children (joints)
		for (int i = 0; i < joint->children.size(); i++) {
			
			Joint* child = joint->children[i]; // joint pointers from children vector
			vect = child->transform.getCol(3).xyz(); // getting vector from point
			len = vect.abs(); // norm of the vector
			
			// computing rotation axis 
			z_axis = vect.normalized();
			normal = Vector3f::cross(Vector3f(0, 0, 1), z_axis);

			// computing transformation matrices
			T = Matrix4f::translation(0, 0, 0.5);
			R = Matrix4f::rotation(normal, float(acos(z_axis.z()) * (len != 0)));
			S = Matrix4f::scaling(0.025f, 0.025f, len);

			stack.push(R * S * T); // pushing matrix transformation to stack
			glLoadMatrixf(stack.top()); // loading transformation matrix
			glutSolidCube(1.0f); // block bone
			stack.pop();

			// recursive call to function
			get_bone(child, stack);
		}
	}
	stack.pop();
}

void SkeletalModel::drawSkeleton() { get_bone(m_rootJoint, m_matrixStack); }



void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ) {
	/*
	Description:
		Set the rotation part of the joint's transformation matrix based on the arguments.

	Arguments:
		- jointIndex: integer that labels the joint index.
		- rX, rY, rZ: Euler angles (inputs are in degrees).

	Return:
		void
	*/

	Matrix4f M_x, M_y, M_z; // declaring rotation matrices

	// computing rotation matrices from Euler angles (* pi / 180.)
	M_x = Matrix4f::rotateX(rX);
	M_y = Matrix4f::rotateY(rY);
	M_z = Matrix4f::rotateZ(rZ); 

	Joint* joint = m_joints[jointIndex]; // defining pointer variable
	joint->transform = joint->transform * M_z * M_y * M_x; // applying rotations
}


void world_to_joint (Joint* joint, MatrixStack stack) {
	/*
	Description:
		Sets the joint rotation transformation matrix w.r.t the joint coordinates from the world coordinates
		(this method computes a per-joint transform from world-space to joint space in the BIND POSE).

	Arguments:
		- joint: pointer to root joint.
		- stack: matrix stack.

	Return:
		void
	*/
	stack.push(joint->transform.inverse());
	
	// checking for number of joints
	if (joint->children.size() != 0) {

		// loop over joints
		for (int i = 0; i < joint->children.size(); i++) {
			world_to_joint(joint->children[i], stack); // recursion
		}
	}
	
	joint->bindWorldToJointTransform = stack.top();
	stack.pop();
}

void SkeletalModel::computeBindWorldToJointTransforms() { world_to_joint (m_rootJoint, m_matrixStack); }


void joint_to_world (Joint* joint, MatrixStack stack) {
	/*
	Description:
		Sets the joint rotation transformation matrix w.r.t the world coordinates from the local coordinates
		(this method computes a per-joint transform from world-space to joint space in the BIND POSE).

	Arguments
		- joint: pointer to root joint.
		- stack: matrix stack.

	Return:
		void
	*/
	
	stack.push(joint->transform);
	
	// checking for number of joints
	if (joint->children.size() != 0) {

		// loop over joints
		for (int i = 0; i < joint->children.size(); i++) {
			joint_to_world(joint->children[i], stack); // recursion
		}
	}

	joint->currentJointToWorldTransform = stack.top();
	stack.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms() { joint_to_world(m_rootJoint, m_matrixStack); }



void SkeletalModel::updateMesh()
{
	/*
	Description:
		Updates the vertices of the mesh given the current state of the skeleton. This is the core of SSD.
		(uses both the bind pose world --> joint transforms and the current joint --> world transforms.)
		
	Arguments:
		-

	Return:
		void
	*/

	// init variables
	vector<float> weight; // declare vector of weights
	Vector3f current_v; // declare current vertex (vector)
	Vector4f new_v, update; // declare new vertex (point)

	// loop over mesh vertices
	for (unsigned v = 0; v < m_mesh.currentVertices.size(); v++) {
		
		current_v = m_mesh.bindVertices[v]; // current vertex
		new_v = Vector4f(0., 0., 0., 0.); // init new vertex
		weight = m_mesh.attachments[v]; // current weight value

		// loop over weights
		for (unsigned w = 0; w < weight.size(); w++) {
			Joint* joint = m_joints[w]; // init joint pointer

			// computing new vector for current iteration
			update = Vector4f(current_v, 1.); 
			update = (joint->bindWorldToJointTransform) * update;
			update = (joint->currentJointToWorldTransform) * update;
			new_v = new_v + (weight[w] * update); 
		}

		// update mesh vertex for next iteration
		m_mesh.currentVertices[v] = Vector3f(new_v[0], new_v[1], new_v[2]); 
	}
}



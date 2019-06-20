#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

float pi = 3.141592653;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
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

	if( skeletonVisible )
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
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
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

}


void get_joint (MatrixStack& stack, Joint* joint) {
	/*
	Description:
		Function that draws sphere at each joint by traversing joint hierarchy through recursion.

	Arguments:
		- stack: matrix stack.
		- joint: pointer to root joint.

	Return:
		void
	*/

	stack.push(joint->transform); // pushing transformation matrix to stack
	glLoadMatrixf(stack.top());  // loading the matrix to the stack top
	glutSolidSphere(0.025f, 12, 12); // creating the joint sphere 

	// recursion to load matrices to stack
	std::vector< Joint* >::iterator iter; // iterator for recursion
	for (iter = joint->children.begin(); iter != joint->children.end(); iter++) {
		get_joint(stack, *iter);
	}

	stack.pop(); // pop the current joint if no child
}

void SkeletalModel::drawJoints( ) { get_joint(m_matrixStack, m_rootJoint); }


void get_bone (MatrixStack& stack, Joint* joint) {
	/*
	Description:
		Function that draws blocks (bones) by traversing joint hierarchy through recursion.

	Arguments:
		- stack: matrix stack.
		- joint: pointer to root joint.

	Return:
		void
	*/
	
	Matrix4f M, T, R, S; // declaring transformation matrices
	Vector3f vect, x_hat, r_axis; // declaring vectors
	Vector4f pt; // declaring points
	float theta; // declaring rotation parameter

	stack.push(joint->transform); // pushing joint transformation to stack
	
	// recursion loop
	std::vector< Joint* >::iterator iter;
	for (iter = joint->children.begin(); iter != joint->children.end(); iter++) {
		M = stack.top(); // take matrix from top of stack
		vect = Vector3f(0.0, 0.0, 0.0); // reinit vector
		pt = Vector4f(0.0, 0.0, 0.0, 1.0); // reinit point

		// computing vectors for transformation
		pt = (*iter)->transform * pt;
		vect = Vector3f(pt[0], pt[1], pt[2]); // getting vector info from point
		x_hat = Vector3f(1.0, 0.0, 0.0); // x-axis unit vector 
		r_axis = Vector3f::cross(x_hat, vect); r_axis.normalize(); // normalized rotation axis

		// computing transformation matrices
		theta = acos( Vector3f::dot(vect, x_hat) / vect.abs() ); // rotation parameter
		R = Matrix4f::rotation(r_axis, theta); // rotation matrix
		T = Matrix4f::translation(0.5 * vect); // translation matrix
		S = Matrix4f::scaling(50 * vect.abs(), 1, 1); // scale matrix

		M = M * T * R * S; // M = TRS

		glLoadMatrixf(M); // loading transformed matrix to the stack
		glutSolidCube(.020); // drawing bone block
		get_bone(stack, *iter); // recalling function for recursion
	}

	stack.pop();
}

void SkeletalModel::drawSkeleton() { get_bone(m_matrixStack, m_rootJoint);  }

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

void world_to_joint (MatrixStack m_stack, Joint* joint) {
	/*
	Description:
		Sets the joint rotation transformation matrix w.r.t the joint coordinates from the world coordinates
		(this method computes a per-joint transform from world-space to joint space in the BIND POSE).

	Arguments:
		- m_stack: matrix stack.
		- joint: pointer to root joint.

	Return:
		void
	*/

	m_stack.push(joint->transform.inverse()); // pushing transformation matrix to stack
	glLoadMatrixf(m_stack.top());  // loading the matrix to the stack

	// recursion to load matrices to stack
	std::vector< Joint* >::iterator iter; // iterator for recursion
	for (iter = joint->children.begin(); iter != joint->children.end(); iter++) {
		world_to_joint(m_stack, *iter);
	}	

	m_stack.pop();
}

void SkeletalModel::computeBindWorldToJointTransforms() { world_to_joint(m_matrixStack, m_rootJoint); }


void joint_to_world (MatrixStack m_stack, Joint* joint) {
	/*
	Description:
		Sets the joint rotation transformation matrix w.r.t the world coordinates from the local coordinates
		(this method computes a per-joint transform from world-space to joint space in the BIND POSE).

	Arguments:
		- m_stack: matrix stack.
		- joint: pointer to root joint.

	Return:
		void
	*/

	m_stack.push(joint->transform);
	glLoadMatrixf(m_stack.top());  // loading the matrix to the stack
	
	// recursion to load matrices to stack
	std::vector< Joint* >::iterator iter; // iterator for recursion
	for (iter = joint->children.begin(); iter != joint->children.end(); iter++) {
		joint_to_world(m_stack, *iter);
	}
	
	m_stack.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms() { joint_to_world (m_matrixStack, m_rootJoint); }


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

			// computing new vector
			update = Vector4f(current_v, 1.);
			update = (joint->bindWorldToJointTransform) * update;
			update = (joint->currentJointToWorldTransform) * update;
			new_v = new_v + (weight[w] * update); 
		}

		// update mesh vertex for next iteration
		m_mesh.currentVertices[v] = Vector3f(new_v[0], new_v[1], new_v[2]); 
	}
}


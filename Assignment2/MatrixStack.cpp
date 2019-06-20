#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
	// Initialize the matrix stack with the identity matrix.
	m_matrices.push_back(Matrix4f::identity());
	
}

void MatrixStack::clear()
{
	// Revert to just containing the identity matrix.
	m_matrices.clear();
	m_matrices.push_back(Matrix4f::identity());
}

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
	return m_matrices[0];
}

void MatrixStack::push( const Matrix4f& m )
{
	// Push m onto the stack.
	// Your stack should have OpenGL semantics:
	// the new top should be the old top multiplied by m
	m_matrices.push_back(m);
	Matrix4f newTop = m_matrices[0] * m;
	m_matrices.insert(m_matrices.begin(), newTop);	
}

void MatrixStack::pop()
{
	// Remove the top element from the stack
	m_matrices.erase(m_matrices.begin());
}

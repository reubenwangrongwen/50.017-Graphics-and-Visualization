#pragma once

// including necessary libraries
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <Eigen/dense>


// type definitions
typedef Eigen::MatrixXcd cdM; // dense complex matrix
typedef Eigen::VectorXcd cdV; // dense conplex vector
typedef std::complex<double> cplex; // complex long floating point values

// constants
const cplex i(0., 1.); // complex number i
const double pi = 3.1415926535; // value of pi

// model parameters
const double dt = 0.05; // time steps
const double m = 0.1; // particle mass
double x_c = 0.; // center of square-well


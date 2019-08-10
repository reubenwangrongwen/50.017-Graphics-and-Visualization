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

// quantum model parameters
const int n0 = 1; // initial quantization index 
const double m = 0.1; // mass of quantum particle
const double x_min = -5.; // lower-bound of coordinate-space domain
const double x_max = 5.; // upper-bound of coordinate-space domain
const double L0 = 2.; // width of ISW
const double x_c = 0.; // center of square-well

// cloth model parameters
const int height = 12;
const int width = 12;
const cdV x_domain = Eigen::VectorXcd::LinSpaced(width, x_min, x_max); // coordinate space vector



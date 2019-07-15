# Assignment 1

Here is the completed code for assignment 3 writen in C++ on Visual Studio 2019. 
This program requires Windows SDK version 10.0.16299.0 to run. 

## TO DO

Extra credit assignments (optional).

## Extra Credit Components

Attempted an implementation of the Runge-Kutta-Felhberg numerical ODE technique. It is still very slow and needs to be optimized. 

## Running the Executable

To run the simple particle system:
```
cd [path] a3.exe s e/t/r/R h
```

To run the pendulum system:
```
cd [path] a3.exe p num_particles idx e/t/r/R h
```

To run the cloth system:
```
cd [path] a3.exe c grid_edge e/t/r/R h
```

where [path] is the file path to the solution folder, num_particles is the number of particles in the simulation, 
grid_edge is the number of particles per edge of the cloth simulation, idx is the index of the particle to be rendered, 
e/t/r/R is the choice of numerical ODE solver:

* e: Forward Euler method.
* t: Trapezoidal method.
* r: Runge-Kutta 4th order method.
* R: Runge-Kutta-Felhberg 4th/5th order method.

and h is the numerical step size.

## References

Below is a list of references used for the completion of this assignment. 

http://songho.ca/opengl/index.html

http://www.cplusplus.com/reference/

http://maths.cnam.fr/IMG/pdf/RungeKuttaFehlbergProof.pdf





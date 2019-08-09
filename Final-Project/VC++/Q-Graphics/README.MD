# Quantum Infinite-Square Well w/ Cloth System

Here is the code for visualizing a quantum infinite-square well system with an added cloth-life surface. 
This code is written in C++ on Visual Studio 2019. 
This program requires Windows SDK version 10.0.16299.0 to run. 

### Class Inheritance Hierarchy

From parent to child classes, the class inheritance hierarchy is as follows:
* wavefunction
* ParticleSystem
* ClothSystem
* TimeEvolution

## TO DO

* Fix cloth force vectors for the cloth system.
* Implement quantum time-evolution. 

## Running the Executable

To run the particle system simulations:
```
cd [path] quantum_ISW.exe e/t/r/R h
```

where [path] is the file path to the solution folder, "e/t/r/R" is the choice of numerical ODE solver:

* e: Forward Euler method.
* t: Trapezoidal method.
* r: Runge-Kutta 4th order method.
* R: Runge-Kutta-Felhberg 4th/5th order method.

and "h" is the numerical step size (optional argument).

### Simulation Functionalities

You can use "r" to toggle between smooth cloth shading and grid render options.

## References

Below is a list of references used for the completion of this assignment. 

http://songho.ca/opengl/index.html

http://www.cplusplus.com/reference/

http://maths.cnam.fr/IMG/pdf/RungeKuttaFehlbergProof.pdf

https://www.uni-muenster.de/imperia/md/content/physik_tp/lectures/ss2016/num_methods_ii/rkm.pdf





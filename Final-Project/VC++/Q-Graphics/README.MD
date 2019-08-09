# Assignment 3

Here is the completed code for assignment 3 writen in C++ on Visual Studio 2019. 
This program requires Windows SDK version 10.0.16299.0 to run. 

## TO DO

* Fix cloth rendering for it to render both sides.
* Optimize Runge-Kutta-Felhberg numerical ODE solver method.

## Extra Credit Components

* Implemented the Runge-Kutta 4th order numerical ODE solver (**easy extra credit**). 
* Implemented smooth shading for cloth system, but only renders one side (**easy extra credit**).
* Implementated the Runge-Kutta-Felhberg numerical ODE technique (**medium extra credit**). 

## Running the Executable

To run the particle system simulations:
```
cd [path] a3.exe e/t/r/R h
```

where [path] is the file path to the solution folder, "e/t/r/R" is the choice of numerical ODE solver:

* e: Forward Euler method.
* t: Trapezoidal method.
* r: Runge-Kutta 4th order method.
* R: Runge-Kutta-Felhberg 4th/5th order method.

and "h" is the numerical step size (optional argument).

### Simulation Functionalities

While in the graphical particle system simulation window, use "t" to toggle between different systems (simple, pendulum, cloth). 
While in the cloth system, you can:
* use "r" to toggle between smooth cloth shading and grid render options,  
* use "m" to toggle between applying a centripetal force to act on the cloth pin points to cause motion.

## References

Below is a list of references used for the completion of this assignment. 

http://songho.ca/opengl/index.html

http://www.cplusplus.com/reference/

http://maths.cnam.fr/IMG/pdf/RungeKuttaFehlbergProof.pdf

https://www.uni-muenster.de/imperia/md/content/physik_tp/lectures/ss2016/num_methods_ii/rkm.pdf





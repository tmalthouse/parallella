#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "e_lib.h"

//HACK: Should be sizeof(). Doesn't really matter on any system in the past 30 years though.
#define FLOAT_SIZE 4
#define CORE_COUNT 16
//TODO: Get max iterations from host program
#define MAX_ITER 10000000


#define INITIAL_SPACING 10 //m

//Masses and acceleration are in arbitrary units.
#define k 10 //force/dist
#define m 128 //mass
#define pull = 10 //mass*dist/time^2

volatile unsigned long long *t = (void *) 0x7000;
volatile unsigned long long *iterations = (void*) 0x7008;
volatile float *locations = (void *) 0x7010;
volatile float *velocities = (void *) (location + FLOAT_SIZE*CORE_COUNT);

int main(void) {
  e_coreid_t id = e_get_coreid();
  unsigned row, col;

  e_coords_from_coreid (id, &row, &col);

  //Set the initial location of the body
  //Use char to save memory--would need to be changed to int (int16?) for >255 cores
  char order = 4*row + col;
  unsigned long position = order*INITIAL_SPACING;
  float vel = 0;
  unsigned long iter = 0;



  while (iter<MAX_ITER) {
    iter++;
    /*Stepping thorough:
    // k is the spring constant
    ((*(locations--)-position)-INITIAL_SPACING is the amount the joint is stressed (distance fron equilibrium)
    We repeat that for the other side, and subtract the two to get the net force on the body
    ...and divide by the mass to get the acceleration.
    */
    //TODO: Make sure the division always takes same number of clock cycles
    float accNet = (k*((*(locations-FLOAT_SIZE)-position)-INITIAL_SPACING)) - (k*((*(locations+FLOAT_SIZE)-position)-INITIAL_SPACING))/m;
    //Then we change the velocity by the acceleration (dT is 1 sec, so it cancels out.)
    vel += accNet;
    //Update the position...
    position += vel;
    //And write those changes to shared memory.
    *(velocities+order*FLOAT_SIZE) = vel;
    *(locations+order*FLOAT_SIZE) = position;
    *iterations++;
    //TODO: ensure clock cycles stay synchronized.
  }
  return EXIT_SUCCESS;
}

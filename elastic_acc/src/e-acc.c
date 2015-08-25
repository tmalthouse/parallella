#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "e_lib.h"

#define FLOAT_SIZE 4
#define CORE_COUNT 16
#define MAX_ITER 10000000


#define INITIAL_SPACING 10 //m

//Masses and acceleration are in kg and m/s^2, respectively. K is in N/m (kg/s^2)
#define k 10 //N/m
#define m 128 //kg
#define pull = 10 //N

volatile unsigned long long *t = (void *) 0x7000;
volatile unsigned long long *iter = (void *) 0x7008;
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



  while (*iter/16<MAX_ITER) {
    *iter++;
    /*Stepping thorough:
    // k is the spring constant
    ((*(locations--)-position)-INITIAL_SPACING is the amount the joint is stressed (distance fron equilibrium)
    We repeat that for the other side, and subtract the two to get the net force on the body
    ...and divide by the mass to get the acceleration.
    */
    float accNet = (k*((*(locations-FLOAT_SIZE)-position)-INITIAL_SPACING)) - (k*((*(locations+FLOAT_SIZE)-position)-INITIAL_SPACING))/m;
    //Then we change the velocity by the acceleration (dT is 1 sec, so it cancels out.)
    vel += accNet;
    //Update the position...
    position += vel;
    //And write those changes to shared memory.
    *(velocities+order*FLOAT_SIZE) = vel;
    *(locations+order*FLOAT_SIZE) = position;
  }
  return EXIT_SUCCESS;
}

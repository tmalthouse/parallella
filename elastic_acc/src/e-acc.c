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
#define invM 128 //mass^-1
#define pull 10 //mass*dist/time^2

volatile unsigned long long *t = (void *) 0x7000;
volatile unsigned long long *iterations = (void*) 0x7008;
volatile float *locations = (void *) 0x7010;
volatile float *velocities = (void *) 0x7050;

float net_acceleration(float spr_const, char order, float * locations, float mass, float position);

int main(void) {
  e_coreid_t id = e_get_coreid();
  unsigned row, col;

  e_coords_from_coreid (id, &row, &col);

  //Set the initial location of the body
  //Use char to save memory--would need to be changed to int (int16?) for >255 cores
  char order = 4*row + col;
  unsigned long position = order*INITIAL_SPACING+30;
  float vel = 30;
  unsigned long iter = 0;


  while (iter<MAX_ITER) {
    iter++;

    //TODO: Make sure the division always takes same number of clock cycles
    float accNet = net_acceleration(k, order, locations, m, position);
    //Then we change the velocity by the acceleration (ΔT is 1 unit, so it cancels out.)
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

inline float net_acceleration(float spr_const, char order, float * locations, float inv_mass, float position) {
  switch (order){

    /*Stepping thorough:
    ((*(locations--)-position)-INITIAL_SPACING is the amount the joint is stressed (distance fron equilibrium)
    We repeat that for the other side, and subtract the two to get the net force on the body
    ...and divide by the mass to get the acceleration (Actually multiply by 1/m for performance).
    */

    case (0):
      return (pull-spr_const*((position-locations[1])-INITIAL_SPACING)*inv_mass);

    case (15):
      return (spr_const*((locations[14]-position)-INITIAL_SPACING))*inv_mass;

    default:
      return (spr_const*((locations[order--]-position)-INITIAL_SPACING)-spr_const*((position-locations[order++])-INITIAL_SPACING))*inv_mass;
  }

}

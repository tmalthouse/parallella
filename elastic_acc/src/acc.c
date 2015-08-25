#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

#include <e-hal.h>
//TODO: Send max iterations to device program
#define MAX_ITERATIONS = 10000000;

int main(int argc, char *argv[]) {

  unsigned row, col, coreid, i, j;
	e_platform_t platform;
	e_epiphany_t dev;

  // Initialize the Epiphany HAL and connect to the chip
	e_init(NULL);

	// Reset the system
	e_reset_system();

	// Get the platform information
	e_get_platform_info(&platform);

	// Create a workgroup using all of the cores
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	e_reset_group(&dev);

	// Load the device code into each core of the chip, and don't start it yet
	e_load_group("e-acc.elf", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);

  e_start_group(&dev);
  uint64_t iterations;

  while (MAX_ITERATIONS*16>iterations) {
    for(row=0;row<platform.rows;row++)
		{
			for(col=0;col<platform.cols;col++)
			{
        uint64_t timestep;
        uint64_t iter_num;
        float loc;
        float vel;

        if(e_read(&dev, row, col, 0x7000, &timestep, sizeof(uint64_t)) != sizeof(uint64_t)){
					fprintf(stderr, "Failed to read\n");
        }
        if(e_read(&dev, row, col, 0x7008, &iter_num, sizeof(uint64_t)) != sizeof(uint64_t)){
  				fprintf(stderr, "Failed to read\n");
        }
        if(e_read(&dev, row, col, 0x7010, &loc, sizeof(float)) != sizeof(float)){
					fprintf(stderr, "Failed to read\n");
        }
        if(e_read(&dev, row, col, 0x7010 + 4*16, &vel, sizeof(float)) != sizeof(float)){
					fprintf(stderr, "Failed to read\n");
        }

        fprintf(stderr, "The timestep is %d, the iterations*16=%d, the position is %f, and the vel is %f\n", timestep, iter_num, loc, vel);
        iterations += iter_num;
      }
    }
  }

}

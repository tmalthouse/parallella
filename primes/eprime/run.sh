#!/bin/bash

set -e

#Set this to 1/(n*8) the number you want to test to, where n is the core number.
PRIMES_PER_CORE=500000

./prime.elf ${PRIMES_PER_CORE}

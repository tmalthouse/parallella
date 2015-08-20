#!/bin/bash

set -e

PRIMES_PER_CORE=500000


./prime.elf ${PRIMES_PER_CORE}

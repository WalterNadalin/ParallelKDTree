#!/bin/bash

#PBS -q dssc
#PBS -l nodes=2:ppn=24
#PBS -l walltime=2:00:00 

cd ~/SecondAssignment/Hybrid/

module load openmpi-4.1.1+gnu-9.3.0

make clean
make parallel=1

rm data/hybrid.dat

export OMP_PLACES=cores

processors=4
points=15
information=all
threads=2

mpirun -np processors -npernode 2 --map-by socket ./tree.x $points $information $thread > data/hybrid.dat

exit

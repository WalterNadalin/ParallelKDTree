#!/bin/bash

#PBS -q dssc
#PBS -l nodes=2:ppn=24
#PBS -l walltime=0:00:10 

cd ~/SecondAssignment/Hybrid/

module load openmpi-4.1.1+gnu-9.3.0

make clean
make parallel=1

rm data/hybrid.dat

export OMP_PLACES=cores

points=31
information=info
threads=4

#mpirun -np 1 --map-by socket ./tree.x $points $information $threads > data/hybrid.dat
mpirun -np 2 -npernode 2 --map-by socket ./tree.x $points $information $threads # >> data/hybrid.dat
#mpirun -np 4 -npernode 2 --map-by socket ./tree.x $points $information $threads >> data/hybrid.dat
#mpirun -np 8 -npernode 2 --map-by socket ./tree.x $points $information $threads >> data/hybrid.dat

exit

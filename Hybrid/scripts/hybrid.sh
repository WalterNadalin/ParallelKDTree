#!/bin/bash

#PBS -q dssc
#PBS -l nodes=4:ppn=24
#PBS -l walltime=2:00:00 

cd ~/SecondAssignment/Hybrid/

module load openmpi-4.1.1+gnu-9.3.0

make clean
make parallel=1

rm data/hybrid.dat

export OMP_PLACES=cores

points=28
information=nothing
threads=12

#mpirun -np 1 --map-by socket ./tree.x $points $information $threads > data/hybrid.dat
#mpirun -np 2 -npernode 2 --map-by socket ./tree.x $points $information $threads >> data/hybrid.dat
#mpirun -np 4 -npernode 2 --map-by socket ./tree.x $points $information $threads >> data/hybrid.dat
mpirun -np 8 -npernode 2 --map-by socket ./tree.x $points $information $thread >> data/hybrid.dat

exit

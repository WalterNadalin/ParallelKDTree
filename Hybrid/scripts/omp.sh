
#!/bin/bash

#PBS -q dssc_gpu
#PBS -l nodes=1:ppn=12
#PBS -l walltime=01:00:00 

cd ~/SecondAssignment/Hybrid/

module load openmpi-4.1.1+gnu-9.3.0

make clean
make parallel=1

rm data/omp.dat

export OMP_PLACES=cores

mpirun -np 1 --map-by socket ./tree.x 18 nothing 12 2>/dev/null > data/omp.dat

exit

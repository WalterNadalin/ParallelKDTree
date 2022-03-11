
#!/bin/bash

#PBS -q dssc
#PBS -l nodes=2:ppn=24
#PBS -l walltime=01:00:00 

cd ~/SecondAssignment/Hybrid/

module load openmpi-4.1.1+gnu-9.3.0

export OMP_PLACES=sockets
export I_MPI_PIN=enable

mpirun -n 4 -npernode 2 --map-by socket ./a.out 2>/dev/null

exit

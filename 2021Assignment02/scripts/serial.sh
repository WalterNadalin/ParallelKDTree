
#!/bin/bash

#PBS -q dssc
#PBS -l nodes=1:ppn=1
#PBS -l walltime=03:30:00 

cd ~/SecondAssignment/Hybrid/

module load openmpi-4.1.1+gnu-9.3.0

make clean
make

rm data/serial.dat

$points=15
$information=print

./tree.x $points $information

exit

#!/bin/bash
#SBATCH --job-name="game-of-life"
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --time=00:05:00
#SBATCH --partition=normal
#SBATCH --output=gol_%j.out
#SBATCH --error=gol_%j.err

module purge
module load 2020
module load GCC

echo "OpenMP Game Of Life"
echo

for ncores in {16..16}

do
  export OMP_NUM_THREADS=$ncores
  echo "CPUS: " $OMP_NUM_THREADS
  ./main
  echo "DONE "
done

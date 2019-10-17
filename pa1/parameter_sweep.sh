#PBS -N parameter-sweep
#PBS -l walltime=1:00:00
#PBS -l nodes=1:ppn=40
#PBS -j oe
#PBS -A PZS0711

# load modules
module load intel
module load python
module load cmake

# Move sources to TMPDIR and build
cd ${TMPDIR}
cp ${PBS_O_WORKDIR}/* . -rp

mkdir build && cd build
CC=icc cmake ..
make
mv amr ../. && cd ..

# run tests
vals=$(python -c "import numpy; print(' '.join(map(str, numpy.linspace(0.10, 0.01, 10))))")
for affect_rate in $vals; do
  for epsilon in $vals; do
    time ./amr ${affect_rate} ${epsilon} <tests/testgrid_400_12206
  done
done

# Post-process

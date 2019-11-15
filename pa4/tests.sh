#PBS -N AMR-tests
#PBS -l walltime=4:00:00
#PBS -l nodes=1:ppn=28:gpus=1
#PBS -j oe
#PBS -A PZS0711

# Load modules
module load cuda

# Move sources to TMPDIR and build
cd ${TMPDIR}
cp ${PBS_O_WORKDIR}/* . -rp

make clean && make

# Testing for small file

affect_rate="0.1"
epsilon="0.1"

mkdir -p ${PBS_O_WORKDIR}/results

test_file="tests/testgrid_400_12206"
test_name=$(basename ${test_file})
mkdir ${test_name}
echo "Running tests with file ${test_file}..."

prog="cuda-persistent"
echo "Using ${prog}..."
results_file=$(basename ${test_file})/${prog}
echo "" >${results_file}
for t in 32 64 96 128 256 288 320 352 384; do
  { time ./${prog} ${affect_rate} ${epsilon} ${t} 1 <${test_file}; } 2>&1 | tee -a ${results_file}
  cp ${results_file} ${HOME}/$(basename ${test_file})_${prog}
done

prog="cuda-disposable"
echo "Using ${prog}..."
results_file=$(basename ${test_file})/${prog}
echo "" >${results_file}
for b in 1 2 4 6 8 10 12; do
  for t in 32 64 96 128 256; do
    { time ./${prog} ${affect_rate} ${epsilon} ${t} ${b} <${test_file}; } 2>&1 | tee -a ${results_file}
    cp ${results_file} ${HOME}/$(basename ${test_file})_${prog}
  done
done

echo "Finished tests with file ${test_file}"

mkdir -p ${PBS_O_WORKDIR}/results/${test_name} && cp ${test_name}/* ${PBS_O_WORKDIR}/results/${test_name}/

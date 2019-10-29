#PBS -N AMR-tests
#PBS -l walltime=1:00:00
#PBS -l nodes=1:ppn=40
#PBS -j oe
#PBS -A PZS0711
#PBS -q debug

run_tests() {
  local test_file="$1"

  echo "Running tests with file ${test_file}..."

  # Run tests
  for prog in ${programs[@]}; do
    echo "Using ${prog}..."
    results_file=$(basename ${test_file})/${prog}
    echo "" >${results_file}
    for t in ${threads}; do
      { time ./${prog} ${affect_rate} ${epsilon} ${t} <${test_file}; } 2>&1 | tee -a ${results_file}
      cp ${results_file} ${HOME}/$(basename ${test_file})_${prog}
    done
  done

  echo "Finished tests with file ${test_file}"
}

# Load modules
module load intel
module load python

# Bring in configuration file
# affect_rate, epsilon, programs, tests
source ${PBS_O_WORKDIR}/tests.cfg

# Move sources to TMPDIR and build
cd ${TMPDIR}
cp ${PBS_O_WORKDIR}/* . -rp

make clean && make

# Run tests
mkdir -p ${PBS_O_WORKDIR}/results

for test_file in ${test_files[@]}; do
  test_name=$(basename ${test_file})
  mkdir ${test_name}
  run_tests ${test_file}
  mkdir -p ${PBS_O_WORKDIR}/results/${test_name} && cp ${test_name}/* ${PBS_O_WORKDIR}/results/${test_name}/
done

cd ${PBS_O_WORKDIR}
#python process_results.py results/tests_results.png results/

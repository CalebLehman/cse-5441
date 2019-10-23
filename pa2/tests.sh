#PBS -N AMR-tests
#PBS -l walltime=0:30:00
#PBS -l nodes=1:ppn=40
#PBS -j oe
#PBS -A PZS0711
#PBS -q debug

run_tests() {
  local test_file="$1"

  echo "Running tests with file ${test_file}..."

  # Run tests
  for prog in ${programs[@]}; do
    results_file=$(basename ${test_file})_${prog}_results.txt
    for t in ${threads}; do
      echo "=> threads ${t}" | tee -a ${results_file}
      { time ./${prog} ${affect_rate} ${epsilon} ${t} <${test_file}; } 2>&1 | tee -a ${results_file}
    done
  done

  echo "Finished tests with file ${test_file}"
}

# Load modules
module load intel
module load python

# Move sources to TMPDIR and build
cd ${TMPDIR}
cp ${PBS_O_WORKDIR}/* . -rp

make clean && make

# Bring in configuration file
# affect_rate, epsilon, programs, tests
source tests.cfg

# Run tests
mkdir -p ${PBS_O_WORKDIR}/results

for test_file in ${test_files[@]}; do
  run_tests ${test_file}
done

# Process results
for test_file in ${test_files[@]}; do
  test_name=$(basename ${test_file})
  results_files=
  for prog in ${programs[@]}; do
    results_files="${results_files} ${test_name}_${prog}_results.txt"
  done
  plt_out_file=${test_name}_results.png
  echo "python process_results.py ${test_name} ${plt_out_file} ${results_files}"
  for prog in ${programs[@]}; do
    results_file=${test_name}_${prog}_results.txt
    mv ${results_file} ${PBS_O_WORKDIR}/results/.
  done
  echo "mv ${plt_out_file} ${PBS_O_WORKDIR}/results/."
done

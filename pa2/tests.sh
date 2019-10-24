#PBS -N AMR-tests
#PBS -l walltime=6:00:00
#PBS -l nodes=1:ppn=40
#PBS -j oe
#PBS -A PZS0711

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

results_files=
test_names=
for test_file in ${test_files[@]}; do
  test_name=$(basename ${test_file})
  test_names="${test_names} ${test_name}"
  for prog in ${programs[@]}; do
    results_file=${test_name}_${prog}_results.txt
    results_files="${results_files} ${results_file}"
  done
done
cd ${PBS_O_WORKDIR}
plt_out_file=tests_results.png
python process_results.py ${plt_out_file} results/ -t ${test_names} -p ${programs[@]}

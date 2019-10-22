#PBS -N AMR-tests
#PBS -l walltime=0:30:00
#PBS -l nodes=1:ppn=40
#PBS -j oe
#PBS -A PZS0711
#PBS -q debug

run_tests() {
  local test_file="$1"

  echo "Running tests with file ${test_file}..."

  # Make directory to store temporary results
  results_dir=${test_file}_results
  mkdir ${results_dir}

  # Run tests
  threads=$(python -c "\
print(list(range(20)))"\
  )
  for t in ${threads}; do
    results_file=${results_dir}/${affect_rate}_${epsilon}
      { time ./amr ${affect_rate} ${epsilon} <${test_file}; } 2>&1 | tee -a ${results_file} &
  done
  wait $(jobs -rp)

  # Aggregate results
  results_file=$(basename ${test_file})_results.txt
  cat ${results_dir}/* >${results_file}

  echo "Finished tests with file ${test_file}"
}

# Load modules
module load intel
module load python

# Move sources to TMPDIR and build
cd ${TMPDIR}
cp ${PBS_O_WORKDIR}/* . -rp

make clean && make

# Run tests
mkdir -p ${PBS_O_WORKDIR}/results

source tests.cfg
for test_file in ${test_files[@]}; do
  run_tests ${test_file}
done

# Process results
for test_file in ${test_files[@]}; do
  test_name=$(basename ${test_file})
  results_file=${test_name}_results.txt
  plt_out_file=${test_name}_results.png
  python process_parameter_sweep.py ${test_name} ${results_file} ${plt_out_file}
  mv ${results_file} ${PBS_O_WORKDIR}/results/.
  mv ${plt_out_file} ${PBS_O_WORKDIR}/results/.
done

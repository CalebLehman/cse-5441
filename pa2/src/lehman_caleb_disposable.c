#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "amr.h"
#include "common.h"

const char* usage = "\
Usage: disposable [affect-rate] [epsilon] [num-threads]\n\
\n\
affect-rate: float value controlling the effect of neighboring boxes\n\
epislon    : float value determining the cutoff for convergence\n\
             should be non-negative\n\
num-threads: number of threads to spawn for computation\n\
             should be positive\n";

int main(int argc, char** argv) {
    /**
     * Parse command-line arguments
     */
    if (argc != 4) {
        printf("%s", usage);
        exit(1);
    }
    float affect_rate = strtof(argv[1], NULL);
    float epsilon     = strtof(argv[2], NULL);
    Count num_threads = strtol(argv[3], NULL, 10);
    if ((epsilon < 0) || (num_threads < 1)) {
        printf("Invalid parameters\n");
        printf("%s", usage);
        exit(1);
    }

    /**
     * Parse input data from standard input
     */
    AMRInput* input = parseInput();

    /**
     * Run and collect timing information
     */
    time_t  time_before;
    time(&time_before);
    clock_t clock_before = clock();
    struct timespec gettime_before;
    clock_gettime(CLOCK_REALTIME, &gettime_before);

    AMROutput output = run(input, affect_rate, epsilon, num_threads);

    time_t time_after;
    time(&time_after);
    time_t clock_after = clock();
    struct timespec gettime_after;
    clock_gettime(CLOCK_REALTIME, &gettime_after);

    output.time_seconds  = difftime(time_after, time_before);
    output.clock_seconds = (clock_after - clock_before) / (double) CLOCKS_PER_SEC;
    output.gettime_seconds = (double) (
        (gettime_after.tv_sec - gettime_before.tv_sec) +
        ((gettime_after.tv_nsec - gettime_before.tv_nsec) / 1000000000.0)
    );

    /**
     * Display results
     */
    displayOutput(output);

    /**
     * Clean up
     */
    destroyInput(input);
    return 0;
}

/**
 * {@inheritDoc}
 */
AMROutput run(AMRInput* input, float affect_rate, float epsilon, Count num_threads) {
    /**
     * Repeat until convergence
     */
    AMRMaxMin max_min = getMaxMin(input);
    DSV* updated_vals = malloc(input->N * sizeof(*updated_vals));

    /**
     * updated_vals and input->vals are swapped during
     * execution, need to remember originals for clean up
     */
    DSV* orig_vals         = input->vals;
    DSV* orig_updated_vals = updated_vals;

    WorkerData* data_structs = malloc(num_threads * sizeof(*data_structs));
    pthread_t*  threads      = malloc(num_threads * sizeof(*threads));
    unsigned long iter;
    for (iter = 0; (max_min.max - max_min.min) / max_min.max > epsilon; ++iter, max_min = getMaxMin(input)) {
        /**
         * For each thread
         */
        for (Count tid = 0; tid < num_threads; ++tid) {
            data_structs[tid].input        = input;
            data_structs[tid].affect_rate  = affect_rate;
            data_structs[tid].epsilon      = epsilon;
            data_structs[tid].tid          = tid;
            data_structs[tid].num_threads  = num_threads;
            data_structs[tid].vals         = input->vals;
            data_structs[tid].updated_vals = updated_vals;
            data_structs[tid].max_min      = NULL;
            pthread_create(&threads[tid], NULL, &worker, (void*)&data_structs[tid]);
        }
        for (Count tid = 0; tid < num_threads; ++tid) {
            pthread_join(threads[tid], NULL);
        }

        /**
         * Commit updated DSVs
         */
        DSV* temp = input->vals;
        input->vals = updated_vals;
        updated_vals = temp;
    }

    input->vals = orig_vals;
    free(orig_updated_vals);
    free(data_structs);
    free(threads);

    AMROutput result;
    result.affect_rate = affect_rate;
    result.epsilon     = epsilon;
    result.iterations  = iter;
    result.max         = max_min.max;
    result.min         = max_min.min;
    return result;
}

void* worker(void* data) {
    WorkerData* worker_data = (WorkerData*)data;
    Count     num_threads   = worker_data->num_threads;
    Count     tid   = worker_data->tid;
    AMRInput* input = worker_data->input;
    Count     start = worker_data->tid * (input->N / worker_data->num_threads);
    Count     end   = (worker_data->tid == worker_data->num_threads - 1)
        ? input->N
        : (worker_data->tid + 1) * (input->N / worker_data->num_threads);

    float affect_rate  = worker_data->affect_rate;
    DSV*  updated_vals = worker_data->updated_vals;

    /**
     * For each box handled by this thread
     */
    for (Count i = start; i < end; ++i) {
        BoxData* box = &input->boxes[i];
        /**
         * Compute updated DSV
         */
        updated_vals[i] = box->self_overlap * input->vals[i];
        for (Count nhbr = 0; nhbr < box->num_nhbrs; ++nhbr) {
            updated_vals[i] += box->overlaps[nhbr] * input->vals[box->nhbr_ids[nhbr]];
        }
        updated_vals[i] /= box->perimeter;
        updated_vals[i] = input->vals[i] * (1 - affect_rate)
            + updated_vals[i] * affect_rate;
    }

    pthread_exit(NULL);
    return NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cuda_runtime_api.h>
#include <cuda.h>

#include "amr.h"
#include "common.h"
#include "kernel.h"

const char* usage = "\
Usage: amr [affect-rate] [epsilon]\n\
\n\
affect-rate: float value controlling the effect of neighboring boxes\n\
epislon    : float value determining the cutoff for convergence\n";

int main(int argc, char** argv) {
    /**
     * Parse command-line arguments
     */
    if (argc != 3) {
        printf("%s", usage);
        exit(1);
    }
    float affect_rate = strtof(argv[1], NULL);
    float epsilon     = strtof(argv[2], NULL);

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

    AMROutput output = run(input, affect_rate, epsilon);

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
AMROutput run(AMRInput* input, float affect_rate, float epsilon) {
    /**
     * Copy box data to device
     */
    BoxData* copy_of_boxes = malloc(input->N * sizeof(*copy_of_boxes));
    memcpy(copy_of_boxes, input->boxes, input->N * sizeof(*copy_of_boxes));
    for (Count i = 0; i < input->N; ++i) {
        BoxData* box = &input->boxes[i];

        Count* d_nhbr_ids;
        Coord* d_overlaps;
        cudaMalloc((void**)&d_nhbr_ids, box->num_nhbrs * sizeof(*d_nhbr_ids));
        cudaMalloc((void**)&d_overlaps, box->num_nhbrs * sizeof(*d_overlaps));
        cudaMemcpy(d_nhbr_ids, box->nhbr_ids, box->num_nhbrs * sizeof(*d_nhbr_ids), cudaMemcpyHostToDevice);
        cudaMemcpy(d_overlaps, box->overlaps, box->num_nhbrs * sizeof(*d_overlaps), cudaMemcpyHostToDevice);

        copy_of_boxes[i].nhbr_ids = d_nhbr_ids;
        copy_of_boxes[i].overlaps = d_overlaps;
    }

    BoxData* d_boxes;
    cudaMalloc((void**)&d_boxes, input->N * sizeof(*d_boxes));
    cudaMemcpy(d_boxes, copy_of_boxes, input->N * sizeof(*d_boxes), cudaMemcpyHostToDevice);

    DSV* d_current_vals;
    DSV* d_updated_vals;
    cudaMalloc((void**)&d_current_vals, input->N * sizeof(*d_current_vals));
    cudaMalloc((void**)&d_updated_vals, input->N * sizeof(*d_updated_vals));
    cudaMemcpy(d_current_vals, input->vals, input->N * sizeof(*d_current_vals), cudaMemcpyHostToDevice);

    unsigned long* d_iter;
    cudaMalloc((void**)&d_iter, sizeof(*d_iter));

    /**
     * Launch kernel
     */
    int num_blocks    = 1;
    int num_thread_pb = 512;
    launch_kernel(affect_rate, epsilon, num_blocks, num_thread_pb, d_boxes, d_current_vals, d_updated_vals, input->N, d_iter);

    /**
     * Retrieve results
     */
    AMROutput result;

    cudaMemcpy(input->vals, d_current_vals, input->N * sizeof(*d_current_vals), cudaMemcpyDeviceToHost);
    cudaMemcpy(&(result.iterations), d_iter, sizeof(*d_iter), cudaMemcpyDeviceToHost);
    AMRMaxMin max_min = getMaxMin(input->vals, input->N);

    result.max         = max_min.max;
    result.min         = max_min.min;
    result.affect_rate = affect_rate;
    result.epsilon     = epsilon;

    for (Count i = 0; i < input->N; ++i) {
        cudaFree(copy_of_boxes[i].nhbr_ids);
        cudaFree(copy_of_boxes[i].overlaps);
    }
    free(copy_of_boxes);

    cudaFree(d_boxes);
    cudaFree(d_current_vals);
    cudaFree(d_updated_vals);
    cudaFree(d_iter);

    return result;
}

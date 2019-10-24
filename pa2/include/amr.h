#pragma once

#include "common.h"

Count* starts;

/**
 * Computes starting and ending interations
 * so that each thread has roughly equal number of
 * neighbors to handle
 */
void splitByNhbrs(AMRInput* input, Count num_threads) {
    /**
     * Track number of arithmetic ops (+, *, /)
     */
    Count total = 0;
    for (Count i = 0; i < input->N; ++i) {
        total += 2 + 2 * input->boxes[i].num_nhbrs + 4;
    }

    starts[0]         = 0;
    Count curr_id     = 0;
    Count curr_total  = 0;
    Count curr_target = total / num_threads;
    for (Count i = 0; i < input->N - 1; ++i) {
        curr_total += 2 + 2 * input->boxes[i].num_nhbrs + 4;
        if (curr_total >= curr_target) {
            total      -= curr_total;
            curr_target = total / (num_threads - curr_id - 1);

            curr_id        += 1;
            starts[curr_id] = i + 1;
            curr_total      = 0;
        }
    }
    starts[num_threads] = input->N;
}

/**
 * Run Adaptive Mesh Refinement using
 * the given input and parameters.
 * Returns results.
 *
 * @param input pointer to populated {@code AMRInput} struct
 * @return the results in an {@code AMROutput} struct
 */
AMROutput run(AMRInput* input, float affect_rate, float epsilon, Count num_threads);

typedef struct WorkerData {
    AMRInput* input;
    float     affect_rate;
    float     epsilon;
    Count     tid;
    Count     num_threads;
    DSV*      vals;
    DSV*      updated_vals;

    AMRMaxMin* max_min;
    DSV*       priv_max;
    DSV*       priv_min;
} WorkerData;
/**
 * Code run by each thread during single iteration.
 *
 * @param data pointer to {@code WorkerData} struct with thread parameters
 */
void* worker(void* data);

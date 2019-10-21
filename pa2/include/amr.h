#pragma once

#include "common.h"

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
} WorkerData;
/**
 * Code run by each thread during single iteration.
 *
 * @param data pointer to {@code WorkerData} struct with thread parameters
 */
void* worker(void* data);

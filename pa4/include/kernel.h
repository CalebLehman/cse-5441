#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "amr.h"
#include "common.h"

/**
 * Modified from [Stack Overflow answer](https://stackoverflow.com/questions/14038589/what-is-the-canonical-way-to-check-for-errors-using-the-cuda-runtime-api)
 */
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line) {
    if (code != cudaSuccess) {
        fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
        exit(code);
    }
}

void launch_kernel(
    float affect_rate,
    float epsilon,
    int num_blocks,
    int num_thread_pb,
    BoxData* boxes,
    DSV* current_vals,
    DSV* updated_vals,
    Count N,
    unsigned long* h_iter
);

__global__ void maxMinKernel(
    AMRMaxMin* max_min,
    DSV* vals,
    Count N
);

__global__ void kernel(
    float affect_rate,
    float epsilon,
    BoxData* boxes,
    DSV* current_vals,
    DSV* updated_vals,
    Count N,
    unsigned long* h_iter
);

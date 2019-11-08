#pragma once

#include "amr.h"
#include "common.h"

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

__global__ void kernel(
    float affect_rate,
    float epsilon,
    BoxData* boxes,
    DSV* current_vals,
    DSV* updated_vals,
    Count N,
    unsigned long* h_iter
);

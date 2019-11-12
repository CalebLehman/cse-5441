#include <stdio.h> // TODO
#include <unistd.h> // TODO
#include <cuda_runtime_api.h>
#include <cuda.h>

extern "C" {
#include "amr.h"
#include "common.h"
#include "kernel.h"
}

extern "C" {
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
) {
    dim3 temp_dim_grid(1);
    dim3 temp_dim_block(1);

    dim3 dim_grid(num_blocks);
    dim3 dim_block(num_thread_pb);

    AMRMaxMin max_min;
    AMRMaxMin* d_max_min;
    cudaMalloc((void**)&d_max_min, sizeof(*d_max_min));
    maxMinKernel<<<temp_dim_grid,temp_dim_block>>>(d_max_min, current_vals, N);
    cudaMemcpy(&max_min, d_max_min, sizeof(*d_max_min), cudaMemcpyDeviceToHost);

    unsigned long iter;
    for (
        iter = 0;
        (max_min.max - max_min.min) / max_min.max > epsilon;
        ++iter
    ) {
        kernel<<<dim_grid,dim_block>>>(affect_rate, epsilon, boxes, current_vals, updated_vals, N, h_iter);
        cudaDeviceSynchronize();


        /**
         * Commit updated DSVs
         */
        DSV* temp = current_vals;
        current_vals = updated_vals;
        updated_vals = temp;

        maxMinKernel<<<temp_dim_grid,temp_dim_block>>>(d_max_min, current_vals, N);
        cudaMemcpy(&max_min, d_max_min, sizeof(*d_max_min), cudaMemcpyDeviceToHost);
    }

    cudaMemcpy(h_iter, &iter, sizeof(*h_iter), cudaMemcpyHostToDevice);
}
}

__global__ void maxMinKernel(
    AMRMaxMin* max_min,
    DSV* vals,
    Count N
) {
    *max_min = getMaxMin(vals, N);
}

__global__ void kernel(
    float affect_rate,
    float epsilon,
    BoxData* boxes,
    DSV* current_vals,
    DSV* updated_vals,
    Count N,
    unsigned long* h_iter
) {
    int tid         = blockIdx.x * blockDim.x + threadIdx.x;
    int num_threads = blockDim.x * gridDim.x;
    Count start     = tid * (N / num_threads);
    Count end       = (tid == num_threads - 1)
        ? N
        : (tid + 1) * (N / num_threads);

    /**
     * For each box
     */
    for (int i = start; i < end; ++i) {
        BoxData* box = &boxes[i];
        /**
         * Compute updated DSV
         */
        updated_vals[i] = box->self_overlap * current_vals[i];
        for (int nhbr = 0; nhbr < box->num_nhbrs; ++nhbr) {
            updated_vals[i] +=
                box->overlaps[nhbr] * current_vals[box->nhbr_ids[nhbr]];
        }
        updated_vals[i] /= box->perimeter;
        updated_vals[i] = current_vals[i] * (1 - affect_rate)
            + updated_vals[i] * affect_rate;
    }
}

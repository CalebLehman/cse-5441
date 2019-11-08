#include <cuda_runtime_api.h>
#include <cuda.h>

extern "C" {
#include "amr.h"
#include "common.h"
#include "kernel.h"
}

extern "C" {
void launch_kernel(float affect_rate, float epsilon, int num_blocks, int num_thread_pb, BoxData* boxes, DSV* current_vals, DSV* updated_vals, Count N, Count* iter) {
    dim3 dim_grid(num_blocks);
    dim3 dim_block(num_thread_pb);
    kernel<<<dim_grid,dim_block>>>(affect_rate, epsilon, boxes, current_vals, updated_vals, N, iter);
}
}

__global__ void kernel(float affect_rate, float epsilon, BoxData* boxes, DSV* current_vals, DSV* updated_vals, Count N, Count* iter_TODO) {
    AMRMaxMin max_min = getMaxMin(current_vals, N);
    unsigned long iter;
    for (iter = 0; (max_min.max - max_min.min) / max_min.max > epsilon; ++iter, max_min = getMaxMin(current_vals, N)) {
        /**
         * For each box
         */
        for (int i = 0; i < N; ++i) {
            BoxData* box = &boxes[i];
            /**
             * Compute updated DSV
             */
            updated_vals[i] = box->self_overlap * current_vals[i];
            for (int nhbr = 0; nhbr < box->num_nhbrs; ++nhbr) {
                updated_vals[i] += box->overlaps[nhbr] * current_vals[box->nhbr_ids[nhbr]];
            }
            updated_vals[i] /= box->perimeter;
            updated_vals[i] = current_vals[i] * (1 - affect_rate)
                + updated_vals[i] * affect_rate;
        }
        /**
         * Commit updated DSVs
         */
        DSV* temp = current_vals;
        current_vals = updated_vals;
        updated_vals = temp;
    }
}

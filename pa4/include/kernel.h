#pragma once

#include "amr.h"
#include "common.h"

void launch_kernel(int num_blocks, int num_thread_pb, BoxData* boxes, DSV* current_vals, DSV* updated_vals, Count N, Count* iter);

__global__ void kernel(BoxData* boxes, DSV* current_vals, DSV* updated_vals, Count N, Count* iter);

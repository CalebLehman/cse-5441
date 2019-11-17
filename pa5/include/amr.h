#pragma once

#include "common.h"

/**
 * Run Adaptive Mesh Refinement using
 * the given input and parameters.
 * Called by "master" process, which coordinates
 * other processes and collects results.
 * Returns results.
 *
 * @param input pointer to populated {@code AMRInput} struct
 * @param affect_rate value for AMR computation
 * @param epsilon value for AMR computation
 * @return the results in an {@code AMROutput} struct
 */
AMROutput run_master(AMRInput* input, float affect_rate, float epsilon);

/**
 * Run the actual computation.
 * Called by each of the "worker" proceses.
 */
void run_computation();

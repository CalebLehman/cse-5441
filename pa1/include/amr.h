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
AMROutput run(AMRInput* input, float affect_rate, float epsilon);

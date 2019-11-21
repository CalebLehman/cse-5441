#pragma once

#include <time.h>

typedef unsigned int Count;
typedef unsigned int Coord;
typedef double       DSV;
#define COUNT_SPEC "%u"
#define COORD_SPEC "%u"
#define DSV_SPEC "%lf"
#define COUNT_MPI_TYPE MPI_UNSIGNED
#define COORD_MPI_TYPE MPI_UNSIGNED
#define DSV_MPI_TYPE MPI_DOUBLE

typedef struct AMRInput {
    /**
     * General parameters:
     *
     * {@code N}    - number of boxes
     * {@code rows} - number of grid rows
     * {@code cols} - number of grid columns
     */
    Count N;
    Coord rows, cols;

    /**
     * Per-box data
     */
    Coord*   perimeters;
    Count*   num_nhbrs;
    Count*   offsets;
    Coord*   self_overlaps;
    Count    total_nhbrs;
    Count*   nhbr_ids;
    Coord*   overlaps;
    DSV*     vals;
} AMRInput;

/**
 * Parses Adaptive Mesh Refinement input from stdin.
 * Allocates and populates an {@code AMRInput} struct to hold the input.
 * Should be paired with {@code destroyAMRInput}.
 *
 * @return the allocated/populated {@code AMRInput} struct
 */
AMRInput* parseInput();

/**
 * Destroys input created with {@code parseInput()}.
 * Deallocates all allocations from {@code parseInput()}.
 * Should be paired with {@code parseInput()}.
 *
 * @param input pointer to {@code AMRInput} returned by {@code parseInput()}
 */
void destroyInput(AMRInput* input);

typedef struct AMRMaxMin {
    DSV max;
    DSV min;
} AMRMaxMin;
/**
 * Helper function for computing the maximum and minimum DSV
 *
 * @param input pointer to populated {@code AMRInput} struct
 * @return struct with maximum and minimum DSV
 */
static inline AMRMaxMin getMaxMin(AMRInput* input) {
    AMRMaxMin result = { input->vals[0], input->vals[0] };
    for (int i = 1; i < input->N; ++i) {
        DSV val = input->vals[i];
        if (val > result.max) {
            result.max = val;
        } else if (val < result.min) {
            result.min = val;
        }
    }
    return result;
}

typedef struct AMROutput {
    /**
     * General parameters controlling
     * execution of Adaptive Mesh Refinement.
     */
    float affect_rate;
    float epsilon;

    unsigned long iterations;
    DSV           max;
    DSV           min;

    double time_seconds;
    double clock_seconds;
    double gettime_seconds;
} AMROutput;

/**
 * Display results corresponding to given {@code AMROutput}.
 * @param output {@code AMROutput} struct with results of run
 */
void displayOutput(AMROutput output);


/***********************************************
 * Misc. helper functions
 ***********************************************/

/**
 * Helper function for computing the minimum of ints
 */
static inline int min(int a, int b) { return a < b ? a : b; }

/**
 * Helper function for computing the maximum of ints
 */
static inline int max(int a, int b) { return a > b ? a : b; }

static void printDSVs(Count N, DSV* vals) {
    for (int i = 0; i < N; ++i) {
        printf(DSV_SPEC" ", vals[i]);
    }
    printf("\n");
}

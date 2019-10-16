#include <stdio.h>
#include <stdlib.h>

#include "amr.h"
#include "common.h"

const char* usage = "\
Usage: amr [affect-rate] [epsilon]\n\
\n\
affect-rate: float value controlling the effect of neighboring boxes\n\
epislon    : float value determining the cutoff for convergence\n";

int main(int argc, char** argv) {
    /**
     * Parse command-line arguments
     */
    if (argc != 3) {
        printf(usage);
        exit(1);
    }
    float affect_rate = strtof(argv[1], NULL);
    float epsilon     = strtof(argv[2], NULL);

    /**
     * Parse input data from standard input
     */
    AMRInput* input = parseInput();

    /**
     * Run
     */
    AMROutput output = run(input, affect_rate, epsilon);

    printf("%lu\n", output.iterations);
    return 0;
}

/**
 * {@inheritDoc}
 */
AMROutput run(AMRInput* input, float affect_rate, float epsilon) {
    /**
     * Repeat until convergence
     */
    AMRMaxMin max_min = getMaxMin(input);
    DSV* updated_vals = malloc(input->N * sizeof(*updated_vals));
    unsigned long iter;
    for (iter = 0; (max_min.max - max_min.min) / max_min.max > epsilon; ++iter, max_min = getMaxMin(input)) {
        #ifdef DEBUG
        printf("BEGIN ITERATION %lu\n", iter + 1);
        printf("original:\n");
        printDSVs(input->N, input->vals);
        #endif
        /**
         * For each box
         */
        for (int i = 0; i < input->N; ++i) {
            BoxData* box = &input->boxes[i];
            /**
             * Compute updated DSV
             */
            updated_vals[i] = box->self_overlap * input->vals[i];
            for (int nhbr = 0; nhbr < box->num_nhbrs; ++nhbr) {
                updated_vals[i] += box->overlaps[nhbr] * input->vals[box->nhbr_ids[nhbr]];
            }
            updated_vals[i] /= box->perimeter;
            updated_vals[i] = input->vals[i] * (1 - affect_rate)
                + updated_vals[i] * affect_rate;
        }
        #ifdef DEBUG
        printf("updated:\n");
        printDSVs(input->N, updated_vals);
        #endif

        /**
         * Commit updated DSVs
         */
        DSV* temp = input->vals;
        input->vals = updated_vals;
        updated_vals = temp;
    }
    free(updated_vals);

    AMROutput result;
    result.affect_rate = affect_rate;
    result.epsilon     = epsilon;
    result.iterations  = iter;
    result.max         = max_min.max;
    result.min         = max_min.min;
    return result;
}

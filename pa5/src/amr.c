#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#include "amr.h"
#include "common.h"

typedef enum tag {ar_tag, ep_tag, n_tag, dsv_tag, pos_tag} tag;

const char* usage = "\
Usage: amr [affect-rate] [epsilon]\n\
\n\
affect-rate: float value controlling the effect of neighboring boxes\n\
epislon    : float value determining the cutoff for convergence\n";

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        /**
         * Parse command-line arguments
         */
        if (argc != 3) {
            printf("%s", usage);
            exit(1);
        }
        float affect_rate = strtof(argv[1], NULL);
        float epsilon     = strtof(argv[2], NULL);

        /**
         * Parse input data from standard input
         */
        AMRInput* input = parseInput();

        /**
         * Run and collect timing information
         */
        time_t  time_before;
        time(&time_before);
        clock_t clock_before = clock();
        struct timespec gettime_before;
        clock_gettime(CLOCK_REALTIME, &gettime_before);

        AMROutput output = run_master(input, affect_rate, epsilon);

        time_t time_after;
        time(&time_after);
        time_t clock_after = clock();
        struct timespec gettime_after;
        clock_gettime(CLOCK_REALTIME, &gettime_after);

        output.time_seconds  = difftime(time_after, time_before);
        output.clock_seconds = (clock_after - clock_before) / (double) CLOCKS_PER_SEC;
        output.gettime_seconds = (double) (
            (gettime_after.tv_sec - gettime_before.tv_sec) +
            ((gettime_after.tv_nsec - gettime_before.tv_nsec) / 1000000000.0)
        );

        /**
         * Display results
         */
        displayOutput(output);

        /**
         * Clean up
         */
        destroyInput(input);
    } else {
        /**
         * Non-master processes just wait for master to send work
         */
        run_computation();
    }

    MPI_Finalize();
    return 0;
}

/**
 * {@inheritDoc}
 */
AMROutput run_master(AMRInput* input, float affect_rate, float epsilon) {
    /**
     * Repeat until convergence
     */
    AMRMaxMin max_min = getMaxMin(input);

    /**
     * TODO
     */
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int rank = 1; rank < size; ++rank) {
        MPI_Send(&affect_rate, 1, MPI_FLOAT, rank, ar_tag, MPI_COMM_WORLD);
        MPI_Send(&epsilon, 1, MPI_FLOAT, rank, ep_tag, MPI_COMM_WORLD);
        MPI_Send(&input->N, 1, COUNT_MPI_TYPE, rank, n_tag, MPI_COMM_WORLD);

        Count pos[2];
        pos[0] = (rank-1) * (input->N / (size - 1));
        pos[1] = (rank == size - 1)
            ? input->N
            : rank * (input->N / (size - 1));
        MPI_Send(&pos[0], 2, COUNT_MPI_TYPE, rank, pos_tag, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);



    DSV* updated_vals = malloc(input->N * sizeof(*updated_vals));

    /**
     * updated_vals and input->vals are swapped during
     * execution, need to remember originals for clean up
     */
    DSV* orig_vals         = input->vals;
    DSV* orig_updated_vals = updated_vals;

    unsigned long iter;
    for (iter = 0; (max_min.max - max_min.min) / max_min.max > epsilon; ++iter, max_min = getMaxMin(input)) {
        #if (PRINT_DSVS != 0)
        printf("BEGIN ITERATION %lu\n", iter + 1);
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

        /**
         * Commit updated DSVs
         */
        DSV* temp = input->vals;
        input->vals = updated_vals;
        updated_vals = temp;
    }

    input->vals = orig_vals;
    free(orig_updated_vals);

    AMROutput result;
    result.affect_rate = affect_rate;
    result.epsilon     = epsilon;
    result.iterations  = iter;
    result.max         = max_min.max;
    result.min         = max_min.min;
    return result;
}

void run_computation() {
    MPI_Status status;

    float affect_rate;
    MPI_Recv(&affect_rate, 1, MPI_FLOAT, 0, ar_tag, MPI_COMM_WORLD, &status);

    float epsilon;
    MPI_Recv(&epsilon, 1, MPI_FLOAT, 0, ep_tag, MPI_COMM_WORLD, &status);

    Count N;
    MPI_Recv(&N, 1, COUNT_MPI_TYPE, 0, n_tag, MPI_COMM_WORLD, &status);
    DSV* vals         = malloc(N * sizeof(*vals));
    DSV* updated_vals = malloc(N * sizeof(*updated_vals));
    Count pos[2];
    MPI_Recv(&pos[0], 2, COUNT_MPI_TYPE, 0, pos_tag, MPI_COMM_WORLD, &status);
    Count start = pos[0];
    Count end   = pos[1];

    printf("Got ar = %f and ep = %f and N = "COUNT_SPEC" and start = "COUNT_SPEC" and end = "COUNT_SPEC"\n", affect_rate, epsilon, N, start, end);

    MPI_Barrier(MPI_COMM_WORLD);
}

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

const char* invalid_format = "Error: invalid input\n";

/**
 * Struct to store minimially-processed input
 * for box data
 */
#define NUM_DIR 4
typedef enum { TOP=0, BOTTOM, LEFT, RIGHT } DIRECTION;
typedef struct BoxInput {
    Coord x_min, x_max, y_min, y_max;
    Coord perimeter;

    /**
     * [0] = [TOP] corresponds to top neighbors
     * [1] = [BOTTOM] corresponds to bottom neighbors
     * [2] = [LEFT] corresponds to left neighbors
     * [3] = [RIGHT] corresponds to right neighbors
     */
    Count num_nhbrs[NUM_DIR];
    Count* nhbr_ids[NUM_DIR];
} BoxInput;

/**
 * {@inheritDoc}
 */
AMRInput* parseInput() {
    /**
     * Allocate struct
     */
    AMRInput* input = malloc(sizeof(*input));

    /**
     * Read in general parameters
     */
    if (scanf(COUNT_SPEC, &input->N) != 1) {
        fprintf(stderr, "%s", invalid_format);
        exit(1);
    }
    if (scanf(COORD_SPEC, &input->rows) != 1) {
        fprintf(stderr, "%s", invalid_format);
        exit(1);
    }
    if (scanf(COORD_SPEC, &input->cols) != 1) {
        fprintf(stderr, "%s", invalid_format);
        exit(1);
    }

    /**
     * Allocate arrays for per-box values
     */
    BoxInput* bs = malloc(input->N * sizeof(*bs));
    input->vals  = malloc(input->N * sizeof(*input->vals));

    /**
     * Reading in per-box information
     */
    for (int i = 0; i < input->N; ++i) {
        BoxInput* box_input = &bs[i];

        /**
         * Verify that ids are sequential
         */
        Count id;
        if (scanf(COUNT_SPEC, &id) != 1) {
            fprintf(stderr, "%s", invalid_format);
            exit(1);
        }
        #ifdef DEBUG
        if (id != i) {
            fprintf(stderr, "%s", invalid_format);
            exit(1);
        }
        #endif

        /**
         * Size and position information
         */
        Coord y, x, height, width;
        if (scanf(COORD_SPEC COORD_SPEC COORD_SPEC COORD_SPEC,
                  &y,        &x,        &height,   &width)
            != 4) {
            fprintf(stderr, "%s", invalid_format);
            exit(1);
        }
        box_input->x_min     = x;
        box_input->x_max     = x + width;
        box_input->y_min     = y;
        box_input->y_max     = y + height;
        box_input->perimeter = 2 * (height + width);

        /**
         * Topology information
         */
        for (DIRECTION dir = TOP; dir <= RIGHT; ++dir) {
            Count num_nhbrs;
            if (scanf(COUNT_SPEC, &num_nhbrs) != 1) {
                fprintf(stderr, "%s", invalid_format);
                exit(1);
            }
            box_input->num_nhbrs[dir] = num_nhbrs;
            box_input->nhbr_ids[dir]  = malloc(
                num_nhbrs * sizeof(*box_input->nhbr_ids[dir])
            );
            for (int nhbr = 0; nhbr < num_nhbrs; ++nhbr) {
                if (scanf(COUNT_SPEC, &box_input->nhbr_ids[dir][nhbr]) != 1) {
                    fprintf(stderr, "%s", invalid_format);
                    exit(1);
                }
            }
        }

        /**
         * DSV information
         */
        if (scanf(DSV_SPEC, &input->vals[i]) != 1) {
            fprintf(stderr, "%s", invalid_format);
            exit(1);
        }
    }

    /**
     * Processes box data (currently stored in {@code bs})
     * to a more convenient format (to be stored in {@code boxes}).
     */
    input->boxes = malloc(input->N * sizeof(*input->boxes));
    for (int i = 0; i < input->N; ++i) {
        BoxData*  box_data  = &input->boxes[i];
        BoxInput* box_input = &bs[i];

        box_data->perimeter = box_input->perimeter;
        box_data->id        = i;

        box_data->num_nhbrs = 0;
        for (DIRECTION dir = TOP; dir <= RIGHT; ++dir) {
            box_data->num_nhbrs += box_input->num_nhbrs[dir];
        }

        box_data->nhbr_ids = malloc(
            box_data->num_nhbrs * sizeof(*box_data->nhbr_ids)
        );
        box_data->overlaps = malloc(
            box_data->num_nhbrs * sizeof(*box_data->overlaps)
        );
        
        box_data->self_overlap = box_data->perimeter;
        Count total_nhbr = 0;
        for (int nhbr = 0;
             nhbr < box_input->num_nhbrs[TOP];
             ++nhbr, ++total_nhbr
        ) {
            Count nhbr_id = box_input->nhbr_ids[TOP][nhbr];
            box_data->nhbr_ids[total_nhbr] = nhbr_id;
            Coord x_max = min(box_input->x_max, bs[nhbr_id].x_max);
            Coord x_min = max(box_input->x_min, bs[nhbr_id].x_min);
            box_data->overlaps[total_nhbr] = x_max - x_min;
            box_data->self_overlap -= x_max - x_min;
        }
        for (int nhbr = 0;
             nhbr < box_input->num_nhbrs[BOTTOM];
             ++nhbr, ++total_nhbr
        ) {
            Count nhbr_id = box_input->nhbr_ids[BOTTOM][nhbr];
            box_data->nhbr_ids[total_nhbr] = nhbr_id;
            Coord x_max = min(box_input->x_max, bs[nhbr_id].x_max);
            Coord x_min = max(box_input->x_min, bs[nhbr_id].x_min);
            box_data->overlaps[total_nhbr] = x_max - x_min;
            box_data->self_overlap -= x_max - x_min;
        }
        for (int nhbr = 0;
             nhbr < box_input->num_nhbrs[LEFT];
             ++nhbr, ++total_nhbr
        ) {
            Count nhbr_id = box_input->nhbr_ids[LEFT][nhbr];
            box_data->nhbr_ids[total_nhbr] = nhbr_id;
            Coord y_max = min(box_input->y_max, bs[nhbr_id].y_max);
            Coord y_min = max(box_input->y_min, bs[nhbr_id].y_min);
            box_data->overlaps[total_nhbr] = y_max - y_min;
            box_data->self_overlap -= y_max - y_min;
        }
        for (int nhbr = 0;
             nhbr < box_input->num_nhbrs[RIGHT];
             ++nhbr, ++total_nhbr
        ) {
            Count nhbr_id = box_input->nhbr_ids[RIGHT][nhbr];
            box_data->nhbr_ids[total_nhbr] = nhbr_id;
            Coord y_max = min(box_input->y_max, bs[nhbr_id].y_max);
            Coord y_min = max(box_input->y_min, bs[nhbr_id].y_min);
            box_data->overlaps[total_nhbr] = y_max - y_min;
            box_data->self_overlap -= y_max - y_min;
        }
    }

    /**
     * Clean up
     */
    for (int i = 0; i < input->N; ++i) {
        BoxInput* box_input = &bs[i];
        for (DIRECTION dir = TOP; dir <= RIGHT; ++dir) {
            free(box_input->nhbr_ids[dir]);
        }
    }
    free(bs);

    return input;
}

/**
 * {@inheritDoc}
 */
void destroyInput(AMRInput* input) {
    for (int i = 0; i < input->N; ++i) {
        free(input->boxes[i].nhbr_ids);
        free(input->boxes[i].overlaps);
    }
    free(input->boxes);
    free(input->vals);
    free(input);
}

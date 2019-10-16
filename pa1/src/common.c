#include <stdio.h>
#include <stdlib.h>

#include "common.h"

/**
 * Struct to store minimially-processed input
 * for box data
 */
const Count NUM_DIR = 4;
typedef enum { TOP=0, BOTTOM, LEFT, RIGHT } DIRECTION;
typedef struct BoxInput {
    Coord x_min, x_max, y_min, y_max;
    Coord perimeter;

    /**
     * [0] - top neighbors
     * [1] - bottom neighbors
     * [2] - left neighbors
     * [3] - right neighbors
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
    scanf(COUNT_SPEC, &input->N);
    scanf(COORD_SPEC, &input->rows);
    scanf(COORD_SPEC, &input->cols);

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
         * Size and position information
         */
        Coord y, x, height, width;
        scanf(COORD_SPEC COORD_SPEC COORD_SPEC COORD_SPEC,
              &y,        &x,        &height,   &width);
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
            scanf(COUNT_SPEC, &num_nhbrs);
            box_input->num_nhbrs[dir] = num_nhbrs;
            box_input->nhbr_ids[dir]  = malloc(
                num_nhbrs * sizeof(*box_input->nhbr_ids[dir])
            );
            for (int nhbr = 0; nhbr < num_nhbrs; ++nhbr) {
                scanf(COUNT_SPEC, &box_input->nhbr_ids[dir][nhbr]);
            }
        }

        /**
         * DSV information
         */
        scanf(DSV_SPEC, &input->vals[i]);
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

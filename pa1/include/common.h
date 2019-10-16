typedef unsigned int Count;
typedef unsigned int Coord;
typedef double       DSV;
#define COUNT_SPEC "%u"
#define COORD_SPEC "%u"
#define DSV_SPEC "%lf"

typedef struct BoxData {
    Coord perimeter;
    Count id;

    Count  num_nhbrs;
    Count* nhbr_ids;
    Coord* overlaps;

    /**
     * Unnecessary, since boxes currently
     * aren't adaptive
     */
    //Coord x;
    //Coord y;
    //Coord height;
    //Coord width;
} BoxData;

typedef struct AMRInput {
    /**
     * General parameters:
     *
     * N    - number of boxes
     * rows - number of grid rows
     * cols - number of grid columns
     */
    Count N;
    Coord rows, cols;

    /**
     * Per-box data
     */
    BoxData* boxes;
    DSV*     vals;
} AMRInput;

/**
 * Parses Adaptive Mesh Refinement (AMR) input from stdin.
 * Allocates and initializes an AMRInput struct to hold the input.
 * Should be paired with {@code destroyAMRInput}.
 *
 * @return the AMRInput struct
 */
AMRInput* parseInput();

/**
 * Helper function for computing the minimum of int
 */
static inline int min(int a, int b) { return a < b ? a : b; }

/**
 * Helper function for computing the maximum of int
 */
static inline int max(int a, int b) { return a > b ? a : b; }

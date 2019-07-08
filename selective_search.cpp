#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "args.h"
#include "segmentation/segment.h"

void help(char *exe) {
    printf("Usage: %s input output [sigma] [k] [min_size]\n", exe);
}

int main(int argc, char **argv) {
    if (parse_args(argc, argv)) {
        help(argv[0]);
        return -1;
    }
    universe *u = segment(args.sigma, args.k, args.min_size, args.input, args.output);
    printf("Segment found %d components\n", u->num_sets());

    return 0;
}
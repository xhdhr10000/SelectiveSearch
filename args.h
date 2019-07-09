#include <cstdlib>
#include <cstring>

struct ARGS {
    char *input;
    char *output;
    float sigma;
    float k;
    int min_size;
} args;

int parse_args(int argc, char **argv) {
    if (argc < 3) return -1;
    args.input      = argv[1];
    args.output     = argv[2];
    args.sigma      = argc > 3 ? atof(argv[3]) : 0.5;
    args.k          = argc > 4 ? atof(argv[4]) : 200.0;
    args.min_size   = argc > 5 ? atoi(argv[5]) : 400;
    return 0;
}
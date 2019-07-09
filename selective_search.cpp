#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <queue>
#include "segmentation/image.h"
#include "segmentation/misc.h"
#include "segmentation/pnmfile.h"
#include "segmentation/segment-image.h"
#include "args.h"
#include "common.h"
#include "histogram.h"
#include "similarity.h"

using namespace std;

void help(char *exe) {
    printf("Usage: %s input output [sigma] [k] [min_size]\n", exe);
}

vector<Region>* group_segments(universe *u, image<rgb> *im) {
    int size = im->width() * im->height();
    int rSize = u->num_sets() * 2;
    vector<Region> *R = new vector<Region>();
    bool *hash = new bool[size];

    memset(hash, 0, sizeof(bool)*size);
    image<frgb> **gradients = calTextureGradients(im);

    /* Save initial regions to R */
    for (int i=0; i<size; i++) {
        int c = u->find(i, true);
        if (hash[c]) continue;
        hash[c] = true;

        Region r = Region();
        r.box = u->bbox(c);
        r.size = u->size(c);
        /* Calculate color hist for each region */
        calColorHist(r, u, c, im);
        calTextureHist(r, u, c, gradients);
        R->push_back(r);
    }

    /* Calculate similarities */
    printf("\tCalculating similarities..."); fflush(stdout);
    priority_queue<Similarity> *S = new priority_queue<Similarity>();
    float min = 0x7fffffff, max = -0x7fffffff;
    for (int i=1; i<R->size(); i++) {
        for (int j=0; j<i-1; j++) {
            Similarity s(i, j, similarity(R->at(i), R->at(j), im, true, false, false, false));
            min = MIN(min, s.similarity);
            max = MAX(max, s.similarity);
            S->push(s);
        }
    }
    printf("done. min=%f max=%f count %lu\n", min, max, S->size());

    // TODO
    // printf("\tGrouping regions..."); fflush(stdout);
    // while (S->size()) {
    // }

    delete [] gradients;
    delete [] hash;
    delete S;
    return R;
}

image<rgb> *render_ssearch_image(image<rgb> *segment_output, universe *u, int width, int height) {
    image<rgb> *output = segment_output->copy();
    rgb red = {255, 0, 0};
    bool *hash = new bool[width*height];

    memset(hash, 0, sizeof(bool)*width*height);
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            int c = u->find(y*width+x, true);
            if (hash[c]) continue;
            hash[c] = true;
            BBox bbox = u->bbox(c);
            for (int col=bbox.left; col<=bbox.right; col++) {
                imRef(output, col, bbox.top) = red;
                imRef(output, col, bbox.bottom) = red;
            }
            for (int row=bbox.top; row<=bbox.bottom; row++) {
                imRef(output, bbox.left, row) = red;
                imRef(output, bbox.right, row) = red;
            }
        }
    }

    delete [] hash;
    return output;
}

int main(int argc, char **argv) {
    if (parse_args(argc, argv)) {
        help(argv[0]);
        return -1;
    }

    printf("Segmentation begin..."); fflush(stdout);
    image<rgb> *input = loadPPM(args.input);
    universe *u = segment_image(input, args.sigma, args.k, args.min_size); 
    printf("found %d components\n", u->num_sets());

    char *output_segment_file = strcat(strtok(strdup(args.output), "."), "_seg.ppm");
    image<rgb> *output_seg = render_segment_image(u, input->width(), input->height());
    savePPM(output_seg, output_segment_file);

    printf("Grouping begin...\n");
    vector<Region> *R = group_segments(u, input);
    printf("found %lu regions\n", R->size());

    image<rgb> *output = render_ssearch_image(input, u, input->width(), input->height());
    savePPM(output, args.output);

    delete u;
    return 0;
}
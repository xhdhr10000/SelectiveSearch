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
    printf("    Calculating similarities..."); fflush(stdout);
    priority_queue<Similarity> *S = new priority_queue<Similarity>();
    float min = 0x7fffffff, max = -0x7fffffff;
    for (int i=1; i<R->size(); i++) {
        for (int j=0; j<i-1; j++) {
            Similarity s(i, j, similarity(R->at(i), R->at(j), im));
            min = MIN(min, s.similarity);
            max = MAX(max, s.similarity);
            S->push(s);
        }
    }
    printf("done. min=%f max=%f count %lu\n", min, max, S->size());
    delete [] hash;

    int rSize = u->num_sets() * 2;
    hash = new bool[rSize];
    memset(hash, 0, sizeof(bool)*rSize);
    for (int i=0; i<R->size(); i++) hash[i] = true;

    printf("    Grouping regions..."); fflush(stdout);
    while (S->size()) {
        Similarity s = S->top();
        S->pop();
        if (!hash[s.r1] || !hash[s.r2]) continue;
        hash[s.r1] = hash[s.r2] = false;

        Region r1 = R->at(s.r1), r2 = R->at(s.r2);
        Region rt = Region();
        rt.box = BBox::merge(r1.box, r2.box);
        rt.size = r1.size + r2.size;
        for (int i=0; i<CHANNELS*COLOR_HIST_SIZE; i++)
            rt.colorHistogram[i] = (r1.colorHistogram[i]*r1.size + r2.colorHistogram[i]*r2.size) / rt.size;
        for (int i=0; i<CHANNELS*TEXTURE_HIST_DIRS*TEXTURE_HIST_SIZE; i++)
            rt.textureHistogram[i] = (rt.textureHistogram[i]*r1.size + r2.textureHistogram[i]*r2.size) / rt.size;
        R->push_back(rt);
        hash[R->size()-1] = true;
        for (int i=0; i<R->size()-1; i++) {
            if (!hash[i]) continue;
            Similarity s(R->size()-1, i, similarity(R->back(), R->at(i), im));
            S->push(s);
        }
    }
    printf("done. Total %lu regions\n", R->size());

    delete [] gradients;
    delete [] hash;
    delete S;
    return R;
}

image<rgb> *render_ssearch_image(image<rgb> *im, vector<Region> *R, int width, int height) {
    image<rgb> *output = im->copy();

    for (int i=0; i<R->size(); i++) {
        BBox bbox = R->at(i).box;
        rgb color = random_rgb();
        for (int col=bbox.left; col<=bbox.right; col++) {
            imRef(output, col, bbox.top) = color;
            imRef(output, col, bbox.bottom) = color;
        }
        for (int row=bbox.top; row<=bbox.bottom; row++) {
            imRef(output, bbox.left, row) = color;
            imRef(output, bbox.right, row) = color;
        }
    }

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

    image<rgb> *output = render_ssearch_image(input, R, input->width(), input->height());
    savePPM(output, args.output);

    delete u;
    return 0;
}
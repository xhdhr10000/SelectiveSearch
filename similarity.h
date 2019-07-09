#include <cstdio>
#include "common.h"
#include "segmentation/image.h"
#include "segmentation/misc.h"

struct Similarity {
    int r1, r2;
    float similarity;

    Similarity() : r1(0), r2(0), similarity(0.0f) {}
    Similarity(const Similarity &s) : r1(s.r1), r2(s.r2), similarity(s.similarity) {}
    Similarity(int r1, int r2, float similarity=0.0f) : r1(r1), r2(r2), similarity(similarity) {}
    bool operator<(const Similarity &s) const { return this->similarity < s.similarity; }
    void operator=(const Similarity &s) {
        r1 = s.r1;
        r2 = s.r2;
        similarity = s.similarity;
    }
};

float similarity(Region r1, Region r2, image<rgb> *im, bool color=true, bool texture=true, bool size=true, bool fill=true);
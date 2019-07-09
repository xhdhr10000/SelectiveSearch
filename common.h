#ifndef __COMMON_H__
#define __COMMON_H__

#include "segmentation/disjoint-set.h"

#define CHANNELS 3
#define COLOR_HIST_SIZE 25
#define TEXTURE_HIST_DIRS 8
#define TEXTURE_HIST_SIZE 10
#define KERNEL_SIZE 3

typedef struct {
    BBox box;
    int size;
    float colorHistogram[CHANNELS * COLOR_HIST_SIZE];
    float textureHistogram[CHANNELS * TEXTURE_HIST_DIRS * TEXTURE_HIST_SIZE];

    float& colorHist(int c, int index) { return colorHistogram[c*COLOR_HIST_SIZE + index]; }
    float& textureHist(int c, int dir, int index) { return textureHistogram[c*TEXTURE_HIST_DIRS*TEXTURE_HIST_SIZE + dir*TEXTURE_HIST_SIZE + index]; }
} Region;

#endif
#include <cstdlib>
#include "similarity.h"

float simColor(Region r1, Region r2) {
    float s = 0.0f;
    for (int i=0; i<CHANNELS*COLOR_HIST_SIZE; i++) s += MIN(r1.colorHistogram[i], r2.colorHistogram[i]);
    return s;
}

float simTexture(Region r1, Region r2) {
    float s = 0.0f;
    for (int i=0; i<CHANNELS*TEXTURE_HIST_DIRS*TEXTURE_HIST_SIZE; i++) s += MIN(r1.textureHistogram[i], r2.textureHistogram[i]);
    return s;
}

float simSize(Region r1, Region r2, int size) {
    return 1 - (r1.size + r2.size) / size;
}

float simFill(Region r1, Region r2, int size) {
    BBox box = { MIN(r1.box.top,r2.box.top), MAX(r1.box.right,r2.box.right), MAX(r1.box.bottom,r2.box.bottom), MIN(r1.box.left,r2.box.left) };
    return 1 - (box.area() - r1.size - r2.size) / size;
}

float similarity(Region r1, Region r2, image<rgb> *im, bool color, bool texture, bool size, bool fill) {
    return (color   ? simColor(r1,r2) : 0) +
           (texture ? simTexture(r1,r2) : 0) +
           (size    ? simSize(r1,r2,im->width()*im->height()) : 0) +
           (fill    ? simFill(r1,r2,im->width()*im->height()) : 0);
}
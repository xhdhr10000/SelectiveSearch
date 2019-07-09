#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include "segmentation/image.h"
#include "segmentation/misc.h"
#include "common.h"

image<frgb> **calTextureGradients(image<rgb> *im);
void calColorHist(Region &region, universe *u, int c, image<rgb> *im);
void calTextureHist(Region &region, universe *u, int c, image<frgb> **gradients);

#endif
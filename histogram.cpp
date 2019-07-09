#include <cmath>
#include "histogram.h"

void l1Norm(float *hist, int size) {
    float total = 0.0f;
    for (int i=0; i<size; i++) total += fabs(hist[i]);
    for (int i=0; i<size; i++) hist[i] /= total;
}

float *gaussianKernel(float angle) {
    const float sigma = 1.0f;
    float gx[KERNEL_SIZE][KERNEL_SIZE], gy[KERNEL_SIZE][KERNEL_SIZE];
    float *g = new float[KERNEL_SIZE * KERNEL_SIZE];

    for (int x=-1; x<=1; x++) {
        for (int y=-1; y<=1; y++) {
            int i=y+1, j=x+1;
            gx[i][j] = -x/(2*M_PI*powf(sigma,4)) * expf(-(x*x+y*y) / (2*sigma*sigma));
            gy[i][j] = -y/(2*M_PI*powf(sigma,4)) * expf(-(x*x+y*y) / (2*sigma*sigma));
            g [i*KERNEL_SIZE + j] = gx[i][j]*cosf(angle) + gy[i][j]*sinf(angle);
        }
    }
    return g;
}

image<frgb> *convolve(float *kernel, image<rgb> *im) {
    int size = im->width() * im->height();
    image<frgb> *ret = new image<frgb>(im->width(), im->height());
    float max = 0;
    for (int row=0; row<im->height(); row++) {
        for (int col=0; col<im->width(); col++) {
            for (int i=0; i<KERNEL_SIZE; i++) {
                int x = col+i-1;
                if (x < 0 || x >= im->width()) continue;
                for (int j=0; j<KERNEL_SIZE; j++) {
                    int y = row+j-1;
                    if (y < 0 || y >= im->height()) continue;
                    imRef(ret, col, row).r += kernel[i*KERNEL_SIZE+j] * imRef(im, x, y).r;
                    imRef(ret, col, row).g += kernel[i*KERNEL_SIZE+j] * imRef(im, x, y).g;
                    imRef(ret, col, row).b += kernel[i*KERNEL_SIZE+j] * imRef(im, x, y).b;
                }
            }
            if (imRef(ret, col, row).r > max) max = imRef(ret, col, row).r;
            if (imRef(ret, col, row).g > max) max = imRef(ret, col, row).g;
            if (imRef(ret, col, row).b > max) max = imRef(ret, col, row).b;
        }
    }
    return ret;
}

image<frgb> **calTextureGradients(image<rgb> *im) {
    float angles[8] = { 0, M_PI/4, M_PI/2, M_PI*3/4, M_PI, M_PI*5/4, M_PI*3/2, M_PI*7/4 };
    image<frgb> **gs = new image<frgb>*[8];
    for (int a=0; a<8; a++) {
        float *k = gaussianKernel(angles[a]);
        gs[a] = convolve(k, im);
        delete [] k;
    }
    return gs;
}

void calColorHist(Region &region, universe *u, int c, image<rgb> *im) {
    BBox box = u->bbox(c);
    const float binSize = 255.0f / COLOR_HIST_SIZE + 0.00001f;
    memset(region.colorHistogram, 0, sizeof(float)*CHANNELS*COLOR_HIST_SIZE);
    for (int row=box.top; row<=box.bottom; row++) {
        for (int col=box.left; col<=box.right; col++) {
            if (u->find(row*im->width() + col, true) != c) continue;
            region.colorHist(0, (int)(imRef(im, col, row).r/binSize))++;
            region.colorHist(1, (int)(imRef(im, col, row).g/binSize))++;
            region.colorHist(2, (int)(imRef(im, col, row).b/binSize))++;
        }
    }

    l1Norm(region.colorHistogram, COLOR_HIST_SIZE);
    l1Norm(&region.colorHistogram[COLOR_HIST_SIZE], COLOR_HIST_SIZE);
    l1Norm(&region.colorHistogram[COLOR_HIST_SIZE*2], COLOR_HIST_SIZE);
}

void calTextureHist(Region region, universe *u, int c, image<frgb> **im) {
    BBox box = u->bbox(c);
    const float binSize = 110.0f / TEXTURE_HIST_SIZE + 0.00001f;
    const int binOffset = TEXTURE_HIST_SIZE / 2;
    memset(region.textureHistogram, 0, sizeof(float)*CHANNELS*TEXTURE_HIST_DIRS*TEXTURE_HIST_SIZE);
    for (int dir=0; dir<TEXTURE_HIST_DIRS; dir++) {
        for (int row=box.top; row<=box.bottom; row++) {
            for (int col=box.left; col<=box.right; col++) {
                if (u->find(row*im[dir]->width() + col, true) != c) continue;
                region.textureHist(0, dir, (int)(imRef(im[dir], col, row).r/binSize)+binOffset)++;
                region.textureHist(1, dir, (int)(imRef(im[dir], col, row).g/binSize)+binOffset)++;
                region.textureHist(2, dir, (int)(imRef(im[dir], col, row).b/binSize)+binOffset)++;
            }
        }
        l1Norm(&region.textureHist(0, dir, 0), TEXTURE_HIST_SIZE);
        l1Norm(&region.textureHist(1, dir, 0), TEXTURE_HIST_SIZE);
        l1Norm(&region.textureHist(2, dir, 0), TEXTURE_HIST_SIZE);
    }
}

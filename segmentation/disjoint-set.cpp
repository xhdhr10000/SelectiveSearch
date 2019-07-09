#include "disjoint-set.h"
#include "misc.h"

universe::universe(int width, int height) {
  int elements = width * height;
  elts = new uni_elt[elements];
  num = elements;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int i = y * width + x;
      elts[i].rank = 0;
      elts[i].size = 1;
      elts[i].p = i;
      elts[i].bbox.top = elts[i].bbox.bottom = y;
      elts[i].bbox.left = elts[i].bbox.right = x;
    }
  }
}
  
universe::~universe() {
  delete [] elts;
}

int universe::find(int x, bool keep_structure) {
  int y = x;
  while (y != elts[y].p)
    y = elts[y].p;
  if (!keep_structure) elts[x].p = y;
  return y;
}

void universe::join(int x, int y) {
  if (elts[x].rank > elts[y].rank) {
    elts[y].p = x;
    elts[x].size += elts[y].size;
    elts[x].bbox.top = MIN(elts[x].bbox.top, elts[y].bbox.top);
    elts[x].bbox.right = MAX(elts[x].bbox.right, elts[y].bbox.right);
    elts[x].bbox.bottom = MAX(elts[x].bbox.bottom, elts[y].bbox.bottom);
    elts[x].bbox.left = MIN(elts[x].bbox.left, elts[y].bbox.left);
  } else {
    elts[x].p = y;
    elts[y].size += elts[x].size;
    elts[y].bbox.top = MIN(elts[x].bbox.top, elts[y].bbox.top);
    elts[y].bbox.right = MAX(elts[x].bbox.right, elts[y].bbox.right);
    elts[y].bbox.bottom = MAX(elts[x].bbox.bottom, elts[y].bbox.bottom);
    elts[y].bbox.left = MIN(elts[x].bbox.left, elts[y].bbox.left);
    if (elts[x].rank == elts[y].rank)
      elts[y].rank++;
  }
  num--;
}

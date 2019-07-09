/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#ifndef DISJOINT_SET
#define DISJOINT_SET

#include "misc.h"
// disjoint-set forests using union-by-rank and path compression (sort of).

struct BBox {
  int top, right, bottom, left;

  BBox() {}
  BBox(int top, int right, int bottom, int left) : top(top), right(right), bottom(bottom), left(left) {}
  int area() { return (bottom-top+1) * (right-left+1); }
  static BBox merge(BBox b1, BBox b2) { return BBox(MIN(b1.top,b2.top), MAX(b1.right,b2.right), MAX(b1.bottom,b2.bottom), MIN(b1.left,b2.left)); }
};

typedef struct {
  int rank;
  int p;
  int size;
  BBox bbox;
} uni_elt;

class universe {
public:
  universe(int width, int height);
  ~universe();
  int find(int x, bool keep_structure=false);
  void join(int x, int y);
  int size(int x) const { return elts[x].size; }
  BBox bbox(int x) { return elts[x].bbox; }
  int num_sets() const { return num; }

private:
  uni_elt *elts;
  int num;
};

#endif

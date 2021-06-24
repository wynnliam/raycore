// Liam Wynn, 6/24/2021, Raycore

#include "scalar.h"

void scale_to_11(sdata* data) {
  unsigned int* dst = data->dst;
  unsigned int* src = data->src;
  const int dr = data->dr;
  const int dc = data->dc;
  const int tc = data->tc;
  const int th = data->th;

  dst[(0 + dr) * 320 + dc] = src[(0 << 6) + tc];
  dst[(1 + dr) * 320 + dc] = src[(5 << 6) + tc];
  dst[(2 + dr) * 320 + dc] = src[(11 << 6) + tc];
  dst[(3 + dr) * 320 + dc] = src[(17 << 6) + tc];
  dst[(4 + dr) * 320 + dc] = src[(23 << 6) + tc];
  dst[(5 + dr) * 320 + dc] = src[(29 << 6) + tc];
  dst[(6 + dr) * 320 + dc] = src[(34 << 6) + tc];
  dst[(7 + dr) * 320 + dc] = src[(40 << 6) + tc];
  dst[(8 + dr) * 320 + dc] = src[(46 << 6) + tc];
  dst[(9 + dr) * 320 + dc] = src[(52 << 6) + tc];
  dst[(10 + dr) * 320 + dc] = src[(58 << 6) + tc];
}


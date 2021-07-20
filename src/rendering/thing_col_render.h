#ifndef THING_COL_RENDER
#define THING_COL_RENDER

#include "../map/map.h"

struct thing_column_render_data {
	SDL_Rect* src;
	const SDL_Rect* dest;
	const int* frame_offset;
  struct mapdef* map;

	int thing_sorted_index;
	int screen_column;

  // For rendering
  int start_row, end_row;
  // Used when rendering each col, and calculated once per thing.
  int tex_height;
  int thing_dist_sqrt;
};

#endif

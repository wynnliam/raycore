#include "./thing_col_render.h"

#define PROJ_W 320
#define PROJ_H 200
extern int z_buffer_2d[PROJ_W][PROJ_H];
extern unsigned int thing_pixels[64000];

void thing_col_1(struct thing_column_render_data* td) {
  struct mapdef* map = td->map;
  const int th = td->tex_height;
  const int tds = td->thing_dist_sqrt;
  const int sr = td->start_row;
  const int dy = td->dest->y;
  const int dh = td->dest->h;
  const int fo1 = td->frame_offset[1];
  const int ind = td->thing_sorted_index;
  const int scrc = td->screen_column;
  const int t_x = td->src->x + td->frame_offset[0];
  int t_y;
  unsigned int t_c;
  int czb;
  int k = sr;
  czb = z_buffer_2d[scrc][k];
  t_y = (((k - dy) * th) / dh) + fo1;
  t_c = map->things[ind].data[(t_y << 6) + t_x];
  if(czb == -1 && tds <= czb && ((unsigned char*)(&t_c))[3] > 0) {
    thing_pixels[k * PROJ_W + scrc] = t_c;
    z_buffer_2d[scrc][k] = tds;
  }
}

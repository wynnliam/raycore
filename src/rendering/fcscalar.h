// Liam Wynn, 6/28/2021, Raycore

#include "../map/map.h"

#ifndef RAYCAST_FC_SCALAR
#define RAYCAST_FC_SCALAR

typedef struct {
  // Screen column
  int sc;
  // Sine value
  int sv;
  // cosine value
  int cv;
  int ray_angle_relative_to_player_rot;
  struct mapdef* map;
  // Floor screen pixels
  unsigned int* fcp;

  // Player position
  int player_x, player_y;
} fcd;

void draw_fc(const int bottom, fcd* fcd);

void draw_fc_100(fcd* fcd);
void draw_fc_101(fcd* fcd);
void draw_fc_102(fcd* fcd);
void draw_fc_103(fcd* fcd);
void draw_fc_104(fcd* fcd);
void draw_fc_105(fcd* fcd);
void draw_fc_106(fcd* fcd);
void draw_fc_107(fcd* fcd);
void draw_fc_108(fcd* fcd);
void draw_fc_109(fcd* fcd);
void draw_fc_110(fcd* fcd);
void draw_fc_111(fcd* fcd);
void draw_fc_112(fcd* fcd);
void draw_fc_113(fcd* fcd);
void draw_fc_114(fcd* fcd);
void draw_fc_115(fcd* fcd);
void draw_fc_116(fcd* fcd);
void draw_fc_117(fcd* fcd);
void draw_fc_118(fcd* fcd);
void draw_fc_119(fcd* fcd);
void draw_fc_120(fcd* fcd);
void draw_fc_121(fcd* fcd);
void draw_fc_122(fcd* fcd);
void draw_fc_123(fcd* fcd);
void draw_fc_124(fcd* fcd);
void draw_fc_125(fcd* fcd);
void draw_fc_126(fcd* fcd);
void draw_fc_127(fcd* fcd);
void draw_fc_128(fcd* fcd);
void draw_fc_129(fcd* fcd);
void draw_fc_130(fcd* fcd);
void draw_fc_131(fcd* fcd);
void draw_fc_132(fcd* fcd);
void draw_fc_133(fcd* fcd);
void draw_fc_134(fcd* fcd);
void draw_fc_135(fcd* fcd);
void draw_fc_136(fcd* fcd);
void draw_fc_137(fcd* fcd);
void draw_fc_138(fcd* fcd);
void draw_fc_139(fcd* fcd);
void draw_fc_140(fcd* fcd);
void draw_fc_141(fcd* fcd);
void draw_fc_142(fcd* fcd);
void draw_fc_143(fcd* fcd);
void draw_fc_144(fcd* fcd);
void draw_fc_145(fcd* fcd);
void draw_fc_146(fcd* fcd);
void draw_fc_147(fcd* fcd);
void draw_fc_148(fcd* fcd);
void draw_fc_149(fcd* fcd);
void draw_fc_150(fcd* fcd);
void draw_fc_151(fcd* fcd);
void draw_fc_152(fcd* fcd);
void draw_fc_153(fcd* fcd);
void draw_fc_154(fcd* fcd);
void draw_fc_155(fcd* fcd);
void draw_fc_156(fcd* fcd);
void draw_fc_157(fcd* fcd);
void draw_fc_158(fcd* fcd);
void draw_fc_159(fcd* fcd);
void draw_fc_160(fcd* fcd);
void draw_fc_161(fcd* fcd);
void draw_fc_162(fcd* fcd);
void draw_fc_163(fcd* fcd);
void draw_fc_164(fcd* fcd);
void draw_fc_165(fcd* fcd);
void draw_fc_166(fcd* fcd);
void draw_fc_167(fcd* fcd);
void draw_fc_168(fcd* fcd);
void draw_fc_169(fcd* fcd);
void draw_fc_170(fcd* fcd);
void draw_fc_171(fcd* fcd);
void draw_fc_172(fcd* fcd);
void draw_fc_173(fcd* fcd);
void draw_fc_174(fcd* fcd);
void draw_fc_175(fcd* fcd);
void draw_fc_176(fcd* fcd);
void draw_fc_177(fcd* fcd);
void draw_fc_178(fcd* fcd);
void draw_fc_179(fcd* fcd);
void draw_fc_180(fcd* fcd);
void draw_fc_181(fcd* fcd);
void draw_fc_182(fcd* fcd);
void draw_fc_183(fcd* fcd);
void draw_fc_184(fcd* fcd);
void draw_fc_185(fcd* fcd);
void draw_fc_186(fcd* fcd);
void draw_fc_187(fcd* fcd);
void draw_fc_188(fcd* fcd);
void draw_fc_189(fcd* fcd);
void draw_fc_190(fcd* fcd);
void draw_fc_191(fcd* fcd);
void draw_fc_192(fcd* fcd);
void draw_fc_193(fcd* fcd);
void draw_fc_194(fcd* fcd);
void draw_fc_195(fcd* fcd);
void draw_fc_196(fcd* fcd);
void draw_fc_197(fcd* fcd);
void draw_fc_198(fcd* fcd);
void draw_fc_199(fcd* fcd);
#endif

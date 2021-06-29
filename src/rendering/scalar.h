// Liam Wynn, 6/24/2021, Raycore

#ifndef RAYCAST_SCALAR
#define RAYCAST_SCALAR

typedef struct {
  // Where we will write the scaled src to.
  unsigned int* dst;
  // Where we read pixels from
  unsigned int* src;
  // Used only for things. Used to make sure sure we don't write
  // pixels if the slice is too far.
  unsigned int** z;
  // the destination row we want to write to
  int dr;
  // the destination col we want to write to
  int dc;
  // the texture col we want to read from.
  int tc;
  // the height of the texture
  int th;
  // texture delta: How many pixels do we move before sampling the next
  // texture
  int td;
  // offset of texture: If we want to render a part of a texture, we can
  // specify the start of that here. If wanted to all pixels after the
  // first 22, then I set ofst to 22.
  int ofst;
  // The distance this wall slice is from the user
  int hdist;
} sdata;

void init_td_table();
int get_texture_delta(const int th, const int s);

void scale_to_1(sdata* data);
void scale_to_2(sdata* data);
void scale_to_3(sdata* data);
void scale_to_4(sdata* data);
void scale_to_5(sdata* data);
void scale_to_6(sdata* data);
void scale_to_7(sdata* data);
void scale_to_8(sdata* data);
void scale_to_9(sdata* data);
void scale_to_10(sdata* data);
void scale_to_11(sdata* data);
void scale_to_12(sdata* data);
void scale_to_13(sdata* data);
void scale_to_14(sdata* data);
void scale_to_15(sdata* data);
void scale_to_16(sdata* data);
void scale_to_17(sdata* data);
void scale_to_18(sdata* data);
void scale_to_19(sdata* data);
void scale_to_20(sdata* data);
void scale_to_21(sdata* data);
void scale_to_22(sdata* data);
void scale_to_23(sdata* data);
void scale_to_24(sdata* data);
void scale_to_25(sdata* data);
void scale_to_26(sdata* data);
void scale_to_27(sdata* data);
void scale_to_28(sdata* data);
void scale_to_29(sdata* data);
void scale_to_30(sdata* data);
void scale_to_31(sdata* data);
void scale_to_32(sdata* data);
void scale_to_33(sdata* data);
void scale_to_34(sdata* data);
void scale_to_35(sdata* data);
void scale_to_36(sdata* data);
void scale_to_37(sdata* data);
void scale_to_38(sdata* data);
void scale_to_39(sdata* data);
void scale_to_40(sdata* data);
void scale_to_41(sdata* data);
void scale_to_42(sdata* data);
void scale_to_43(sdata* data);
void scale_to_44(sdata* data);
void scale_to_45(sdata* data);
void scale_to_46(sdata* data);
void scale_to_47(sdata* data);
void scale_to_48(sdata* data);
void scale_to_49(sdata* data);
void scale_to_50(sdata* data);
void scale_to_51(sdata* data);
void scale_to_52(sdata* data);
void scale_to_53(sdata* data);
void scale_to_54(sdata* data);
void scale_to_55(sdata* data);
void scale_to_56(sdata* data);
void scale_to_57(sdata* data);
void scale_to_58(sdata* data);
void scale_to_59(sdata* data);
void scale_to_60(sdata* data);
void scale_to_61(sdata* data);
void scale_to_62(sdata* data);
void scale_to_63(sdata* data);
void scale_to_64(sdata* data);
void scale_to_65(sdata* data);
void scale_to_66(sdata* data);
void scale_to_67(sdata* data);
void scale_to_68(sdata* data);
void scale_to_69(sdata* data);
void scale_to_70(sdata* data);
void scale_to_71(sdata* data);
void scale_to_72(sdata* data);
void scale_to_73(sdata* data);
void scale_to_74(sdata* data);
void scale_to_75(sdata* data);
void scale_to_76(sdata* data);
void scale_to_77(sdata* data);
void scale_to_78(sdata* data);
void scale_to_79(sdata* data);
void scale_to_80(sdata* data);
void scale_to_81(sdata* data);
void scale_to_82(sdata* data);
void scale_to_83(sdata* data);
void scale_to_84(sdata* data);
void scale_to_85(sdata* data);
void scale_to_86(sdata* data);
void scale_to_87(sdata* data);
void scale_to_88(sdata* data);
void scale_to_89(sdata* data);
void scale_to_90(sdata* data);
void scale_to_91(sdata* data);
void scale_to_92(sdata* data);
void scale_to_93(sdata* data);
void scale_to_94(sdata* data);
void scale_to_95(sdata* data);
void scale_to_96(sdata* data);
void scale_to_97(sdata* data);
void scale_to_98(sdata* data);
void scale_to_99(sdata* data);
void scale_to_100(sdata* data);
void scale_to_101(sdata* data);
void scale_to_102(sdata* data);
void scale_to_103(sdata* data);
void scale_to_104(sdata* data);
void scale_to_105(sdata* data);
void scale_to_106(sdata* data);
void scale_to_107(sdata* data);
void scale_to_108(sdata* data);
void scale_to_109(sdata* data);
void scale_to_110(sdata* data);
void scale_to_111(sdata* data);
void scale_to_112(sdata* data);
void scale_to_113(sdata* data);
void scale_to_114(sdata* data);
void scale_to_115(sdata* data);
void scale_to_116(sdata* data);
void scale_to_117(sdata* data);
void scale_to_118(sdata* data);
void scale_to_119(sdata* data);
void scale_to_120(sdata* data);
void scale_to_121(sdata* data);
void scale_to_122(sdata* data);
void scale_to_123(sdata* data);
void scale_to_124(sdata* data);
void scale_to_125(sdata* data);
void scale_to_126(sdata* data);
void scale_to_127(sdata* data);
void scale_to_128(sdata* data);
void scale_to_129(sdata* data);
void scale_to_130(sdata* data);
void scale_to_131(sdata* data);
void scale_to_132(sdata* data);
void scale_to_133(sdata* data);
void scale_to_134(sdata* data);
void scale_to_135(sdata* data);
void scale_to_136(sdata* data);
void scale_to_137(sdata* data);
void scale_to_138(sdata* data);
void scale_to_139(sdata* data);
void scale_to_140(sdata* data);
void scale_to_141(sdata* data);
void scale_to_142(sdata* data);
void scale_to_143(sdata* data);
void scale_to_144(sdata* data);
void scale_to_145(sdata* data);
void scale_to_146(sdata* data);
void scale_to_147(sdata* data);
void scale_to_148(sdata* data);
void scale_to_149(sdata* data);
void scale_to_150(sdata* data);
void scale_to_151(sdata* data);
void scale_to_152(sdata* data);
void scale_to_153(sdata* data);
void scale_to_154(sdata* data);
void scale_to_155(sdata* data);
void scale_to_156(sdata* data);
void scale_to_157(sdata* data);
void scale_to_158(sdata* data);
void scale_to_159(sdata* data);
void scale_to_160(sdata* data);
void scale_to_161(sdata* data);
void scale_to_162(sdata* data);
void scale_to_163(sdata* data);
void scale_to_164(sdata* data);
void scale_to_165(sdata* data);
void scale_to_166(sdata* data);
void scale_to_167(sdata* data);
void scale_to_168(sdata* data);
void scale_to_169(sdata* data);
void scale_to_170(sdata* data);
void scale_to_171(sdata* data);
void scale_to_172(sdata* data);
void scale_to_173(sdata* data);
void scale_to_174(sdata* data);
void scale_to_175(sdata* data);
void scale_to_176(sdata* data);
void scale_to_177(sdata* data);
void scale_to_178(sdata* data);
void scale_to_179(sdata* data);
void scale_to_180(sdata* data);
void scale_to_181(sdata* data);
void scale_to_182(sdata* data);
void scale_to_183(sdata* data);
void scale_to_184(sdata* data);
void scale_to_185(sdata* data);
void scale_to_186(sdata* data);
void scale_to_187(sdata* data);
void scale_to_188(sdata* data);
void scale_to_189(sdata* data);
void scale_to_190(sdata* data);
void scale_to_191(sdata* data);
void scale_to_192(sdata* data);
void scale_to_193(sdata* data);
void scale_to_194(sdata* data);
void scale_to_195(sdata* data);
void scale_to_196(sdata* data);
void scale_to_197(sdata* data);
void scale_to_198(sdata* data);
void scale_to_199(sdata* data);
void scale_to_200(sdata* data);

void scale_to_i(sdata* data, const int scale_to);

#endif

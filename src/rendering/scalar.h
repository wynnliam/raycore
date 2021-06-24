// Liam Wynn, 6/24/2021, Raycore

//void scale_to_2(unsigned int* dst, unsigned int* src, int* zb, const int

typedef struct {
  unsigned int* dst;
  unsigned int* src;
  unsigned int** z;
  int dr;
  int dc;
  int tc;
  int th;
} sdata;

void scale_to_11(sdata* data);

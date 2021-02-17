// Liam Wynn, 2/14/2021, Raycore

#ifndef RAYCORE_NET_UTIL
#define RAYCORE_NET_UTIL

#define MAX_CLIENTS		16
#define PORT	20715

// This is what we will send to each client every
// so often.
typedef struct {
  int active;
  int level_id;
  int x, y, rot;
} client_data;

#endif

// Liam Wynn, 2/14/2021, Raycore

#ifndef RAYCORE_NET_UTIL
#define RAYCORE_NET_UTIL

#define MAX_CLIENTS		16
#define PORT	20715

// A single integer the server can send the client
#define CLIENT_SIGNAL	0
// The state of the game. TODO: Should recieve
// as UDP
#define CLIENT_GAME		CLIENT_SIGNAL + 1

// If the user connects, the value will be
// their index in the client list.
#define SIGNAL_CONNECT	0
// The the server if full, they get this.
#define SIGNAL_FULL		SIGNAL_CONNECT + 1

// This is what we will send to each client every
// so often.
typedef struct {
  int active;
  int level_id;
  int x, y, rot;
} client_data;

typedef struct {
  char type;
  char value;
} client_signal;

typedef struct {
  char type;
  struct union {
    client_signal signal;
    client_data[MAX_CLIENTS] game;
  } data; 
} client_message;

#endif

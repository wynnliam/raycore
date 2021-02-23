// Liam Wynn, 2/14/2021, Raycore

#ifndef RAYCORE_NET_UTIL
#define RAYCORE_NET_UTIL

#include <SDL2/SDL_net.h>

#define MAX_CLIENTS		16
#define PORT	20715

/* MESSAGE WE SEND TO CLIENT */

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
  union {
    client_signal signal;
    client_data game[MAX_CLIENTS];
  } data; 
} client_message;

// Sends a message to a given socket.
int send_message_to_client(TCPsocket client_socket, client_message* message);
int recv_message_from_server(TCPsocket server_socket, client_message* result);

/* SEND MESSAGE TO SERVER */

// Local update of client's current data.
#define SERVER_LOCAL_UPDATE		0

typedef struct {
  char type;
  union {
    client_data local;
  } data;
} server_message;

int send_message_to_server(TCPsocket server_socket, server_message* message);
int recv_message_from_client(TCPsocket client_socket, server_message* result);
#endif

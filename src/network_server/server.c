// Liam Wynn, 2/11/2021, Raycore

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "./util.h"

typedef struct {
  TCPsocket tcp_socket;
  client_data data;
  int active;
} client_connect;

// Holds the IPv4 address and port number
// for this server.
IPaddress ip_addr;

// TODO: Eventually we will have both a TCP
// socket and UDP socket. TCP is for everything that
// isn't sending/recieving constant game updates
TCPsocket tcp_server_socket;

// There are 1 + MAX_CLIENTS sockets
// to listen for.
SDLNet_SocketSet sockets;

client_connect clients[MAX_CLIENTS];
int num_clients = 0;

// The main server processing loop.
void server();

int main() {
  if(SDL_Init(0) == -1) {
    printf("server: SDL_Init: %s\n", SDL_GetError());
    return -1;
  }

  if(SDLNet_Init() == -1) {
    printf("server: SDLNet_Init: %s\n", SDLNet_GetError());
    return -1;
  }

  printf("server: SDL initialized\n");

  if(SDLNet_ResolveHost(&ip_addr, NULL, PORT) == -1) {
    printf("server: SDLNet_Resolve_Host: %s\n", SDLNet_GetError());
    goto lbl_quit;
  }

  tcp_server_socket = SDLNet_TCP_Open(&ip_addr);
  if(tcp_server_socket == NULL) {
    printf("server: SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    goto lbl_quit;
  }

  sockets = SDLNet_AllocSocketSet(1 + MAX_CLIENTS);
  if(!sockets) {
    printf("server: SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
    SDLNet_TCP_Close(tcp_server_socket);
    goto lbl_quit;
  }

  SDLNet_TCP_AddSocket(sockets, tcp_server_socket);

  printf("server: starting\n");
  printf("server: listening on port %d\n", PORT);

  server();

  SDLNet_TCP_Close(tcp_server_socket);

lbl_quit:
  printf("server: shutting down\n");
  SDLNet_Quit();
  SDL_Quit();
  printf("server: bye bye\n");

  return 0;
}

void server() {
  // Whenever any activity is detected, we will pick
  // it up with this variable.
  int num_ready;

  while(1) {
    printf("server: doing important stuff\n");
    SDL_Delay(1000);
    num_ready = SDLNet_CheckSockets(sockets, 10);

    if(num_ready == -1) {
      printf("server: SDLNet_CheckSockets: %s\n", SDLNet_GetError());
      break;
    }

    if(num_ready == 0)
      continue;

    printf("server: I detected activity on %d sockets\n", num_ready);
  }
}

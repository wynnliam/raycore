// Liam Wynn, 2/11/2021, Raycore

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define PORT	20715

// Holds the IPv4 address and port number
// for this server.
IPaddress ip_addr;

// TODO: Eventually we will have both a TCP
// socket and UDP socket. TCP is for everything that
// isn't sending/recieving constant game updates
TCPsocket tcp_server_socket;

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

  printf("server: starting\n");
  printf("server: listening on port %d\n", PORT);

  while(1) {
    printf("server: doing important stuff\n");
    SDL_Delay(1000);
  }

  SDLNet_TCP_Close(tcp_server_socket);

lbl_quit:
  printf("server: shutting down\n");
  SDLNet_Quit();
  SDL_Quit();
  printf("server: bye bye\n");

  return 0;
}

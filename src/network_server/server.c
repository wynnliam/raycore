// Liam Wynn, 2/11/2021, Raycore

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

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
  printf("server: starting\n");

lbl_quit:
  printf("server: shutting down\n");
  SDLNet_Quit();
  SDL_Quit();
  printf("server: bye bye\n");

  return 0;
}

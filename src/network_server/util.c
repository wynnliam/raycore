// Liam Wynn, 2/17/2021, Raycore

#include "./util.h"

int send_message(TCPsocket socket, client_message* message) {
  SDLNet_TCP_Send(socket, message, sizeof(client_message));
  return 1;
}

int recv_message(TCPsocket socket, client_message* result) {
  return SDLNet_TCP_Recv(socket, result, sizeof(client_message));
}

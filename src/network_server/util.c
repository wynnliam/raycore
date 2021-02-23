// Liam Wynn, 2/17/2021, Raycore

#include "./util.h"

int send_message_to_client(TCPsocket socket, client_message* message) {
  SDLNet_TCP_Send(socket, message, sizeof(client_message));
  return 1;
}

int recv_message_from_server(TCPsocket socket, client_message* result) {
  return SDLNet_TCP_Recv(socket, result, sizeof(client_message));
}

int send_message_to_server(TCPsocket server_socket, server_message* message) {
  return SDLNet_TCP_Send(server_socket, message, sizeof(server_message));
}

int recv_message_from_client(TCPsocket client_socket, server_message* result) {
  return SDLNet_TCP_Recv(client_socket, result, sizeof(server_message));
}

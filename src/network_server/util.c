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
  char buf[16];
  SDLNet_Write32(message->data.local.x, buf);
  SDLNet_Write32(message->data.local.y, buf + 4);
  SDLNet_Write32(message->data.local.rot, buf + 8);
  SDLNet_Write32(message->data.local.level_id, buf + 12);
  return SDLNet_TCP_Send(server_socket, buf, 16);
}

int recv_message_from_client(TCPsocket client_socket, server_message* result) {
  char buf[16];
  int recvd = SDLNet_TCP_Recv(client_socket, buf, 16);

  if(recvd < 1)
    return recvd;

  result->data.local.x = SDLNet_Read32(buf);
  result->data.local.y = SDLNet_Read32(buf + 4);
  result->data.local.rot = SDLNet_Read32(buf + 8);
  result->data.local.level_id = SDLNet_Read32(buf + 12);

  return recvd;
}

/*int end = 0; // Index of first free space in the buffer.
while (true) {
  // Append new data at the end of the previous remaining data
  int received = SDLNet_TCP_Recv(clientSocketTCP, buffer + end, 5000 - end);
  // handle errors etc

  end = end + received;
  while (true) {
    // detect a message at the start of the buffer
    int index = 0;
    while (index < end && buffer[index] != '\0') {
      index = index + 1;
    }
    if (index >= end) { // Failed to find a message, wait for more data
      break;
    }
    
    // 'index' points to the '\0', the last byte of the message
    int length = index + 1;
    
    // buffer[0] to buffer[index] is a message
    printf("Found a message of length %d\n", length);
    
    // Remove message from the buffer (by moving everything after it)
    memmove(buffer, buffer + length, end - length);
    end = end - length;
    
    // and try to find the next message
  }
}*/

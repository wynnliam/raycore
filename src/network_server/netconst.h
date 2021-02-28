// Liam Wynn, 2/27/2021, Raycore

#ifndef RAYCORE_NETCONST
#define RAYCORE_NETCONST

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

#endif

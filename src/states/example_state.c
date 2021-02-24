// Liam Wynn, 7/8/2019, Raycore

#include "./example_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../rendering/raycaster.h"
#include "../input_handler.h"

#include "../map/map_loading/map_loading.h"

// TODO: Move this elsewhere (entities should only exist in maps)
#include "../map/entity/entity.h"
#include "../map/entity/example_entity.h"

#include "../network_server/util.h"

// Stores the player 
static int player_x, player_y;
// The player rotation.
static int player_rot;

// Temporary storage for map.
static unsigned int curr_level;
static struct mapdef* map;

static int quit = 0;
static int next_state;

extern int sin128table[361];
extern int cos128table[361];

static pthread_t network_pthread;
// If 1, we are in single-player mode
static int single_player = 1;
// Information needed if we are connected to a server.
static IPaddress ip;
static TCPsocket tcp_socket;
static SDLNet_SocketSet sockets;
// When 1, will quit the network_handler thread
static int terminate_network_handler;
static pthread_mutex_t mtx_terminate_nethand;
// What we send to the server ever frame.
static client_data to_send;
static pthread_mutex_t mtx_data;

static void* network_handler() {
  int quit = 0;
  server_message message;

  do {
    pthread_mutex_lock(&mtx_data);
    message.data.local = to_send;
    pthread_mutex_unlock(&mtx_data);

    int result = send_message_to_server(tcp_socket, &message);
    if(!result)
      printf("client: send_message_to_server: %s\n", SDLNet_GetError());

    // TODO: Check for messages from server.

    pthread_mutex_lock(&mtx_terminate_nethand);
    quit = terminate_network_handler;
    pthread_mutex_unlock(&mtx_terminate_nethand);

  } while(!quit);

  SDLNet_TCP_DelSocket(sockets, tcp_socket);
  SDLNet_TCP_Close(tcp_socket);

  return 0;
}

void state_example_initialize(SDL_Renderer* renderer) {
	player_x = 256;
	player_y = 256;
	player_rot = 0;

	// Raycaster Initialization

	// Initializes all the angle lookup tables.
	initialize_lookup_tables();
	// Intializes the rendering textures.
	initialize_render_textures(renderer);
	// Enables transparent pixel 
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Map Loading

	initialize_map_lookup_table();
}

void state_example_enter(const int from_state, void* message) {
	// Setting this to NONE requests the loop handler to NOT
	// transition to another state.
	next_state = STATE_ID_NONE;

	if(!message)
		return;

	// The message is a string in this case.
	if(from_state == STATE_ID_MAIN_MENU)
		printf("%s", (char*)message);

	free(message);

    char addr[100];

    printf("Please enter the server name, or 0 for single player mode: ");
    char c;
    int index = 0;
    while((c = getchar()) != '\n') {
      addr[index] = c;
      index++;
    }
    addr[index] = '\0';
    if(strcmp(addr, "0") == 0) {
      single_player = 1;
      printf("Playing in singleplayer mode\n");
    } else {
      single_player = 0;
      printf("Connecting to: %s\n", addr);

      if(SDLNet_ResolveHost(&ip, addr, 20715) == -1) {
        printf("client: SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        single_player = 1;
        goto post_try_connect;
      }

      tcp_socket = SDLNet_TCP_Open(&ip);
      if(!tcp_socket) {
        printf("client: SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        single_player = 1;
        goto post_try_connect;
      }

      sockets = SDLNet_AllocSocketSet(1);
      SDLNet_TCP_AddSocket(sockets, tcp_socket);
      client_message message;

      while(1) {
        SDLNet_CheckSockets(sockets, 100);
        if(SDLNet_SocketReady(tcp_socket)) {
          recv_message_from_server(tcp_socket, &message);

          // We expect it to be a signal
          if(message.type == CLIENT_SIGNAL && message.data.signal.type == SIGNAL_CONNECT) {
            printf("client: connected. I am %d\n", message.data.signal.value);
          } else {
            printf("client: server full\n");
            single_player = 1;
          }
          break;
        }
      }
    }

    post_try_connect:
    if(!single_player) {
      terminate_network_handler = 0;
      pthread_mutex_init(&mtx_terminate_nethand, NULL);
      pthread_mutex_init(&mtx_data, NULL);
      pthread_create(&network_pthread, NULL, (void*)network_handler, NULL);
    }

	curr_level = 4;
	map = load_map_from_file(do_map_lookup(curr_level));
	spawn_player(map, &player_x, &player_y, &player_rot, 2);
	curr_level++;
}

void state_example_leave() {
	printf("Example Leave!\n");

    // Wait for the network to finish up
    if(!single_player) {
      pthread_mutex_lock(&mtx_terminate_nethand);
      terminate_network_handler = 1;
      pthread_mutex_unlock(&mtx_terminate_nethand);
      pthread_join(network_pthread, NULL);
    }
}

void state_example_process_input() {
	if(key_pressed_once(SDL_SCANCODE_P)) {
		next_state = STATE_ID_MAIN_MENU;
	}
}

void state_example_update() {
	if(!map) {
		return;
	}

	clear_all_thing_signals(map);

	if(key_pressed(SDL_SCANCODE_A)) {
		player_rot += 2;

		if(player_rot < 0)
			player_rot += 360;
		if(player_rot > 360)
			player_rot -= 360;
	}

	if(key_pressed(SDL_SCANCODE_D)) {
		player_rot -= 2;

		if(player_rot < 0)
			player_rot += 360;
		if(player_rot > 360)
			player_rot -= 360;
	}

	if(key_pressed(SDL_SCANCODE_W)) {
		player_y -= (sin128table[player_rot] << 4) >> 7;
		player_x += (cos128table[player_rot] << 4) >> 7;

		if(get_tile(player_x, player_y, map) < 0 || is_position_wall(map, player_x, player_y)) {
			player_y += (sin128table[player_rot] << 4) >> 7;
			player_x -= (cos128table[player_rot] << 4) >> 7;
		}
	}

	if(key_pressed(SDL_SCANCODE_S)) {
		player_y += (sin128table[player_rot] << 4) >> 7;
		player_x -= (cos128table[player_rot] << 4) >> 7;

		if(get_tile(player_x, player_y, map) < 0 || is_position_wall(map, player_x, player_y)) {
			player_y -= (sin128table[player_rot] << 4) >> 7;
			player_x += (cos128table[player_rot] << 4) >> 7;
		}
	}

	if(key_pressed_once(SDL_SCANCODE_E)) {
		printf("User activation!\n");

		int player_tile_pos, thing_tile_pos;
		int* thing_pos;
		// First divide player position by 64 then compute tile index.
		player_tile_pos = (player_y >> 6) * map->map_w + (player_x >> 6);

		unsigned int i;
		for(i = 0; i < map->num_things; i++) {
			thing_pos = map->things[i].position;
			// Do the same for the thing position.
			thing_tile_pos = (thing_pos[1] >> 6) * map->map_w + (thing_pos[0] >> 6);

			if(thing_tile_pos == player_tile_pos)
				set_signal_user_interact_on(&map->things[i]);
		}

		// TODO: Map quadtree.
		// TODO: Move this code to map.h/map.c (procedure check_map_user_interaction or something).
	}

	if(key_pressed(SDL_SCANCODE_C)) {
		printf("Player position = [%d, %d]. Player rotation = %d\n", player_x, player_y, player_rot);
	}

	update_thing_anims(map, player_rot);
	update_entities(map);

	if(map->signal_level_transition > -1) {
		curr_level = map->signal_level_transition;
		int next_spawn = map->signal_next_spawn;
		free_map(&map);
		map = load_map_from_file(do_map_lookup(curr_level));
		spawn_player(map, &player_x, &player_y, &player_rot, next_spawn);
	}

    if(!single_player) {
      pthread_mutex_lock(&mtx_data);
      to_send.level_id = curr_level;
      to_send.x = player_x;
      to_send.y = player_y;
      to_send.rot = player_rot;
      pthread_mutex_unlock(&mtx_data); 
    }
}


void state_example_draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	if(map)
		cast_rays(renderer, map, player_x, player_y, player_rot);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}

void state_example_clean_up() {
	free_map(&map);
}

int state_example_quit() {
	return quit;
}

int state_example_next_state() {
	return next_state;
}

void* state_example_get_pass_message() {
	return NULL;
}

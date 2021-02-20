CMP= gcc
FLG= -Wall -o2 -g
LIB= -lm -lSDL2 -lSDL2_net -pthread

OUT= ./bin/raycore.out
SRC= ./src/*.c \
     ./src/rendering/*.c \
     ./src/animation/*.c \
	 ./src/states/*.c \
     ./src/map/*.c \
     ./src/map/entity/*.c \
     ./src/map/map_loading/*.c \
     ./src/map/map_loading/tokenizer/*.c \
     ./src/map/map_loading/parser/*.c \
     ./src/map/map_loading/ir/*.c \
	 ./src/map/map_loading/entity_loading/*.c \
     ./src/map/map_loading/utilities/*.c \
     ./src/network_server/util.c

SRV_OUT= ./bin/raycore_server.out
SRV_SRC= ./src/network_server/*.c

.PHONY: clean

all: $(SRC)
	$(CMP) $(FLG) $(SRC) -o $(OUT) $(LIB)

server: $(SRV_SRC)
	$(CMP) $(FLG) $(SRV_SRC) -o $(SRV_OUT) $(LIB)

clean:
	-rm $(OUT) $(SRV_OUT)

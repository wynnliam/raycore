CMP= gcc
FLG= -g -Wall
LIB= -lm -lSDL2
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
     ./src/map/map_loading/utilities/*.c \
     ./src/map/map_loading/parse/*.c

.PHONY: clean

all: $(SRC)
	$(CMP) $(FLG) $(SRC) -o $(OUT) $(LIB)

clean:
	rm $(OUT)

CMP= gcc
FLG= -g -Wall
LIB= -lm -lSDL2
OUT= ./bin/raycore.out

SRC= ./src/*.c \
	 ./src/parse/*.c

.PHONY: clean

all: $(SRC)
	$(CMP) $(FLG) $(SRC) -o $(OUT) $(LIB)

clean:
	rm $(OUT)

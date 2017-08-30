OBJECTS := $(patsubst source/*.c, build/*.o, $(wildcard source/*.c))
TESTS   := $(patsubst tests/*.c, build/test_*.o, $(wildcard tests/*.c))
FLAGS   := -DSDL_MAIN_HANDLED -ansi -Wall -Wextra -pedantic -ggdb
INCLUDE := -Iinclude -IC:\dev\include 

all: $(OBJECTS)
	gcc $(FLAGS) $(INCLUDE) -c main.c -o build/main.o
	gcc $(FLAGS) -LC:\dev\lib $(INCLUDE) -o bin build/main.o $(OBJECTS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf 

test: $(TESTS) $(OBJECTS)
	gcc $(FLAGS) -LC:\dev\lib $(INCLUDE) -o tests/bin $(OBJECTS) $(TESTS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf 
	./tests/bin

build/%.o: source/%.c
	gcc $(FLAGS) $(INCLUDE) -c $< -o $@ 

.PHONY: clean
clean:
	rm -f bin tests/bin
	rm -rf build/*.o

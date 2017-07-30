OBJECTS := $(patsubst source/*.c, build/*.o, $(wildcard source/*.c) $(wildcard source/**/*.c))
FLAGS   := -DSDL_MAIN_HANDLED -ansi -Wall -Wextra -pedantic -ggdb
INCLUDE := -Iinclude -IC:\dev\include 

all: $(OBJECTS)
	gcc $(FLAGS) -LC:\dev\lib $(INCLUDE) -o bin $(OBJECTS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf 

build/**/%.o: source/%.c
	gcc $(FLAGS) $(INCLUDE) -c $< -o $@ 

.PHONY: clean
clean:
	rm -rf build/*.o

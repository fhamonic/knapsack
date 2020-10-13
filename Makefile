CC=g++
CC_NORM=c++17

SRC_DIR=src
BUILD_DIR=build

INCLUDE_FLAGS=-I include

#-DNDEBUG to silent asserts
CFLAGS=-g -W -Wall -ansi -pedantic -std=$(CC_NORM) -fconcepts -O2 -flto -march=native -pipe $(INCLUDE_FLAGS)
LDFLAGS=

EXTENSION=.out

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

all: run

dir: 
	mkdir -p $(BUILD_DIR)

test: $(OBJ) $(BUILD_DIR)/test.o
	$(CC) -o $@$(EXTENSION) $^ $(LDFLAGS)

run: dir test
	./test.out instances/sac0
	./test.out instances/sac1
	./test.out instances/sac2
	./test.out instances/sac3
	./test.out instances/sac4


clean:
	rm -rf $(BUILD_DIR)/*.o

mrproper: clean
	rm -rf $(BUILD_DIR)
	rm -rf -f *$(EXTENSION)
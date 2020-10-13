CC=g++-9
CC_NORM=c++2a

CPUS ?= $(shell getconf _NPROCESSORS_ONLN || echo 1)
MAKEFLAGS += --jobs=$(CPUS)

SRC_DIR=src
THIRDPARTY_DIR=thirdparty
BUILD_DIR=build

LEMON_INCLUDE_DIR=~/Libs/lemon-1.3.1/

INCLUDE_FLAGS=-I include -I $(SRC_DIR) -I $(THIRDPARTY_DIR)

#-DNDEBUG
CFLAGS=-g -W -Wall -Wno-deprecated-copy -ansi -pedantic -std=$(CC_NORM) -fconcepts -O2 -flto -march=native -pipe $(INCLUDE_FLAGS)
LDFLAGS=

EXTENSION=.out


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

build_dir:
	mkdir -p $(BUILD_DIR)
output_dir:
	mkdir -p output
dir : build_dir output_dir

all : dir $(OBJ) $(EXEC)



test: $(OBJ) $(BUILD_DIR)/test.o
	$(CC) -o $@$(EXTENSION) $^ $(LDFLAGS)

run:
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
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

knapstack_bnb: $(OBJ) $(BUILD_DIR)/knapstack_bnb.o
	$(CC) -o $@$(EXTENSION) $^ $(LDFLAGS)

run: dir knapstack_bnb
	./knapstack_bnb.out instances/sac0
	./knapstack_bnb.out instances/sac1
	./knapstack_bnb.out instances/sac2
	./knapstack_bnb.out instances/sac3
	./knapstack_bnb.out instances/sac4

test_knapstack_bnb: $(OBJ) $(BUILD_DIR)/test_knapstack_bnb.o
	$(CC) -o $@$(EXTENSION) $^ $(LDFLAGS)



clean:
	rm -rf $(BUILD_DIR)/*.o

mrproper: clean
	rm -rf $(BUILD_DIR)
	rm -rf -f *$(EXTENSION)
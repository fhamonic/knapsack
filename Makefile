MAKEFLAGS += --no-print-directory

CPUS?=$(shell getconf _NPROCESSORS_ONLN || echo 1)
CC:=g++

BUILD_DIR = build

.PHONY: all test clean single-header

all: $(BUILD_DIR)
	@cd $(BUILD_DIR) && \
	cmake --build . --parallel $(CPUS)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	cmake -DCMAKE_CXX_COMPILER=$(CC) -DCMAKE_BUILD_TYPE=Release -DENABLE_TEST=ON -DENABLE_EXEC=ON -DWARNINGS=ON -DOPTIMIZE_FOR_NATIVE=ON ..

test: all
	@cd $(BUILD_DIR) && \
	ctest --output-on-failure
	
clean:
	@rm -rf $(BUILD_DIR)

single-header: single-header/knapsack.hpp

single-header/knapsack.hpp:
	@python3 -m quom --include_directory include include/all.hpp fhamonic_knapsack.hpp.tmp && \
	mkdir -p single-header/fhamonic && \
	echo "/*" > single-header/knapsack.hpp && \
	cat LICENSE >> single-header/knapsack.hpp && \
	echo "*/" >> single-header/knapsack.hpp && \
	cat fhamonic_knapsack.hpp.tmp >> single-header/knapsack.hpp && \
	rm fhamonic_knapsack.hpp.tmp

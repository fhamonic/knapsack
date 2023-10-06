MAKEFLAGS += --no-print-directory

CPUS?=$(shell getconf _NPROCESSORS_ONLN || echo 1)
CC:=g++

BUILD_DIR = build

.PHONY: all test clean single-header

all: $(BUILD_DIR)
	@cd $(BUILD_DIR) && \
	cmake --build . --parallel $(CPUS)

$(BUILD_DIR):
	@conan install . -of=${BUILD_DIR} -b=missing -pr=default && \
	cd $(BUILD_DIR) && \
	cmake -DCMAKE_CXX_COMPILER=$(CC) -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DENABLE_EXEC=ON -DOPTIMIZE_FOR_NATIVE=ON ..

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

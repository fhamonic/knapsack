MAKEFLAGS += --no-print-directory

CPUS?=$(shell getconf _NPROCESSORS_ONLN || echo 1)
CC:=g++-10

BUILD_DIR = build

.PHONY: all test clean single-header

all: $(BUILD_DIR)
	@cd $(BUILD_DIR) && \
	cmake --build . --parallel $(CPUS)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	conan install .. && \
	cmake -DCMAKE_CXX_COMPILER=$(CC) -DCMAKE_BUILD_TYPE=Release -DWARNINGS=ON -DCOMPILE_FOR_NATIVE=ON -DCOMPILE_WITH_LTO=ON ..

test: all
	@cd $(BUILD_DIR) && \
	ctest --output-on-failure
	
clean:
	@rm -rf $(BUILD_DIR)

single-header: single-header/fhamonic_knapstack.hpp

single-header/fhamonic_knapstack.hpp:
	@python3 -m quom --include_directory include include/all.hpp fhamonic_knapstack.hpp.tmp && \
	mkdir -p single-header && \
	echo "/*" > single-header/fhamonic_knapstack.hpp && \
	cat LICENSE >> single-header/fhamonic_knapstack.hpp && \
	echo "*/" >> single-header/fhamonic_knapstack.hpp && \
	cat fhamonic_knapstack.hpp.tmp >> single-header/fhamonic_knapstack.hpp && \
	rm fhamonic_knapstack.hpp.tmp

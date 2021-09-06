MAKEFLAGS += --no-print-directory

CPUS?=$(shell getconf _NPROCESSORS_ONLN || echo 1)

BUILD_DIR = build

.PHONY: all clean single-header

all: $(BUILD_DIR)
	@cd $(BUILD_DIR) && \
	cmake --build . --parallel $(CPUS)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	conan install .. && \
	cmake -DCMAKE_CXX_COMPILER=g++-10 -DCMAKE_BUILD_TYPE=Release -DWARNINGS=ON -DCOMPILE_FOR_NATIVE=ON -DCOMPILE_WITH_LTO=ON ..

clean:
	@rm -rf $(BUILD_DIR)

single-header: single-header/knapstack_solver.hpp

single-header/knapstack_solver.hpp:
	@python3 -m quom --include_directory include include/all.hpp knapstack_solver.hpp.tmp && \
	mkdir -p single-header && \
	echo "/*" > single-header/knapstack_solver.hpp && \
	cat LICENSE >> single-header/knapstack_solver.hpp && \
	echo "*/" >> single-header/knapstack_solver.hpp && \
	cat knapstack_solver.hpp.tmp >> single-header/knapstack_solver.hpp && \
	rm knapstack_solver.hpp.tmp

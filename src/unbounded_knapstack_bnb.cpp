#include <iostream>
#include <filesystem>

#include "unbounded_knapstack/branch_and_bound.hpp"

#include "utils/chrono.hpp"
#include "utils/instance_parsers.hpp"

int main(int argc, const char *argv[]) {
    if(argc < 2) {
        std::cerr << "input requiered : <knapstack_instance_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::filesystem::path instance_path = argv[1];
    if(!std::filesystem::exists(instance_path)) {
        std::cerr << instance_path << ":" << " File does not exists" << std::endl;
        return EXIT_FAILURE;
    } 
    
    UnboundedKnapstack::Instance<int,int> instance = parse_unbounded_instance(instance_path);
    UnboundedKnapstack::BranchAndBound<UnboundedKnapstack::Instance, int, int> solver;

    Chrono chrono;
    UnboundedKnapstack::Solution solution = solver.solve(instance);
    int time_us = chrono.timeUs();

    std::cout << solution.getValue() << " in " << time_us << " µs" << std::endl;

    return EXIT_SUCCESS;
}


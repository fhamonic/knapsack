#include <filesystem>
#include <iostream>

#include "knapsack/dynamic_programming.hpp"

#include "utils/chrono.hpp"
#include "utils/instance_parsers.hpp"

namespace Knapsack = fhamonic::knapsack;

int main(int argc, const char * argv[]) {
    if(argc < 2) {
        std::cerr << "input requiered : <knapsack_instance_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::filesystem::path instance_path = argv[1];
    if(!std::filesystem::exists(instance_path)) {
        std::cerr << instance_path << ":"
                  << " File does not exists" << std::endl;
        return EXIT_FAILURE;
    }

    Knapsack::Instance instance = parse_tp_instance(instance_path);
    // Knapsack::Instance instance = parse_classic_instance(instance_path);
    Knapsack::DynamicProgramming<int, int> solver;

    Chrono chrono;
    Knapsack::Solution solution = solver.solve(instance);
    int time_us = chrono.timeUs();

    std::cout << solution.getValue() << " in " << time_us << " µs" << std::endl;

    return EXIT_SUCCESS;
}

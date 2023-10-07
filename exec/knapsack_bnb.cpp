#include <filesystem>
#include <iostream>

#include "knapsack/knapsack_bnb.hpp"

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

    Instance instance = parse_tp_instance(instance_path);
    // Instance instance = parse_classic_instance(instance_path);

    Chrono chrono;

    auto knapsack = Knapsack::knapsack_bnb(
        instance.getBudget(), instance.items(),
        [&instance](const Instance<int, int>::Item & i) {
            return i.value;
        },
        [&instance](const Instance<int, int>::Item & i) {
            return i.cost;
        });

    knapsack.solve();
    // knapsack.solve(std::chrono::seconds(10));

    int time_us = chrono.timeUs();

    int solution_value = 0;
    for(auto && i : knapsack.solution()) {     
        // std::cout << i.value << " " << i.cost << std::endl;
        solution_value += i.value;
    }
    std::cout << solution_value << " in " << time_us << " µs" << std::endl;

    return EXIT_SUCCESS;
}

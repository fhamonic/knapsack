#include <filesystem>
#include <iostream>

#include "knapsack/unbounded_knapsack_bnb.hpp"

#include "utils/chrono.hpp"
#include "utils/instance_parsers.hpp"

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

    Instance<int, int> instance =
        parse_unbounded_instance(instance_path);

    Chrono chrono;

    auto unbounded_knapsack = fhamonic::knapsack::unbounded_knapsack_bnb(
        instance.getBudget(), instance.items(),
        [&instance](const Instance<int, int>::Item & i) {
            return i.value;
        },
        [&instance](const Instance<int, int>::Item & i) {
            return i.cost;
        });

    // unbounded_knapsack.solve();
    unbounded_knapsack.solve(std::chrono::seconds(30));

    int time_us = chrono.timeUs();

    int solution_value = 0;
    for(auto && [i, nb] : unbounded_knapsack.solution()) {
        // std::cout << i.value << " " << i.cost << std::endl;
        solution_value += i.value * nb;
    }
    std::cout << solution_value << " in " << time_us << " µs" << std::endl;

    return EXIT_SUCCESS;
}

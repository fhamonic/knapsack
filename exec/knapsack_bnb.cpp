// #include <filesystem>
// #include <iostream>

// #include "knapsack/branch_and_bound.hpp"
// // #include "knapsack/parallel_branch_and_bound.hpp"

// #include "utils/chrono.hpp"
// #include "utils/instance_parsers.hpp"

// namespace Knapsack = fhamonic::knapsack;

// int main(int argc, const char * argv[]) {
//     if(argc < 2) {
//         std::cerr << "input requiered : <knapsack_instance_file>" <<
//         std::endl; return EXIT_FAILURE;
//     }
//     std::filesystem::path instance_path = argv[1];
//     if(!std::filesystem::exists(instance_path)) {
//         std::cerr << instance_path << ":"
//                   << " File does not exists" << std::endl;
//         return EXIT_FAILURE;
//     }

//     Knapsack::Instance instance = parse_tp_instance(instance_path);
//     // Knapsack::Instance instance = parse_classic_instance(instance_path);
//     Knapsack::BranchAndBound<int, int> solver;
//     // Knapsack::ParallelBranchAndBound solver(instance);

//     Chrono chrono;
//     Knapsack::Solution solution = solver.solve(instance);
//     int time_us = chrono.timeUs();

//     std::cout << solution.getValue() << " in " << time_us << " µs" <<
//     std::endl;

//     return EXIT_SUCCESS;
// }

#include <filesystem>
#include <iostream>

#include "knapsack/branch_and_bound_stl.hpp"

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

    // Knapsack::Instance instance = parse_tp_instance(instance_path);
    Knapsack::Instance instance = parse_classic_instance(instance_path);

    Chrono chrono;

    auto knapsack = Knapsack::knapsack_bnb(
        instance.getBudget(), instance.items(),
        [&instance](const Knapsack::Instance<int, int>::Item & i) {
            return i.value;
        },
        [&instance](const Knapsack::Instance<int, int>::Item & i) {
            return i.cost;
        });

    knapsack.solve_jthread(1);

    int time_us = chrono.timeUs();

    int solution_value = 0;
    for(auto && i : knapsack.solution()) {     
        // std::cout << i.value << " " << i.cost << std::endl;
        solution_value += i.value;
    }
    std::cout << solution_value << " in " << time_us << " µs" << std::endl;

    return EXIT_SUCCESS;
}

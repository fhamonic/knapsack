#include <iostream>
#include <filesystem>
#include <fstream>

#include "branch_and_bound.hpp"

#include "utils/chrono.hpp"

Knapstack::Instance<int,int> parse_tp_instance(const std::filesystem::path & instance_path) {
    Knapstack::Instance<int,int> instance;
    std::ifstream file(instance_path);
    int budget;
    file >> budget;
    instance.setBudget(budget);
    int value, weight;
    while(file >> weight >> value) instance.addItem(value, weight);
    return instance;
}

Knapstack::Instance<int,int> parse_classic_instance(const std::filesystem::path & instance_path) {
    Knapstack::Instance<int,int> instance;
    std::ifstream file(instance_path);
    int nb_items, budget;
    file >> nb_items >> budget;
    instance.setBudget(budget);
    int value, weight;
    for(int i=0; i<nb_items; ++i) {
        file >> weight >> value;
        instance.addItem(value, weight);
    }
    return instance;
}

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
    
    Knapstack::Instance instance = parse_classic_instance(instance_path);
    Knapstack::BranchAndBound solver(instance);

    Chrono chrono;
    Knapstack::Solution solution = solver.solve();
    int time_us = chrono.timeUs();

    std::cout << solution.getValue() << " in " << time_us << " µs" << std::endl;

    return EXIT_SUCCESS;
}


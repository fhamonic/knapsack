#ifndef INSTANCE_PARSER_HPP
#define INSTANCE_PARSER_HPP

#include <filesystem>
#include <fstream>
#include <iostream>

#include "knapsack/instance.hpp"
#include "unbounded_knapsack/instance.hpp"

inline fhamonic::knapsack::Instance<int, int> parse_tp_instance(
    const std::filesystem::path & instance_path) {
    fhamonic::knapsack::Instance<int, int> instance;
    std::ifstream file(instance_path);
    int budget;
    file >> budget;
    instance.setBudget(budget);
    int value, weight;
    while(file >> weight >> value) instance.addItem(value, weight);
    return instance;
}

inline fhamonic::knapsack::Instance<int, int> parse_classic_instance(
    const std::filesystem::path & instance_path) {
    fhamonic::knapsack::Instance<int, int> instance;
    std::ifstream file(instance_path);
    int nb_items, budget;
    file >> nb_items >> budget;
    instance.setBudget(budget);
    int value, weight;
    for(int i = 0; i < nb_items; ++i) {
        file >> weight >> value;
        instance.addItem(weight, value);
    }
    int taken, opt = 0;
    for(int i = 0; file >> taken; ++i) opt += taken * instance[i].value;
    return instance;
}

inline fhamonic::unbounded_knapsack::Instance<int, int> parse_unbounded_instance(
    const std::filesystem::path & instance_path) {
    fhamonic::unbounded_knapsack::Instance<int, int> instance;
    std::ifstream file(instance_path);
    int nb_items, budget;
    file >> nb_items >> budget;
    instance.setBudget(budget);
    int value, weight;
    for(int i = 0; i < nb_items; ++i) {
        file >> value >> weight;
        instance.addItem(weight, value);
    }
    int opt;
    file >> opt;
    std::cout << "opt = " << opt << std::endl;
    return instance;
}

#endif  // INSTANCE_PARSER_HPP
#include "gtest/gtest.h"

#include <filesystem>
#include <iostream>

#include "knapsack/branch_and_bound.hpp"
#include "knapsack/dynamic_programming.hpp"
#include "utils/instance_parsers.hpp"

namespace Knapsack = fhamonic::knapsack;
namespace UnboundedKnapsack = fhamonic::unbounded_knapsack;

std::vector<std::pair<Knapsack::Instance<int, int>, double>> instances;

class Environment : public ::testing::Environment {
public:
    void SetUp() {
        instances.emplace_back(parse_tp_instance("../../instances/knapsack/sac0"),
                               103);
        instances.emplace_back(parse_tp_instance("../../instances/knapsack/sac1"),
                               2077672);
        instances.emplace_back(parse_tp_instance("../../instances/knapsack/sac2"),
                               2095878);
        instances.emplace_back(parse_tp_instance("../../instances/knapsack/sac3"),
                               2132531);
        instances.emplace_back(parse_tp_instance("../../instances/knapsack/sac4"),
                               2166542);
    }
};

int main(int argc, char ** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment);
    return RUN_ALL_TESTS();
}

TEST(KnapsackBNB, OptTest) {
    Knapsack::BranchAndBound<int, int> solver;
    for(const auto & [instance, opt] : instances) {
        Knapsack::Solution<int, int> solution = solver.solve(instance);
        EXPECT_EQ(solution.getValue(), opt);
    }
}

TEST(KnapsackDP, OptTest) {
    Knapsack::DynamicProgramming<int, int> solver;
    for(const auto & [instance, opt] : instances) {
        Knapsack::Solution<int, int> solution = solver.solve(instance);
        EXPECT_EQ(solution.getValue(), opt);
    }
}

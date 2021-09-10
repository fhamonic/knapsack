#include "gtest/gtest.h"

#include <filesystem>
#include <iostream>

#include "knapstack/branch_and_bound.hpp"
#include "knapstack/dynamic_programming.hpp"
#include "utils/instance_parsers.hpp"

std::vector<std::pair<Knapstack::Instance<int, int>, double>> instances;

class Environment : public ::testing::Environment {
public:
    void SetUp() {
        instances.emplace_back(parse_tp_instance("instances/knapstack/sac0"),
                               103);
        instances.emplace_back(parse_tp_instance("instances/knapstack/sac1"),
                               103);
        instances.emplace_back(parse_tp_instance("instances/knapstack/sac2"),
                               103);
        instances.emplace_back(parse_tp_instance("instances/knapstack/sac3"),
                               103);
        instances.emplace_back(parse_tp_instance("instances/knapstack/sac4"),
                               103);
    }
};

int main(int argc, char ** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment);
    return RUN_ALL_TESTS();
}

TEST(KnapstackBNB, OptTest) {
    Knapstack::BranchAndBound<int, int> solver;
    for(const auto & [instance, opt] : instances) {
        Knapstack::Solution<int, int> solution = solver.solve(instance);
        EXPECT_EQ(solution.getValue(), 103);
    }
}

TEST(KnapstackDP, OptTest) {
    Knapstack::DynamicProgramming<int, int> solver;
    for(const auto & [instance, opt] : instances) {
        Knapstack::Solution<int, int> solution = solver.solve(instance);
        EXPECT_EQ(solution.getValue(), 103);
    }
}

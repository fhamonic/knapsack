#ifndef FHAMONIC_KNAPSACK_INSTANCE_HPP
#define FHAMONIC_KNAPSACK_INSTANCE_HPP

#include <limits>
#include <vector>

namespace fhamonic {
namespace knapsack {

template <typename Value, typename Cost>
class Instance {
public:
    class Item {
    public:
        Value value;
        Cost cost;

    public:
        Item(Value v, Cost c) : value{v}, cost{c} {}
        Item(const Item & item) : value{item.value}, cost{item.cost} {}
        double getRatio() const {
            if(cost == 0) return std::numeric_limits<double>::max();
            return static_cast<double>(value) / static_cast<double>(cost);
        }
        bool operator<(const Item & other) const {
            return getRatio() > other.getRatio();
        }
    };

private:
    Cost budget;
    std::vector<Item> items;

public:
    Instance() {}

    void setBudget(Cost b) { budget = b; }
    Cost getBudget() const { return budget; }

    void addItem(Value v, Cost w) { items.push_back(Item(v, w)); }
    size_t itemCount() const { return items.size(); }

    const std::vector<Item> & getItems() const { return items; }
    const Item getItem(const std::size_t i) const { return items[static_cast<std::size_t>(i)]; }
    const Item operator[](const std::size_t i) const { return items[i]; }
};

}  // namespace Knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_INSTANCE_HPP
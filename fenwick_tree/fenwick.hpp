#include <iostream>
#include <ranges>
#include <vector>

template <typename T, typename Operator = decltype([](const T& lhs, const T& rhs) { return lhs + rhs; }),
          typename Inverse = decltype([](const T& lhs, const T& rhs) { return lhs - rhs; }), T baseVal = 0>
class Fenwick {
    std::vector<T> _tree;
    size_t _size;

    [[nodiscard]] static auto inline getParent(size_t value) -> size_t { return value | (value + 1); }
    [[nodiscard]] static auto inline getChild(size_t value) -> size_t { return value & (value + 1); }
    auto build(const std::vector<T>& data) -> void {
        for (auto [index, value] : std::views::enumerate(data)) {
            _tree[index] = Operator {}(_tree[index], value);
            size_t parent = getParent(index);
            if (parent < _size) {
                _tree[parent] = Operator {}(_tree[parent], _tree[index]);
            }
        }
    }

    [[nodiscard]] auto getRange(long right) const -> T {
        T result = baseVal;
        // splice togther the binary ranges to get the total
        for (; right >= 0; right = getChild(right) - 1) {
            result = Operator {}(result, _tree[right]);
        }
        return result;
    }

public:
    Fenwick(size_t size)
        : _size { size } {
        _tree.resize(_size, baseVal);
    }

    Fenwick(const std::vector<T> data)
        : Fenwick(data.size()) {
        build(data);
    }

    [[nodiscard]] auto getRange(size_t left, size_t right) const -> std::optional<T> {
        if (left > right || right >= _size) {
            return {};
        }
        return Inverse {}(getRange(right), getRange(static_cast<long>(left) - 1));
    }

    auto update(size_t index, const T& delta) -> void {
        // add delta value to children, then update parent ranges that
        // contain this element
        for (; index < _size; index = getParent(index)) {
            _tree[index] = Operator {}(_tree[index], delta);
        }
    }

    [[nodiscard]] auto size() const noexcept -> size_t { return _size; }
};

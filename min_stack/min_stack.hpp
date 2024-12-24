#include <iterator>
#include <optional>
#include <vector>

template <typename T, typename Compare = std::less<T>>
class MinStack {
    std::vector<std::pair<T, T>> _stack;

public:
    template <typename Iter>
    MinStack(Iter begin, Iter end) {
        for (; begin != end; ++begin) {
            this->push(*begin);
        }
    }

    MinStack() = default;

    MinStack(MinStack&&) = delete;
    auto operator=(const MinStack&) -> MinStack& = delete;
    auto operator=(MinStack&& other) noexcept -> MinStack& { std::swap(_stack, other._stack); }
    explicit MinStack(std::vector<std::pair<T, T>> stack)
        : _stack(std::move(stack)) {}
    MinStack(const MinStack& ms) = delete;

    auto push(const T& item) -> void {
        if (_stack.empty()) {
            _stack.emplace_back(item, item);
            return;
        }
        _stack.emplace_back(item, std::min(item, _stack.back().second, Compare {}));
    }

    [[nodiscard]] auto top() noexcept -> std::optional<T> {
        if (_stack.empty()) {
            return {};
        }
        return _stack.back().first;
    }

    auto pop() -> void {
        if (_stack.empty()) {
            return;
        }
        _stack.pop_back();
    }

    [[nodiscard]] auto getMin() -> std::optional<T> {
        if (_stack.empty()) {
            return {};
        }
        return _stack.back().second;
    }

    [[nodiscard]] auto size() const noexcept -> size_t { return _stack.size(); }

    [[nodiscard]] auto empty() const noexcept -> bool { return _stack.size() == 0; }
};

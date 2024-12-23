#pragma once

#include <cstdlib>
#include <vector>

template <typename T, typename NodeVal = T,
          typename Op = decltype([](const NodeVal& a, const NodeVal& b) { return a + b; }),
          typename Base = decltype([](const T& data) -> NodeVal { return static_cast<NodeVal>(data); })>
class SegmentTree {
    std::vector<NodeVal> _tree;
    size_t _size;

    auto buldTree(const std::vector<T>& data, size_t index, size_t l, size_t r) -> NodeVal {
        if (l == r) {
            return (_tree[index] = Base {}(data[l]));
        }

        size_t middle = l + (r - l) / 2;
        NodeVal left = buldTree(data, leftChild(index), l, middle);
        NodeVal right = buldTree(data, rightChild(index), middle + 1, r);

        return (_tree[index] = Op {}(left, right));
    }

    [[nodiscard]] auto query(size_t index, size_t ql, size_t qr, size_t cl, size_t cr) const -> NodeVal {
        if (ql == cl && qr == cr) {
            return _tree[index];
        }
        size_t middle = cl + (cr - cl) / 2;
        if (qr <= middle) {
            return query(leftChild(index), ql, qr, cl, middle);
        }
        if (middle + 1 <= ql) {
            return query(rightChild(index), ql, qr, middle + 1, cr);
        }
        NodeVal left = query(leftChild(index), ql, middle, cl, middle);
        NodeVal right = query(rightChild(index), middle + 1, qr, middle + 1, cr);
        return Op {}(left, right);
    }

    auto update(size_t index, size_t left, size_t right, size_t updateIndex, const T& value) -> void {
        if (left == right) {
            _tree[index] = Base {}(value);
            return;
        }

        size_t middle = left + (right - left) / 2;
        if (updateIndex <= middle) {
            update(leftChild(index), left, middle, updateIndex, value);
        } else {
            update(rightChild(index), middle + 1, right, updateIndex, value);
        }

        _tree[index] = Op {}(_tree[leftChild(index)], _tree[rightChild(index)]);
    }

protected:
    static inline auto leftChild(size_t index) -> size_t { return 2 * index + 1; }
    static inline auto rightChild(size_t index) -> size_t { return 2 * index + 2; }
    auto getTree() -> std::vector<NodeVal>& { return _tree; }
    [[nodiscard]] auto getSize() const noexcept -> size_t { return _size; }

public:
    SegmentTree(const std::vector<T>& data)
        : _size { data.size() } {
        _tree.resize(4 * _size);
        buldTree(data, 0, 0, _size - 1);
    }

    [[nodiscard]] auto query(size_t left, size_t right) const -> NodeVal { return query(0, left, right, 0, _size - 1); }

    auto update(size_t updateIndex, const T& value) -> void { update(0, 0, _size - 1, updateIndex, value); }
};

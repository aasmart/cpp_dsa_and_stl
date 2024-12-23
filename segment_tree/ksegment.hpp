#pragma once

#include <optional>

#include "segment.hpp"

template <typename T, T value>
class kSegmentTree
    : public SegmentTree<T, size_t, decltype([](size_t a, size_t b) { return a + b; }),
                         decltype([](size_t data) -> size_t { return data == value; })> {
    [[nodiscard]] auto findKthIndex(size_t k, size_t index, size_t left, size_t right) -> std::optional<size_t> {
        if (k > this->getTree()[index]) {
            return {};
        }
        if (left >= right) {
            return left;
        }
        size_t middle = left + (right - left) / 2;
        size_t leftCount = this->getTree()[this->leftChild(index)];
        if (k <= leftCount) {
            return findKthIndex(k, this->leftChild(index), left, middle);
        }
        return findKthIndex(k - leftCount, this->rightChild(index), middle + 1, right);
    }

public:
    [[nodiscard]] auto findKthIndex(size_t k) -> std::optional<size_t> {
        return findKthIndex(k, 0, 0, this->getSize() - 1);
    }
};

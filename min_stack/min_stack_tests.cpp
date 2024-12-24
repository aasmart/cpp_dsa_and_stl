#include <optional>

#include <gtest/gtest.h>

#include "min_stack.hpp"

TEST(MinStackTest, Simple) {
    MinStack<int> ms;

    ASSERT_TRUE(ms.empty());

    ms.push(10);
    ms.push(14);

    ASSERT_EQ(ms.top().value(), 14);
    ASSERT_EQ(ms.getMin().value(), 10);
    ASSERT_EQ(ms.size(), 2);

    ms.push(8);
    ASSERT_EQ(ms.top(), 8);
    ASSERT_EQ(ms.getMin(), 8);

    ms.pop();
    ASSERT_EQ(ms.top(), 14);
    ASSERT_EQ(ms.getMin(), 10);
}

TEST(MinStackTest, FillConstructor) {
    std::vector<int> nums = { 5, 3, 9, 12, 2, 1 };
    MinStack<int> ms { nums.begin(), nums.end() };

    ASSERT_EQ(ms.size(), nums.size());
    ASSERT_EQ(ms.top().value(), 1);
    ASSERT_EQ(ms.getMin().value(), 1);

    ms.pop();

    ASSERT_EQ(ms.getMin().value(), 2);
    ASSERT_EQ(ms.top().value(), 2);

    ms.pop();

    ASSERT_EQ(ms.getMin().value(), 3);
    ASSERT_EQ(ms.top().value(), 12);

    while (!ms.empty()) {
        ms.pop();
    }

    ASSERT_EQ(ms.top(), std::nullopt);
}

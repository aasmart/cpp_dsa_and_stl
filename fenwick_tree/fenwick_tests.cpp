#include <algorithm>
#include <cstddef>
#include <optional>
#include <random>

#include <strings.h>

#include <gtest/gtest.h>

#include "fenwick.hpp"

TEST(FenwickTest, Empty) {
    Fenwick<int> f(0);
    EXPECT_EQ(f.getRange(0, 0), std::nullopt);
    EXPECT_EQ(f.getRange(2, 0), std::nullopt);
    EXPECT_EQ(f.getRange(0, 2), std::nullopt);

    EXPECT_EQ(f.size(), 0);
}

TEST(FenwickTest, SingleDefaultConstructor) {
    Fenwick<int> f { 1 };
    EXPECT_EQ(f.getRange(0, 0).value(), 0);
    f.update(0, 5);
    EXPECT_EQ(f.getRange(0, 0).value(), 5);
    EXPECT_EQ(f.getRange(0, 1), std::nullopt);
    EXPECT_EQ(f.getRange(1, 2), std::nullopt);

    f.update(1, 3);
    EXPECT_EQ(f.getRange(0, 0), 5);

    f.update(0, -3);
    EXPECT_EQ(f.getRange(0, 0), 2);
    EXPECT_EQ(f.size(), 1);
}

TEST(FenwickTest, DoubleDefaultConstructor) {
    Fenwick<int> f { 2 };
    EXPECT_EQ(f.getRange(0, 1).value(), 0);
    f.update(0, 5);
    EXPECT_EQ(f.getRange(0, 0).value(), 5);
    EXPECT_EQ(f.getRange(1, 1).value(), 0);
    EXPECT_EQ(f.getRange(0, 1).value(), 5);

    f.update(1, 3);
    EXPECT_EQ(f.getRange(0, 0).value(), 5);
    EXPECT_EQ(f.getRange(1, 1).value(), 3);
    EXPECT_EQ(f.getRange(0, 1).value(), 8);

    f.update(0, -2);
    EXPECT_EQ(f.getRange(0, 0).value(), 3);
    EXPECT_EQ(f.getRange(1, 1).value(), 3);
    EXPECT_EQ(f.getRange(0, 1).value(), 6);

    f.update(2, 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 3);
    EXPECT_EQ(f.getRange(1, 1).value(), 3);
    EXPECT_EQ(f.getRange(0, 1).value(), 6);

    EXPECT_EQ(f.size(), 2);
}

TEST(FenwickTest, VectorConstructor) {
    std::vector<int> data = { 6, 7, 1, 4, 6, 3, -1, 2, 8, 9 };
    Fenwick<int> f { data };
    EXPECT_EQ(f.size(), data.size());

    EXPECT_EQ(f.getRange(0, 0).value(), 6);
    EXPECT_EQ(f.getRange(1, 1).value(), 7);
    EXPECT_EQ(f.getRange(0, 1).value(), 13);
    EXPECT_EQ(f.getRange(0, 2).value(), 14);

    EXPECT_EQ(f.getRange(0, 3).value(), 18);
    EXPECT_EQ(f.getRange(1, 3).value(), 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 6);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 0, [](int current, int v) { return current + v; }));

    f.update(0, 3);
    data[0] += 3;

    EXPECT_EQ(f.getRange(0, 3).value(), 21);
    EXPECT_EQ(f.getRange(1, 3).value(), 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 9);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 0, [](int current, int v) { return current + v; }));

    f.update(7, -2);
    data[7] += -2;

    EXPECT_EQ(f.getRange(0, 3).value(), 21);
    EXPECT_EQ(f.getRange(1, 3).value(), 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 9);
    EXPECT_EQ(f.getRange(7, 7).value(), 0);
    EXPECT_EQ(f.getRange(7, 9).value(), 17);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 0, [](int current, int v) { return current + v; }));
}

TEST(FenwickTest, Multiplication) {
    std::vector<int> data = { 6, 7, 1, 4, 6, 3, 7, 2, 8, 9 };
    Fenwick<int, decltype([](const auto& lhs, const auto& rhs) { return lhs * rhs; }),
            decltype([](const auto& lhs, const auto& rhs) { return lhs / rhs; }), 1>
      f { data };
    EXPECT_EQ(f.size(), data.size());

    EXPECT_EQ(f.getRange(1, 1).value(), 7);
    EXPECT_EQ(f.getRange(0, 1).value(), 42);
    EXPECT_EQ(f.getRange(0, 2).value(), 42);

    EXPECT_EQ(f.getRange(0, 3).value(), 42 * 4);
    EXPECT_EQ(f.getRange(1, 3).value(), 7 * 1 * 4);
    EXPECT_EQ(f.getRange(0, 0).value(), 6);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 1, [](int current, int v) { return current * v; }));

    f.update(0, 3);
    data[0] *= 3;

    EXPECT_EQ(f.getRange(0, 3).value(), 18 * 7 * 1 * 4);
    EXPECT_EQ(f.getRange(1, 3).value(), 7 * 1 * 4);
    EXPECT_EQ(f.getRange(0, 0).value(), 18);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 1, [](int current, int v) { return current * v; }));

    f.update(7, -2);
    data[7] *= -2;

    EXPECT_EQ(f.getRange(0, 3).value(), 18 * 7 * 1 * 4);
    EXPECT_EQ(f.getRange(1, 3).value(), 7 * 1 * 4);
    EXPECT_EQ(f.getRange(0, 0).value(), 18);
    EXPECT_EQ(f.getRange(7, 7).value(), -4);
    EXPECT_EQ(f.getRange(7, 9).value(), -4 * 8 * 9);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 1, [](int current, int v) { return current * v; }));
}

TEST(FenwickTest, LargeRandom) {
    std::vector<int> data(1000);
    std::mt19937 mt { 100 };
    std::uniform_int_distribution numDist { -1000, 1000 };
    std::bernoulli_distribution optDist { 0.5 };
    std::uniform_int_distribution indexDist { 0, static_cast<int>(data.size()) - 1 };

    std::ranges::generate(data, [&]() { return numDist(mt); });

    Fenwick<int> f { data };

    for (auto _ : std::views::iota(0, 100000)) {
        if (optDist(mt)) {
            auto [lower, upper] = std::minmax(indexDist(mt), indexDist(mt));
            ASSERT_EQ(f.getRange(lower, upper), std::ranges::fold_left(data.begin() + lower, data.begin() + upper + 1,
                                                                       0, [](int acc, int v) { return acc + v; }));
        } else {
            auto index = indexDist(mt);
            auto delta = numDist(mt);
            f.update(index, delta);
            data[index] += delta;
        }
    }
}

TEST(FenwickOneIndexTest, Empty) {
    OneBasedFenwick<int> f(0);
    EXPECT_EQ(f.getRange(0, 0), std::nullopt);
    EXPECT_EQ(f.getRange(2, 0), std::nullopt);
    EXPECT_EQ(f.getRange(0, 2), std::nullopt);

    EXPECT_EQ(f.size(), 0);
}

TEST(FenwickOneIndexTest, SingleDefaultConstructor) {
    OneBasedFenwick<int> f { 1 };
    EXPECT_EQ(f.getRange(0, 0).value(), 0);
    f.update(0, 5);
    EXPECT_EQ(f.getRange(0, 0).value(), 5);
    EXPECT_EQ(f.getRange(0, 1), std::nullopt);
    EXPECT_EQ(f.getRange(1, 2), std::nullopt);

    f.update(1, 3);
    EXPECT_EQ(f.getRange(0, 0), 5);

    f.update(0, -3);
    EXPECT_EQ(f.getRange(0, 0), 2);
    EXPECT_EQ(f.size(), 1);
}

TEST(FenwickOneIndexTest, DoubleDefaultConstructor) {
    OneBasedFenwick<int> f { 2 };
    EXPECT_EQ(f.getRange(0, 1).value(), 0);
    f.update(0, 5);
    EXPECT_EQ(f.getRange(0, 0).value(), 5);
    EXPECT_EQ(f.getRange(1, 1).value(), 0);
    EXPECT_EQ(f.getRange(0, 1).value(), 5);

    f.update(1, 3);
    EXPECT_EQ(f.getRange(0, 0).value(), 5);
    EXPECT_EQ(f.getRange(1, 1).value(), 3);
    EXPECT_EQ(f.getRange(0, 1).value(), 8);

    f.update(0, -2);
    EXPECT_EQ(f.getRange(0, 0).value(), 3);
    EXPECT_EQ(f.getRange(1, 1).value(), 3);
    EXPECT_EQ(f.getRange(0, 1).value(), 6);

    f.update(2, 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 3);
    EXPECT_EQ(f.getRange(1, 1).value(), 3);
    EXPECT_EQ(f.getRange(0, 1).value(), 6);

    EXPECT_EQ(f.size(), 2);
}

TEST(FenwickOneIndexTest, VectorConstructor) {
    std::vector<int> data = { 6, 7, 1, 4, 6, 3, -1, 2, 8, 9 };
    OneBasedFenwick<int> f { data };
    EXPECT_EQ(f.size(), data.size());

    EXPECT_EQ(f.getRange(0, 0).value(), 6);
    EXPECT_EQ(f.getRange(1, 1).value(), 7);
    EXPECT_EQ(f.getRange(0, 1).value(), 13);
    EXPECT_EQ(f.getRange(0, 2).value(), 14);

    EXPECT_EQ(f.getRange(0, 3).value(), 18);
    EXPECT_EQ(f.getRange(1, 3).value(), 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 6);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 0, [](int current, int v) { return current + v; }));

    f.update(0, 3);
    data[0] += 3;

    EXPECT_EQ(f.getRange(0, 3).value(), 21);
    EXPECT_EQ(f.getRange(1, 3).value(), 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 9);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 0, [](int current, int v) { return current + v; }));

    f.update(7, -2);
    data[7] += -2;

    EXPECT_EQ(f.getRange(0, 3).value(), 21);
    EXPECT_EQ(f.getRange(1, 3).value(), 12);
    EXPECT_EQ(f.getRange(0, 0).value(), 9);
    EXPECT_EQ(f.getRange(7, 7).value(), 0);
    EXPECT_EQ(f.getRange(7, 9).value(), 17);
    EXPECT_EQ(f.getRange(0, 9).value(),
              std::ranges::fold_left(data, 0, [](int current, int v) { return current + v; }));
}

TEST(FenwickOneIndexTest, LargeRandom) {
    std::vector<int> data(1000);
    std::mt19937 mt { 100 };
    std::uniform_int_distribution numDist { -1000, 1000 };
    std::bernoulli_distribution optDist { 0.5 };
    std::uniform_int_distribution indexDist { 0, static_cast<int>(data.size()) - 1 };

    std::ranges::generate(data, [&]() { return numDist(mt); });

    OneBasedFenwick<int> f { data };

    for (auto _ : std::views::iota(0, 100000)) {
        if (optDist(mt)) {
            auto [lower, upper] = std::minmax(indexDist(mt), indexDist(mt));
            ASSERT_EQ(f.getRange(lower, upper), std::ranges::fold_left(data.begin() + lower, data.begin() + upper + 1,
                                                                       0, [](int acc, int v) { return acc + v; }));
        } else {
            auto index = indexDist(mt);
            auto delta = numDist(mt);
            f.update(index, delta);
            data[index] += delta;
        }
    }
}

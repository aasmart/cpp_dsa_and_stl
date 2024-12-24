#include <algorithm>
#include <cstddef>
#include <random>

#include "gtest/gtest.h"
#include "ksegment.hpp"
#include "segment.hpp"

TEST(SegmentTreeTest, SegmentTreeSingle) {
    std::vector<int> nums = { 1 };
    SegmentTree<int> st(nums);
    ASSERT_EQ(st.query(0, 0), 1);
    st.update(0, 2);
    ASSERT_EQ(st.query(0, 0), 2);
}

TEST(SegmentTreeTest, SegmentTreeQueryUpdateSimple) {
    std::vector<int> nums = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    SegmentTree<int> st(nums);
    ASSERT_EQ(st.query(0, 0), 1);
    ASSERT_EQ(st.query(1, 1), 2);
    ASSERT_EQ(st.query(0, 1), 3);
    ASSERT_EQ(st.query(3, 7), 30);
    ASSERT_EQ(st.query(4, 8), 35);
    ASSERT_EQ(st.query(4, 6), 18);
}

TEST(SegmentTreeTest, BasicSegmentTreeTestRandmoQuery) {
    std::mt19937 mt {};
    mt.seed(312);

    std::vector<int> randomVec(100, 0);
    std::iota(randomVec.begin(), randomVec.end(), 0);
    std::shuffle(randomVec.begin(), randomVec.end(), mt);

    std::uniform_int_distribution<int> dist { 0, static_cast<int>(randomVec.size() - 1) };
    SegmentTree<int> st2(randomVec);
    for (int i = 0; i < 1000000; ++i) {
        size_t a = dist(mt);
        size_t b = dist(mt);

        size_t lower = std::min(a, b);
        size_t upper = std::max(a, b);

        int query = st2.query(lower, upper);
        int acc = std::accumulate(randomVec.begin() + lower, randomVec.begin() + upper + 1, 0);
        ASSERT_EQ(query, acc);
    }
}

TEST(SegmentTreeTest, BasicSegmentTreeTestRandomUpdateQuery) {
    std::mt19937 mt {};
    mt.seed(312);

    std::vector<int> randomVec(100, 0);
    std::iota(randomVec.begin(), randomVec.end(), 0);
    std::shuffle(randomVec.begin(), randomVec.end(), mt);

    std::uniform_int_distribution<int> dist { 0, static_cast<int>(randomVec.size() - 1) };
    SegmentTree<int> st2(randomVec);
    for (int i = 0; i < 1000000; ++i) {
        size_t a = dist(mt);
        size_t b = dist(mt);
        size_t updateIndex = dist(mt);
        int updateValue = dist(mt);

        size_t lower = std::min(a, b);
        size_t upper = std::max(a, b);

        randomVec[updateIndex] = updateValue;
        st2.update(updateIndex, updateValue);
        int query = st2.query(lower, upper);
        int acc = std::accumulate(randomVec.begin() + lower, randomVec.begin() + upper + 1, 0);

        ASSERT_EQ(query, acc);
    }
}

TEST(SegmentTreeTest, MaxSegmentTreeTestRandomQueryLarge) {
    std::vector<int> randomVec(100, 0);

    std::mt19937 mt {};
    mt.seed(213);
    std::uniform_int_distribution<int> dist { 0, static_cast<int>(randomVec.size() - 1) };

    std::iota(randomVec.begin(), randomVec.end(), 0);
    std::shuffle(randomVec.begin(), randomVec.end(), mt);

    SegmentTree<int, int, decltype([](int left, int right) { return std::max(left, right); })> stMax(randomVec);
    for (int i = 0; i < 1000000; ++i) {
        size_t a = dist(mt);
        size_t b = dist(mt);

        size_t lower = std::min(a, b);
        size_t upper = std::max(a, b);

        int query = stMax.query(lower, upper);
        int acc = *std::ranges::max_element(randomVec.begin() + lower, randomVec.begin() + upper + 1);
        ASSERT_EQ(query, acc);
    }
}

TEST(SegmentTreeTest, MaxCountSegmentTreeTestInsertDeleteRandomLarge) {
    std::vector<int> randomVec(100, 0);

    std::mt19937 mt {};
    std::uniform_int_distribution<int> dist { 0, static_cast<int>(randomVec.size() - 1) };
    mt.seed(643);

    std::vector<int> maxCountNums(100, 0);
    std::iota(maxCountNums.begin(), maxCountNums.end(), 0);
    std::shuffle(maxCountNums.begin(), maxCountNums.end(), mt);
    SegmentTree<int, std::pair<int, int>,
                decltype([](const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) -> std::pair<int, int> {
                    auto [lhsMax, lhsCount] = lhs;
                    auto [rhsMax, rhsCount] = rhs;

                    if (lhsMax == rhsMax) {
                        return { lhsMax, rhsCount + lhsCount };
                    }
                    if (lhsMax > rhsMax) {
                        return { lhsMax, lhsCount };
                    }
                    return { rhsMax, rhsCount };
                }),
                decltype([](int data) -> std::pair<int, int> { return { data, 1 }; })>
      maxCountSt { maxCountNums };

    for (int i = 0; i < 10000; ++i) {
        size_t a = dist(mt);
        size_t b = dist(mt);
        size_t updateIndex = dist(mt);
        int updateValue = dist(mt);

        size_t lower = std::min(a, b);
        size_t upper = std::max(a, b);

        maxCountNums[updateIndex] = updateValue;
        maxCountSt.update(updateIndex, updateValue);
        auto [max, count] = maxCountSt.query(lower, upper);
        int acc = *std::ranges::max_element(maxCountNums.begin() + lower, maxCountNums.begin() + upper + 1);

        ASSERT_TRUE(max == acc) << "Max test failed";

        int countIf = std::ranges::count_if(maxCountNums.begin() + lower, maxCountNums.begin() + upper + 1,
                                            [max](int num) { return num == max; });

        ASSERT_TRUE(count == countIf) << "Count test failed";
    }
}

TEST(kSegmentTreeTest, kSegmentRandomInsertUpdateLarge) {
    std::mt19937 mt {};
    mt.seed(543);

    std::vector<int> kVec(1000, 0);
    std::ranges::fill(kVec.begin(), kVec.begin() + kVec.size() / 2, 1);
    std::ranges::shuffle(kVec, mt);
    std::uniform_int_distribution<> dist3 { 0, static_cast<int>(kVec.size()) - 1 };
    kSegmentTree<int, 1> kTree(kVec);

    for (int i = 0; i < 10000; ++i) {
        size_t updateIndex = dist3(mt);

        kVec[updateIndex] = 1 - kVec[updateIndex];
        kTree.update(updateIndex, kVec[updateIndex]);
        size_t a = std::max(dist3(mt), 1);
        auto index = kTree.findKthIndex(a);

        size_t count { 0 };
        bool found { false };
        for (int i = 0; i < kVec.size(); ++i) {
            if (kVec[i] == 1) {
                ++count;
            }
            if (count == a) {
                found = true;
                count = i;
                break;
            }
        }

        long long res = index.value_or(-1);
        ASSERT_FALSE((found && res == -1) || (!found && res != -1) || (found && count != res));
    }
}

TEST(SegmentTreeTest, StringSegmentTree) {
    std::vector<char> strs = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

    auto merge = [](auto& a, auto& b) { return a + b; };
    auto base = [](char data) { return std::string(1, data); };

    SegmentTree<char, std::string, decltype(merge), decltype(base)> st(strs);

    ASSERT_EQ(st.query(0, 7), "abcdefgh");
    ASSERT_EQ(st.query(3, 7), "defgh");
    ASSERT_EQ(st.query(4, 4), "e");

    st.update(4, 'q');

    ASSERT_EQ(st.query(4, 4), "q");
    ASSERT_EQ(st.query(3, 7), "dqfgh");
    ASSERT_EQ(st.query(1, 7), "bcdqfgh");
    ASSERT_EQ(st.query(0, 7), "abcdqfgh");
    ASSERT_EQ(st.query(0, 4), "abcdq");
    ASSERT_EQ(st.query(0, 3), "abcd");
}

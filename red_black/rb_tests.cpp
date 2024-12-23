#include <gtest/gtest.h>

#include "red_black.hpp"

TEST(RedBlackTest, InsertSimple) {
    RBTree<int> tree {};
    tree.insert(10);
    tree.insert(12);
    tree.insert(13);
    tree.insert(15);
    tree.insert(17);
    tree.insert(11);
    tree.insert(9);
    tree.insert(16);
    tree.insert(10);
    tree.insert(18);

    ASSERT_EQ(tree.size(), 10) << "Failed size";
}

TEST(RedBlackTest, RandomInsertInvariantSmall) {
    std::uniform_int_distribution<> dist { -1000, 1000 };
    std::mt19937 mt {};
    mt.seed(100);

    RBTree<int> tree {};
    std::vector<int> tree2Data;
    for (uint32_t i = 0; i < 20; ++i) {
        int num = dist(mt);
        tree2Data.push_back(num);
        tree.insert(num);
        ASSERT_TRUE(tree.checkInvariant()) << "Failed insertion";
    }

    std::shuffle(tree2Data.begin(), tree2Data.end(), mt);

    for (auto i : tree2Data) {
        tree.deleteNode(i);
        ASSERT_TRUE(tree.checkInvariant()) << "Failed deletion";
    }
}

TEST(RedBlackTest, RnadomInsertInvariantLarge) {
    std::uniform_int_distribution<> dist { -1000, 1000 };
    std::mt19937 mt {};
    mt.seed(423);

    std::vector<int> treeData;
    RBTree<int> tree {};
    for (uint32_t i = 0; i < 99; ++i) {
        int num = dist(mt);
        treeData.push_back(num);
        ASSERT_TRUE(tree.checkInvariant()) << "Failed insertion";
    }

    std::shuffle(treeData.begin(), treeData.end(), mt);

    for (auto i : treeData) {
        tree.deleteNode(i);
        ASSERT_TRUE(tree.checkInvariant()) << "Failed deletion";
    }
}

TEST(RedBlackTest, RandomInsertDeleteInvariantSmall2) {
    std::uniform_int_distribution<> dist { -1000, 1000 };
    std::mt19937 mt {};
    mt.seed(789);

    std::vector<int> treeData;
    RBTree<int> tree {};
    for (uint32_t i = 0; i < 20; ++i) {
        int num = dist(mt);
        treeData.push_back(num);
        tree.insert(num);
        ASSERT_TRUE(tree.checkInvariant()) << "Failed insertion";
    }

    std::shuffle(treeData.begin(), treeData.end(), mt);

    for (auto i : treeData) {
        tree.deleteNode(i);
        ASSERT_TRUE(tree.checkInvariant()) << "Failed deletion";
    }
}

TEST(RedBlackTest, RandomInsertAndDeleteInvariantLarge) {
    std::mt19937 mt {};
    mt.seed(543);

    std::uniform_int_distribution<> dist { -100, 100 };
    std::vector<int> treeData;
    RBTree<int> tree {};
    for (uint32_t i = 0; i < 1000; ++i) {
        int num = dist(mt);
        int action = dist(mt);
        if (action <= 0) {
            tree.insert(num);
        } else {
            tree.deleteNode(num);
        }
        ASSERT_TRUE(tree.checkInvariant());
    }
}

#include <algorithm>
#include <climits>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <random>

#include <features.h>
#include <unistd.h>

template <typename T, typename Compare = std::less<T>>
class RBTree {
    enum class Color { Red, Black };

    struct Node {
        Node* left { nullptr };
        Node* right { nullptr };
        Node* parent { nullptr };
        T data {};
        Color color { Color::Red };
    };

    Node* _root {};
    size_t _size { 0 };

    inline auto isLeftChild(Node* node) -> bool {
        return node == nullptr || node->parent == nullptr ? false : node->parent->left == node;
    }

    inline auto isRightChild(Node* node) -> bool { return !isLeftChild(node); }

    inline auto isBlackNode(Node* node) const noexcept -> bool { return !node || node->color == Color::Black; }

    void rotateLeft(Node* node) {
        if (node == nullptr) {
            return;
        }
        Node* rightChild { node->right };
        if (rightChild == nullptr) {
            return;
        }

        rightChild->parent = node->parent;
        node->right = rightChild->left;
        if (rightChild->left != nullptr) {
            node->right->parent = node;
        }
        rightChild->left = node;

        // handle fixing where node's original parent points to.
        // or if it's the root
        if (rightChild->parent == nullptr) {
            _root = rightChild;
        } else if (isLeftChild(node)) {
            rightChild->parent->left = rightChild;
        } else {
            rightChild->parent->right = rightChild;
        }
        node->parent = rightChild;
    }

    void rotateRight(Node* node) {
        if (node == nullptr) {
            return;
        }
        Node* leftChild { node->left };
        if (leftChild == nullptr) {
            return;
        }

        leftChild->parent = node->parent;
        node->left = leftChild->right;
        if (leftChild->right != nullptr) {
            node->left->parent = node;
        }
        leftChild->right = node;

        // handle fixing where node's original parent points to.
        // or if it's the root
        if (leftChild->parent == nullptr) {
            _root = leftChild;
        } else if (isLeftChild(node)) {
            leftChild->parent->left = leftChild;
        } else {
            leftChild->parent->right = leftChild;
        }
        node->parent = leftChild;
    }

    void fixInsertion(Node* insert) {
        if (insert->parent->color == Color::Black) {
            return;
        }

        while (insert->parent != nullptr && insert->parent->parent != nullptr) {
            // both insert's uncle and parent are red
            Node* parent = insert->parent;
            if (parent->color != Color::Red) {
                return;
            }
            Node* grandparent = insert->parent->parent;
            Node* uncle = isLeftChild(parent) ? grandparent->right : grandparent->left;

            // uncle is red (nullptr is black colored)
            if (uncle != nullptr && uncle->color == Color::Red) {
                parent->color = Color::Black;
                uncle->color = Color::Black;
                grandparent->color = Color::Red;
            } else {
                bool leftChild = isLeftChild(insert);
                bool parentLeftChild = isLeftChild(parent);

                // update parent in the loop since we've created a "new" parent
                if ((leftChild && !parentLeftChild)) {
                    rotateRight(parent);
                    parent = insert;
                } else if (!leftChild && parentLeftChild) {
                    rotateLeft(parent);
                    parent = insert;
                }

                // since the parent is red, the grandparent must be black
                grandparent->color = Color::Red;
                parent->color = Color::Black;

                if (parentLeftChild) {
                    rotateRight(grandparent);
                } else {
                    rotateLeft(grandparent);
                }
                return;
            }
            insert = grandparent;
        }

        // this is always safe to do since it increases the black depth of
        // every path by 1
        _root->color = Color::Black;
    }

    void fixDeletion(Node* nodeToDelete) {
        Node* parent = nodeToDelete->parent;
        Node* current = nodeToDelete;

        while (parent) {
            bool leftChild { isLeftChild(current) };
            Node* sibling = leftChild ? parent->right : parent->left;

            // sibling's children have to be black
            if (sibling && sibling->color == Color::Red) {
                sibling->color = Color::Black;
                parent->color = Color::Red;
                if (leftChild) {
                    rotateLeft(parent);
                } else {
                    rotateRight(parent);
                }
                // sibling changes after rotation
                sibling = leftChild ? parent->right : parent->left;
            }

            // sibling is black
            if (isBlackNode(sibling)) {
                Node* nephewLeft = sibling->left;
                Node* nephewRight = sibling->right;
                bool siblingsChildrenBlack = !sibling || (isBlackNode(nephewLeft) && isBlackNode(nephewRight));

                Node* closeChild { leftChild ? nephewLeft : nephewRight };
                Node* farChild { leftChild ? nephewRight : nephewLeft };
                if ((closeChild && closeChild->color == Color::Red) && isBlackNode(farChild)) {
                    if (leftChild) {
                        rotateRight(sibling);
                    } else {
                        rotateLeft(sibling);
                    }
                    sibling->color = Color::Red;
                    closeChild->color = Color::Black;

                    // change after rotation relative to current node. We'll need them
                    // for later cases, so we must update them
                    farChild = sibling;
                    sibling = closeChild;
                }

                if (farChild && farChild->color == Color::Red) {
                    if (leftChild) {
                        rotateLeft(parent);
                    } else {
                        rotateRight(parent);
                    }
                    sibling->color = parent->color;
                    parent->color = Color::Black;
                    farChild->color = Color::Black;
                    break;
                }

                if (siblingsChildrenBlack) {
                    if (parent->color == Color::Red) {
                        // Decrease # black paths through sibling, but then increase it
                        // again through parent for both paths. This equalizes the total
                        // on this tree, and we may assume (by induction) that all the other
                        // trees are correct
                        sibling->color = Color::Red;
                        parent->color = Color::Black;
                        break;
                    }

                    // case that parent is black
                    // decrease the # black nodes in paths that go through sibling
                    sibling->color = Color::Red;
                }
            }

            current = current->parent;
            parent = current->parent;
        }

        if (isLeftChild(nodeToDelete)) {
            nodeToDelete->parent->left = nullptr;
        } else {
            nodeToDelete->parent->right = nullptr;
        }
        delete nodeToDelete;
        nodeToDelete = nullptr;

        _root->color = Color::Black;
    }

    // shamelessly stolen from stack overflow
    void printTree(const std::string& prefix, const Node* node, bool isLeft) {
        if (node != nullptr) {
            std::cout << prefix;

            std::cout << (isLeft ? "|-" : "L_");

            // print the value of the node
            std::cout << (node->color == Color::Red ? "R:" : "B:") << node->data << '\n';

            // enter the next tree level - left and right branch
            printTree(prefix + (isLeft ? "|   " : "    "), node->left, true);
            printTree(prefix + (isLeft ? "|   " : "    "), node->right, false);
        }
    }

    void printTree(const Node* node) { printTree("", node, false); }

    static auto checkInvariantHelper(Node* node, bool left) -> std::tuple<bool, int, int> {
        // invariant, black count, largest
        if (node == nullptr) {
            return { true, 1, left ? INT_MIN : INT_MAX };
        }

        auto [lv, lb, le] = checkInvariantHelper(node->left, true);
        auto [rv, rb, re] = checkInvariantHelper(node->right, false);

        if (!lv || !rv || (lb != rb) || (le > node->data || re < node->data)) {
            return { false, -1, left ? INT_MAX : INT_MIN };
        }

        int opt { 0 };
        if (left) {
            opt = std::max(le, node->data);
        } else {
            opt = std::min(re, node->data);
        }

        return { true, lb + (node->color == Color::Black), opt };
    }

    [[nodiscard]] static auto findInorderSuccessor(Node* root) -> Node* {
        if (root == nullptr || root->right == nullptr) {
            return root;
        }

        Node* right { root->right };
        while (right->left) {
            right = right->left;
        }
        return right;
    }

public:
    RBTree() = default;

    ~RBTree() {
        std::queue<Node*> bfs;
        bfs.push(_root);
        while (!bfs.empty()) {
            auto front = bfs.front();
            bfs.pop();
            if (!front) {
                continue;
            }
            bfs.push(front->left);
            bfs.push(front->right);

            delete front;
        }
    }

    RBTree(RBTree<T>&& tree) {
        std::swap(_root, tree._root);
        std::swap(_size, tree._size);
    }

    // RBTree(const RBTree<T>& tree) {
    //     std::queue<Node*> bfs;
    //     bfs.push(tree._root);
    //     while (!bfs.empty()) {
    //         auto front = bfs.front();
    //         bfs.pop();
    //         if (!front) {
    //             continue;
    //         }
    //         bfs.push(front->left);
    //         bfs.push(front->right);
    //     }
    // }

    // this is still missing like half of the default constructors
    // auto operator=(const RBTree<T>& tree) -> RBTree& {
    //     std::swap(_root, tree._root);
    //     std::swap(_size, tree._size);
    //     return *this;
    // }

    auto insert(T data) -> const Node* {
        Node* curr = _root;
        std::unique_ptr<Node> insert(new Node());
        insert->data = data;

        if (curr == nullptr) {
            _root = insert.release();
            _root->color = Color::Black;
            ++_size;
            return _root;
        }

        while (curr != nullptr) {
            if (Compare {}(data, curr->data)) {
                if (curr->left != nullptr) {
                    curr = curr->left;
                    continue;
                }
                insert->parent = curr;
                curr->left = insert.get();
                break;
            }
            if (curr->right != nullptr) {
                curr = curr->right;
                continue;
            }
            insert->parent = curr;
            curr->right = insert.get();
            break;
        }
        ++_size;

        fixInsertion(insert.get());
        return insert.release();
    }

    auto deleteNode(const T& data) -> bool {
        auto res = search(data);
        if (!res.has_value()) {
            return false;
        }

        --_size;

        // find and replace node with its inorder succesor (if it exists)
        // and make that the successor the node we're going to delete
        Node* node { res.value() };
        if (node->left != nullptr && node->right != nullptr) {
            auto succesor { findInorderSuccessor(node) };
            std::swap(node->data, succesor->data);
            node = succesor;
        }

        if (node->left == nullptr && node->right == nullptr) {
            if (node == _root) {
                delete node;
                _root = nullptr;
                return true;
            }
            if (node->color == Color::Red) {
                if (isLeftChild(node)) {
                    node->parent->left = nullptr;
                    delete node;
                    node = nullptr;
                } else {
                    node->parent->right = nullptr;
                    delete node;
                    node = nullptr;
                }
                return true;
            }

            fixDeletion(node);
            return true;
        }

        // safe to just swap since there's only one child at most
        // only has left child
        if (node->left && !node->right) {
            std::swap(node->data, node->left->data);
            node->color = Color::Black;
            delete node->left;
            node->left = nullptr;
            return true;
        }

        // has just right child
        std::swap(node->data, node->right->data);
        node->color = Color::Black;
        delete node->right;
        node->right = nullptr;
        return true;
    }

    [[nodiscard]] auto search(const T& val) -> std::optional<Node*> {
        Node* curr = _root;
        while (curr != nullptr) {
            if (Compare {}(val, curr->data)) {
                curr = curr->left;
            } else if (Compare {}(curr->data, val)) {
                curr = curr->right;
            } else {
                return curr;
            }
        }
        return {};
    }

    auto checkInvariant() const -> bool { return std::get<0>(checkInvariantHelper(_root, false)); }

    void print() { printTree(_root); }

    [[nodiscard]] auto size() const noexcept -> size_t { return _size; }
};

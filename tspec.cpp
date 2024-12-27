#include <iostream>
#include <type_traits>

template <typename T>
struct Foo {
    auto print() const -> void { std::cout << "Hello world, I'm an " << typeid(T).name() << '\n'; }
};

template <>
struct Foo<double> {
    auto print() const -> void { std::cout << "I'm different" << '\n'; }
};

template <typename T, int U, int Dec = 1>
struct Bar : public Bar<T, (U - Dec), Dec> {
    [[nodiscard]] constexpr auto addVal(T val) const -> T {
        val += U;
        return this->Bar<T, U - Dec, Dec>::addVal(val);
        // (Bar<T, U - 1>()).print();
    }
};

template <typename T, int Dec>
struct Bar<T, 0, Dec> {
    [[nodiscard]] constexpr auto addVal(T val) const -> T { return val; }
};

auto main() -> int {
    const Foo<int> foo;
    foo.print();

    const Foo<double> foo2;
    foo2.print();

    const Bar<int, 15, 3> bar;
    int res = bar.addVal(5);
    std::cout << res << '\n';

    const Bar<std::string, 10> bar2;
    std::string res2 = bar2.addVal("hello");
    std::cout << res2;
}

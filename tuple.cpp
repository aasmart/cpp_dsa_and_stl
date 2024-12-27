#include <tuple>

// recursive templates
// - need base case
//   * Recursive case is when there are tail items
//   * Base case is when there are no tail items
// - need recursive case

template <std::size_t i, typename Elt>
struct TupleLeaf {
    Elt v;
};

template <std::size_t i, typename... Items>
struct TupleImpl;

template <std::size_t i>
struct TupleImpl<i> {};

template <std::size_t i, typename Head, typename... Tail>
struct TupleImpl<i, Head, Tail...>
    : public TupleLeaf<i, Head>
    , public TupleImpl<i + 1, Tail...> {};

template <typename... Items>
using Tuple = TupleImpl<0, Items...>;

auto main() -> int {
    MyTuple<int, int> t(3, 3);
    t.get<0>();

    std::tuple<int, int> t2(1, );
}

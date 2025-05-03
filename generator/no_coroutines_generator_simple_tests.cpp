#include "no_coroutines_generator.h"

auto barGen(const std::string& str, size_t n) -> Generator<std::string> {
    return Generator<std::string> { [&str, n](Generator<std::string>::Yielder& gen) {
        for (size_t i = 0; i < n; ++i) {
            if (i > 8) {
                gen.yield_break();
            }
            std::string res = str + std::to_string(i);
            gen.yield(std::move(res));
        }
    } };
}

auto main() -> int {
    auto gen = Generator<int> { [](Generator<int>::Yielder& gen) {
        for (int i = 0; i < 10; ++i) {
            gen.yield(std::move(i));
        }
    } };

    auto it = gen.begin();
    std::cout << *it << std::endl;
    std::cout << "What am I doing?" << std::endl;
    std::cout << *(++it) << std::endl;
    std::for_each(++it, gen.end(), [](int i) { std::cout << i << ' '; });

    std::cout << '\n';

    auto gen2 = Generator<std::pair<std::string, int>> { [](Generator<std::pair<std::string, int>>::Yielder& gen) {
        for (int i = 0; i < 10; ++i) {
            gen.yield({ std::to_string(i), i });
        }
    } };
    std::unordered_map<std::string, int> mp(gen2.begin(), gen2.end());
    for (auto& [k, v] : mp) {
        std::cout << k << " " << v << "\n";
    }

    for (const auto& str : barGen("test", 10)) {
        std::cout << str << '\n';
    }

    auto freshGen = gen2;

    return 0;
}

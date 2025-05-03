#pragma once

#include <algorithm>
#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <ostream>
#include <print>
#include <stdexcept>
#include <thread>

template <typename T>
class Generator {
public:
    class Yielder {
    public:
        /*
         * Emit an rvalue from the generator
         */
        auto yield(T&& val) -> void {
            if (_endGen) {
                yield_break();
                return;
            }

            _currVal = std::move(val);
            _hasNextValue = true;
            _getNext = false;
            _yieldCv.notify_one();

            std::unique_lock lk { _yielding };
            _yieldCv.wait(lk, [&] { return _getNext || _endGen; });
        }

        /*
         * End the generator without eitting a value. Return may also be used
         * instead of this function.
         */
        auto yield_break() -> void {
            _currVal = std::nullopt;
            _hasNextValue = true;
            _getNext = false;
            _endGen = true;
            _yieldCv.notify_one();
        }

    private:
        /*
         * Signal to the yielder that the next value is requested
         */
        auto getNext() -> void {
            _currVal = std::nullopt;
            _getNext = true;
            _hasNextValue = false;
            _yieldCv.notify_one();
        }

        /*
         * Signal to the yielder that it should stop waiting for the
         * next signal. In other words, it stops the generator.
         */
        auto signalEnd() -> void {
            _endGen = true;
            _yieldCv.notify_one();
        }

        std::mutex _yielding;
        std::condition_variable _yieldCv;
        std::optional<T> _currVal;
        bool _hasNextValue = false;
        bool _endGen = false;
        bool _getNext = false;

        friend class Generator;
    };

    class GeneratorIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;

        auto operator*() const -> const value_type& { return _val.value(); }
        auto operator->() -> pointer { return &_val.value(); }

        auto operator++() -> GeneratorIterator& {
            _val = std::move(_next());
            return *this;
        }

        auto operator++(int) -> GeneratorIterator {
            GeneratorIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend auto operator==(const GeneratorIterator& a, const GeneratorIterator& b) -> bool {
            return a._gen == b._gen && a._val == b._val;
        };
        friend auto operator!=(const GeneratorIterator& a, const GeneratorIterator& b) -> bool {
            return a._gen != b._gen || a._val != b._val;
        };

    private:
        std::optional<value_type> _val;
        std::function<std::optional<value_type>()> _next;
        Generator* _gen;

        GeneratorIterator(std::optional<value_type> val, const std::function<std::optional<value_type>()>& next,
                          Generator* gen) noexcept
            : _val { std::move(val) }
            , _next(next)
            , _gen(gen) {}
        friend class Generator;
    };

    Generator(const std::function<void(Yielder&)>& gen)
        : _genFun(gen)
        , _yielder() {}

    Generator(const Generator<T>& other)
        : _genFun(other._genFun)
        , _yielder(Yielder()) {}

    Generator(const Generator<T>&& other) noexcept
        : _genFun(std::move(other._genFun))
        , _yielder(Yielder()) {}

    auto operator=(const Generator<T>& other) = delete;
    auto operator=(const Generator<T>&& other) = delete;

    ~Generator() {
        if (_genThread.has_value()) {
            _yielder.signalEnd();
            _genThread->join();
        }
    }

    /*
     * Get the iterator to the first element in the generator.
     * May only be called once on the existing generator, otherwise
     * an exception will be raised.
     */
    auto begin() -> GeneratorIterator {
        if (_genThread) {
            throw std::runtime_error("Generator is already being consumed");
        }

        _genThread = std::thread([&] {
            try {
                _genFun(_yielder);
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
            _yielder.yield_break();
        });

        std::unique_lock lk { _yielder._yielding };
        _yielder._yieldCv.wait(lk, [&] { return _yielder._currVal; });
        return GeneratorIterator { _yielder._currVal,
                                   [&]() {
                                       _yielder.getNext();
                                       std::unique_lock lk { _yielder._yielding };
                                       _yielder._yieldCv.wait(lk, [&] { return _yielder._hasNextValue; });
                                       return std::move(_yielder._currVal);
                                   },
                                   this };
    }

    /*
     * Iterator to indicate the end of the generation stream
     */
    auto end() -> GeneratorIterator {
        return GeneratorIterator { std::nullopt, [] { return std::nullopt; }, this };
    }

private:
    std::function<void(Yielder&)> _genFun;
    Yielder _yielder;
    std::optional<std::thread> _genThread;
};

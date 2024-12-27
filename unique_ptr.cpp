#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <type_traits>

template <typename T>
class UniquePtr {
    T* _data;

public:
    UniquePtr()
        : _data { nullptr } {}

    explicit UniquePtr(T* data) noexcept
        : _data(data) {}

    UniquePtr(UniquePtr<T>&& other) noexcept
        : _data(other._data) {
        other._data = nullptr;
    }

    UniquePtr(UniquePtr<T> const& other)
        : _data(other._data) {}

    auto operator=(const UniquePtr<T>& other) -> UniquePtr<T>& {
        UniquePtr<T> temp(other);
        swap(temp, *this);
        other._data = nullptr;
        return *this;
    }

    auto operator=(UniquePtr<T>&& other) noexcept -> UniquePtr<T>& {
        _data = other._data;
        other._data = nullptr;
        return *this;
    }

    ~UniquePtr() {
        delete _data;
        _data = nullptr;
    }

    // add lvalue reference is safer since it won't add a reference if it's not possible (e.g. void)
    auto operator*() const -> typename std::add_lvalue_reference<T>::type { return *_data; }
    auto operator->() const -> typename std::add_lvalue_reference<T>::type { return *_data; }

    auto operator[](size_t i) const -> typename std::add_lvalue_reference<T>::type { return *(_data + i); }

    [[nodiscard]] auto get() const noexcept -> T* { return _data; }

    [[nodiscard]] auto release() const noexcept -> T* {
        T* temp = _data;
        _data = nullptr;
        return temp;
    }

    // auto operator<=>(const UniquePtr<T>& other) const noexcept -> bool = default;

    auto swap(UniquePtr<T> other) noexcept { std::swap(other._data, _data); }

    explicit operator bool() const noexcept { return _data != nullptr; }

    template <typename U>
    friend auto operator<<(std::ostream& os, const UniquePtr<U>& ptr) -> std::ostream&;
};

template <typename T>
auto operator<<(std::ostream& os, const UniquePtr<T>& ptr) -> std::ostream& {
    os << ptr.get();
    return os;
}

// don't return rvalue reference since C++ handles moving for us
template <typename T, typename... U>
[[nodiscard]] auto makeUnique(U&&... u) -> UniquePtr<T> {
    return UniquePtr<T>(new T(std::forward<U>(u)...));
}

// TODO: Custom deleters
// TODO Custom allocator
// TODO Weak references
// TODO Move everything into a control block
template <typename T>
class SharedPtr {
    T* _data;
    size_t* _refCount;

public:
    SharedPtr()
        : _data { nullptr }
        , _refCount { nullptr } {}

    explicit SharedPtr(T* data)
        : _data { data } {
        if (_data == nullptr) {
            _refCount = nullptr;
        } else {
            _refCount = new size_t(1);
        }
    }

    SharedPtr(SharedPtr<T>&& other) noexcept
        : _data(other._data)
        , _refCount(other._refCount) {
        other._data = nullptr;
        other._refCount = nullptr;
    }

    SharedPtr(SharedPtr<T>&& other, T* data) noexcept
        : _data { data }
        , _refCount(other._refCount) {
        if (_data != nullptr) {
            ++(*_refCount);
        }
    }

    auto operator=(SharedPtr<T>&& other) noexcept -> SharedPtr<T>& {
        swap(other);
        return *this;
    }

    SharedPtr(const SharedPtr<T>& other) noexcept
        : _data(other._data)
        , _refCount(other._refCount) {
        if (_data != nullptr) {
            ++(*_refCount);
        }
    }

    SharedPtr(const SharedPtr<T>& other, T* data) noexcept
        : _data { data }
        , _refCount { other._refCount } {
        if (_data != nullptr) {
            ++(*_refCount);
        }
    }

    auto operator=(const SharedPtr<T>& other) -> SharedPtr<T>& {
        SharedPtr<T> temp(other);
        swap(temp);
        if (_data != nullptr) {
            ++(*_refCount);
        }
        return *this;
    }

    SharedPtr(UniquePtr<T>& ptr)
        : _data { ptr.get() } {
        if (_data == nullptr) {
            _refCount = nullptr;
        } else {
            _refCount = new size_t(1);
        }
        ptr.release();
    }


    ~SharedPtr() {
        if (_refCount == nullptr) {
            return;
        }
        if (*_refCount <= 1) {
            delete _data;
            delete _refCount;
            _data = nullptr;
            _refCount = nullptr;
        } else if (_refCount != nullptr) {
            --*_refCount;
        }
    }

    [[nodiscard]] auto useCount() const noexcept -> size_t {
        if (_refCount != nullptr) {
            return *_refCount;
        }
        return 0;
    }

    auto reset(T* data) -> void {
        if (_refCount != nullptr && *_refCount == 1) {
            delete _data;
            delete _refCount;
        }
        _data = data;
        if (_data != nullptr) {
            _refCount = new size_t(1);
        } else {
            _refCount = nullptr;
        }
    }

    auto swap(SharedPtr<T>& other) {
        std::swap(other._data, _data);
        std::swap(other._refCount, _refCount);
    }

    [[nodiscard]] auto get() const noexcept -> T* { return _data; }
    [[nodiscard]] auto operator*() const -> typename std::add_lvalue_reference<T>::type { return *_data; }
    [[nodiscard]] auto operator->() const -> typename std::add_lvalue_reference<T>::type { return *_data; }

    [[nodiscard]] auto operator[](size_t i) const -> typename std::add_lvalue_reference<T>::type { return _data[i]; }

    explicit operator bool() const noexcept { return _data != nullptr; }

    template <typename U>
    friend auto operator<<(std::ostream& os, const SharedPtr<U>& ptr) -> std::ostream&;
};

template <typename U>
auto operator<<(std::ostream& os, const SharedPtr<U> ptr) -> std::ostream& {
    os << ptr.get();
    return os;
}

template <typename T, typename... U>
[[nodiscard]] auto makeShared(U&&... u) -> SharedPtr<T> {
    return SharedPtr<T>(new T(std::forward<U>(u)...));
}

void foo(const SharedPtr<int>& ptr) {
    SharedPtr<int> temp = ptr;
    std::cout << "Use count: " << temp.useCount() << '\n';
    *ptr.get() = 10;

    SharedPtr<int> temp2;
    SharedPtr<int> temp3(new int(15));
}

auto main() -> int {
    int* ptr = new int[1] { 1 };
    std::cout << (*ptr);

    UniquePtr<int> myUPtr(ptr);
    std::cout << *myUPtr.get();

    UniquePtr<int> ptr2(UniquePtr<int>(new int[10] { 1, 2, 3, 4 }));
    std::cout << ptr2[1] << std::endl;

    std::cout << ptr2 << std::endl;

    auto ptr3 = makeUnique<int>(10);
    std::cout << *ptr3.get();

    std::shared_ptr<int> sptr(nullptr);
    std::cout << "Use Count: " << sptr.use_count() << '\n';

    SharedPtr<int> mySPtr(new int(5));
    std::cout << "Use Count: " << mySPtr.useCount() << '\n';

    SharedPtr<int> mySPtr2(mySPtr);
    std::cout << "Use Counts: " << mySPtr.useCount() << ", " << mySPtr2.useCount() << '\n';

    std::cout << *mySPtr << ", " << *mySPtr2 << '\n';
    *mySPtr = 3;
    std::cout << *mySPtr << ", " << *mySPtr2 << '\n';

    foo(mySPtr2);
    std::cout << "Use Counts: " << mySPtr.useCount() << ", " << mySPtr2.useCount() << '\n';
    std::cout << *mySPtr << ", " << *mySPtr2 << '\n';

    mySPtr = makeShared<int>(23);
    std::cout << "Use Counts: " << mySPtr.useCount() << ", " << mySPtr2.useCount() << '\n';
    std::cout << *mySPtr << ", " << *mySPtr2 << '\n';
}

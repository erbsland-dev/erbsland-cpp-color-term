// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <coroutine>
#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::text::impl {

/// A custom generator for coroutines in C++20.
/// Can be replaced by `std::generator<T>` in C++23.
template <typename T>
class Generator {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        T current_value;
        std::exception_ptr _exception{};

        auto get_return_object() { return Generator{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        auto initial_suspend() noexcept -> std::suspend_always { return {}; }
        auto final_suspend() noexcept -> std::suspend_always { return {}; }
        template <std::convertible_to<T> Fwd>
        auto yield_value(Fwd &&value) -> std::suspend_always {
            current_value = std::forward<Fwd>(value);
            return {};
        }
        void return_void() {}
        void unhandled_exception() { _exception = std::current_exception(); }
    };

    class iterator {
    public:
        explicit iterator(handle_type handle) : _handle{handle} {}
        iterator() = default;

        auto operator*() const -> T {
            if (_handle && _handle.promise()._exception) {
                std::rethrow_exception(_handle.promise()._exception);
            }
            if (!_handle || _handle.done()) {
                throw std::runtime_error("Dereferencing a completed coroutine");
            }
            return _handle.promise().current_value;
        }

        auto operator++() -> iterator & {
            if (_handle) {
                _handle.resume();
                if (_handle.done() && !_handle.promise()._exception) {
                    _handle = nullptr; // Mark the end of iteration
                }
            }
            return *this;
        }

        auto operator==(const iterator &other) const -> bool { return _handle == other._handle; }

        auto operator!=(const iterator &other) const -> bool { return !(*this == other); }

    private:
        handle_type _handle{};
    };

public:
    Generator() = default;
    explicit Generator(handle_type handle) : _handle(handle) {}
    ~Generator() {
        if (_handle) {
            _handle.destroy();
        }
    }

    Generator(const Generator &) = delete;
    auto operator=(const Generator &) -> Generator & = delete;
    Generator(Generator &&other) noexcept : _handle(std::exchange(other._handle, {})) {}
    auto operator=(Generator &&other) noexcept -> Generator & {
        if (this != &other) {
            if (_handle) {
                _handle.destroy();
            }
            _handle = std::exchange(other._handle, {});
        }
        return *this;
    }

public:
    auto next() -> std::optional<T> {
        if (!prepareNext()) {
            return {};
        }
        return _handle.promise().current_value;
    }

    auto begin() -> iterator {
        if (!prepareNext()) {
            return end();
        }
        return iterator{_handle};
    }

    auto end() -> iterator { return iterator{}; }

private:
    /// Prepare the next element.
    /// @return `true` on success, `false` if there is no next element.
    auto prepareNext() -> bool {
        if (!_handle) {
            return false;
        }
        _handle.resume();
        if (_handle.promise()._exception) {
            std::rethrow_exception(_handle.promise()._exception);
        }
        return !_handle.done();
    }

private:
    handle_type _handle{};
};

}

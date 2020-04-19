#pragma once

namespace std_bp
{

template <typename T>
class Optional
{

public:
    using value_type = T;

    Optional() = default;

    template <typename U = value_type>
    Optional(U &&val) : value_(std::move(val)), hasValue_{true} {}

    Optional(const Optional &) = default;
    Optional &operator=(const Optional &) = default;
    Optional(Optional &&) = default;
    Optional &operator=(Optional &&) = default;

    const T *operator->() const { return &value_; }
    T *operator->() { return &value_; }
    const T &operator*() const { return value_; }
    T &operator*() { return value_; }

    operator bool() const { return hasValue_; }
    bool has_value() const { return hasValue_; }

private:
    value_type value_;
    bool hasValue_ = false;
};

} // namespace std_bp
#pragma once

#include <array>
#include <cstdlib>

class Uuid
{

public:
    typedef uint8_t value_type;
    typedef uint8_t &reference;
    typedef uint8_t const &const_reference;
    typedef uint8_t *iterator;
    typedef uint8_t const *const_iterator;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    Uuid() = default;

    template <typename Iter>
    static Uuid make_uuid(Iter begin, Iter end);
    static Uuid make_uuid(String value);

    constexpr static size_type capacity() { return MAX_SIZE; }
    size_type size() const { return size_; }
    bool isValid() const { return valid_; }

    iterator begin() noexcept { return std::begin(data_); }
    iterator end() noexcept { return std::begin(data_) + size_; }
    const_iterator begin() const noexcept { return std::begin(data_); }
    const_iterator end() const noexcept { return std::begin(data_) + size_; }

private:
    static constexpr size_type MAX_SIZE = 10;

    std::array<value_type, MAX_SIZE> data_{};
    size_type size_;
    bool valid_{false};
};

bool operator==(Uuid const &lhs, Uuid const &rhs) noexcept
{
    return std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs));
}

bool operator!=(Uuid const &lhs, Uuid const &rhs) noexcept
{
    return !(lhs == rhs);
}

bool operator<(Uuid const &lhs, Uuid const &rhs) noexcept
{
    return std::lexicographical_compare(std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs));
}

bool operator>(Uuid const &lhs, Uuid const &rhs) noexcept
{
    return std::lexicographical_compare(std::begin(rhs), std::end(rhs), std::begin(lhs), std::end(lhs));
}

bool operator<=(Uuid const &lhs, Uuid const &rhs) noexcept
{
    return !(lhs > rhs);
}

bool operator>=(Uuid const &lhs, Uuid const &rhs) noexcept
{
    return !(lhs < rhs);
}

template <typename Iter>
Uuid Uuid::make_uuid(Iter begin, Iter end)
{
    Uuid out{};
    size_type size = std::distance(begin, end);

    if (size > capacity())
    {
        return out;
    }

    std::copy(begin, end, std::begin(out));
    out.valid_ = true;
    out.size_ = size;
    return out;
}

// Create a uuid from a formatted hexidecimal string. Eg. 79 81 4E
Uuid Uuid::make_uuid(String value)
{

    // First dump all spaces
    for (size_t i = 0; i < value.length();)
    {
        if (std::isspace(value[i]))
        {
            value.remove(i, 1);
        }
        else
        {
            ++i;
        }
    }

    // Check if we need to add a 0 to the front
    if (value.length() % 2)
    {
        value = '0' + value;
    }

    std::array<Uuid::value_type, Uuid::capacity()> data{};
    for (size_t i = 0; i < value.length(); i += 2)
    {
        char *end;
        data[i / 2] = std::strtol(value.substring(i, i + 2).c_str(), &end, HEX);
    }

    return Uuid::make_uuid(std::begin(data), std::begin(data) + value.length() / 2);
}

String toString(Uuid uuid)
{
    String content;
    for (auto val : uuid)
    {
        content.concat(String(val < 0x10 ? " 0" : " "));
        content.concat(String(val, HEX));
    }
    content.toUpperCase();
    return content;
}

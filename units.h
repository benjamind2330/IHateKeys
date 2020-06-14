#pragma once
#include <ratio>
#include <type_traits>

namespace units
{

    template <typename Rep,
              typename Period = std::ratio<1>>
    class Current;

    template <class ToDuration, class Rep, class Period>
    ToDuration current_cast(const Current<Rep, Period> &d);

    using MicroAmps = units::Current<std::size_t, std::micro>;
    using MilliAmps = units::Current<std::size_t, std::milli>;
    using Amps = units::Current<std::size_t>;
    using KiloAmps = units::Current<std::size_t, std::kilo>;

    template <typename Rep,
              typename Period>
    class Current
    {
    public:
        using rep = Rep;
        using period = Period;

        template <class Rep2>
        explicit Current(const Rep2 &r) : count_{r}
        {
        }

        template <class Rep2, class Period2>
        constexpr Current(const Current<Rep2, Period2> &d) : count_{current_cast<Current>(d).count()}
        {
        }

        Rep count() const
        {
            return count_;
        }

    private:
        Rep count_{};
    };

    template <class ToCurrent, class Rep, class Period>
    ToCurrent current_cast(const Current<Rep, Period> &d)
    {
        using Convert = std::ratio_divide<Period, typename ToCurrent::period>;
        return ToCurrent{static_cast<typename ToCurrent::rep>(d.count() * static_cast<long double>(Convert::num) / Convert::den)};
    }

    inline namespace literals
    {

        MicroAmps operator"" _uA(unsigned long long val)
        {
            return MicroAmps{val};
        }

        MilliAmps operator"" _mA(unsigned long long val)
        {
            return MilliAmps{val};
        }

        Amps operator"" _A(unsigned long long val)
        {
            return Amps{val};
        }
    } // namespace literals

} // namespace units

template <class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator<(const units::Current<Rep1, Period1> &lhs,
                         const units::Current<Rep2, Period2> &rhs)
{
    using CommmonRep = typename std::common_type<Rep1, Rep2>::type;
    using LargerRatio = typename std::conditional<std::ratio_less<Period1, Period2>::value, Period1, Period2>::type;
    using NewCurrent = units::Current<CommmonRep, LargerRatio>;

    return units::current_cast<NewCurrent>(lhs).count() < units::current_cast<NewCurrent>(rhs).count();
}

template <class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator==(const units::Current<Rep1, Period1> &lhs,
                          const units::Current<Rep2, Period2> &rhs)
{
    using CommmonRep = typename std::common_type<Rep1, Rep2>::type;
    using LargerRatio = typename std::conditional<std::ratio_less<Period1, Period2>::value, Period1, Period2>::type;
    using NewCurrent = units::Current<CommmonRep, LargerRatio>;

    return units::current_cast<NewCurrent>(lhs).count() == units::current_cast<NewCurrent>(rhs).count();
}

template <class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator!=(const units::Current<Rep1, Period1> &lhs,
                          const units::Current<Rep2, Period2> &rhs)
{
    return !(lhs == rhs);
}

template <class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator>(const units::Current<Rep1, Period1> &lhs,
                         const units::Current<Rep2, Period2> &rhs)
{
    return !(lhs < rhs) && lhs != rhs;
}

template <class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator<=(const units::Current<Rep1, Period1> &lhs,
                          const units::Current<Rep2, Period2> &rhs)
{
    return !(lhs > rhs);
}

template <class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator>=(const units::Current<Rep1, Period1> &lhs,
                          const units::Current<Rep2, Period2> &rhs)
{
    return !(lhs < rhs);
}

#include "histogram.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::detail {

Histogram::Histogram(int32 bin_size)
    : bin_size{bin_size}
{
    Expects(bin_size >= 1);
}

void Histogram::operator()(int32 value)
{
    Expects(bin_size >= 1);
    Expects(value >= 0);

    const int32 bin = value / bin_size;
    ++bins.try_emplace(bin, 0).first->second;
}

void Histogram::clear() { bins.clear(); }

std::size_t Histogram::size() const { return bins.size(); }

bool Histogram::empty() const { return bins.empty(); }

auto Histogram::begin() const noexcept -> Iterator
{
    return {bins.begin(), bin_size};
}

auto Histogram::end() const noexcept -> Iterator
{
    return {bins.end(), bin_size};
}

Bin Histogram::operator[](int32 index) const
{
    Expects(!bins.empty());

    return *Iterator{bins.find(index), bin_size};
}

Histogram::Iterator::Iterator(
    const Bins::const_iterator& iter,
    int32 bin_size) noexcept
    : iter{iter}
    , bin_size{bin_size}
{
    Expects(bin_size >= 1);
}

Histogram::Iterator::Iterator() noexcept = default;

Bin Histogram::Iterator::operator*() const { return to_bin(*iter); }

auto Histogram::Iterator::operator++() noexcept -> Iterator&
{
    ++iter;
    return *this;
}

auto Histogram::Iterator::operator++(int) noexcept -> Iterator
{
    return {iter++, bin_size};
}

auto Histogram::Iterator::operator--() noexcept -> Iterator&
{
    --iter;
    return *this;
}

auto Histogram::Iterator::operator--(int) noexcept -> Iterator
{
    return {iter--, bin_size};
}

Histogram::Iterator::difference_type operator-(
    const Histogram::Iterator& a,
    const Histogram::Iterator& b) noexcept
{
    return a.iter - b.iter;
}

Histogram::Iterator operator+(
    const Histogram::Iterator& it,
    Histogram::Iterator::difference_type i) noexcept
{
    return {it.iter + i, it.bin_size};
}

Histogram::Iterator operator+(
    Histogram::Iterator::difference_type i,
    const Histogram::Iterator& it) noexcept
{
    return it + i;
}

Histogram::Iterator operator-(
    const Histogram::Iterator& it,
    Histogram::Iterator::difference_type i) noexcept
{
    return {it.iter - i, it.bin_size};
}

Histogram::Iterator operator-(
    Histogram::Iterator::difference_type i,
    const Histogram::Iterator& it) noexcept
{
    return it - i;
}

auto Histogram::Iterator::operator+=(
    Histogram::Iterator::difference_type i) noexcept -> Iterator&
{
    iter += i;
    return *this;
}

auto Histogram::Iterator::operator-=(difference_type i) noexcept
    -> Iterator&
{
    iter -= i;
    return *this;
}

Bin Histogram::Iterator::operator[](difference_type i) const
{
    return to_bin(iter[i]);
}

std::strong_ordering
Histogram::Iterator::operator<=>(const Iterator& other) const noexcept
{
    if (iter == other.iter) return std::strong_ordering::equal;
    if (iter < other.iter) return std::strong_ordering::less;
    return std::strong_ordering::greater;
}

Bin Histogram::Iterator::to_bin(Bins::const_reference v) const
{
    Expects(v.first >= 0);
    Expects(v.second >= 0);
    Expects(bin_size >= 1);

    const auto start = v.first * bin_size;
    return {
        .count{v.second},
        .low{start},
        .middle{start + bin_size / 2},
        .high{start + bin_size}};
}
} // namespace angonoka::detail

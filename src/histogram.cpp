#include "histogram.h"
#include <gsl/gsl-lite.hpp>

namespace angonoka::detail {

Histogram::Histogram(int32 bucket_size)
    : bucket_size{bucket_size}
{
    Expects(bucket_size >= 1);
}

void Histogram::operator()(int32 value)
{
    Expects(bucket_size >= 1);
    Expects(value >= 0);

    const int32 bucket = value / bucket_size;
    ++buckets.try_emplace(bucket, 0).first->second;
}

void Histogram::clear() { buckets.clear(); }

std::size_t Histogram::size() const { return buckets.size(); }

bool Histogram::empty() const { return buckets.empty(); }

auto Histogram::begin() const noexcept -> Iterator
{
    return {buckets.begin(), bucket_size};
}

auto Histogram::end() const noexcept -> Iterator
{
    return {buckets.end(), bucket_size};
}

Bucket Histogram::operator[](int32 index) const
{
    Expects(!buckets.empty());

    return *Iterator{buckets.find(index), bucket_size};
}

Histogram::Iterator::Iterator(
    const Buckets::const_iterator& iter,
    int32 bucket_size) noexcept
    : iter{iter}
    , bucket_size{bucket_size}
{
    Expects(bucket_size >= 1);
}

Histogram::Iterator::Iterator() noexcept = default;

Bucket Histogram::Iterator::operator*() const
{
    return to_bucket(*iter);
}

auto Histogram::Iterator::operator++() noexcept -> Iterator&
{
    ++iter;
    return *this;
}

auto Histogram::Iterator::operator++(int) noexcept -> Iterator
{
    return {iter++, bucket_size};
}

auto Histogram::Iterator::operator--() noexcept -> Iterator&
{
    --iter;
    return *this;
}

auto Histogram::Iterator::operator--(int) noexcept -> Iterator
{
    return {iter--, bucket_size};
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
    return {it.iter + i, it.bucket_size};
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
    return {it.iter - i, it.bucket_size};
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

Bucket Histogram::Iterator::operator[](difference_type i) const
{
    return to_bucket(iter[i]);
}

std::strong_ordering
Histogram::Iterator::operator<=>(const Iterator& other) const noexcept
{
    if (iter == other.iter) return std::strong_ordering::equal;
    if (iter < other.iter) return std::strong_ordering::less;
    return std::strong_ordering::greater;
}

Bucket
Histogram::Iterator::to_bucket(Buckets::const_reference v) const
{
    Expects(v.first >= 0);
    Expects(v.second >= 0);
    Expects(bucket_size >= 1);

    const auto start = v.first * bucket_size;
    return {
        .count{v.second},
        .low{start},
        .middle{start + bucket_size / 2},
        .high{start + bucket_size}};
}
} // namespace angonoka::detail

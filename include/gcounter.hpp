#ifndef GCOUNTER_H
#define GCOUNTER_H

#include <numeric>
#include <optional>
#include <system_error>
#include <utility>

#include <crdt_traits.hpp>
#include <dot.hpp>
#include <version_vector.hpp>

namespace crdt {

template <actor_type A> struct gcounter {
  version_vector<A> inner;

  gcounter() = default;
  gcounter(version_vector<A> &&v) noexcept : inner(std::move(v)) {}
  gcounter(const gcounter &) = default;
  gcounter(gcounter &&) = default;

  gcounter& operator=(gcounter&&) = default;

  auto operator<=>(const gcounter<A> &) const noexcept = default;

  auto validate_op(const dot<A> &Op) noexcept
      -> std::optional<std::error_condition> {
    return std::nullopt;
  }

  void apply(const dot<A> &Op) noexcept { inner.apply(Op); }

  auto validate_merge(const version_vector<A> &) noexcept
      -> std::optional<std::error_condition> {
    return std::nullopt;
  }

  void merge(const gcounter<A> &other) noexcept { inner.merge(other.inner); }

  void reset_remove(const version_vector<A> &v) { inner.reset_remove(v); }

  auto inc(const A &a, std::uint32_t steps = 1) noexcept -> gcounter<A> {
    auto delta = (*this) + std::pair{a, steps};
    apply(delta);

    gcounter<A> res;
    res.apply(delta);

    return res;
  }

  auto operator+(const A &a) const noexcept -> dot<A> {
    return ((*this) + std::pair{a, 1});
  }

  auto operator+(std::pair<A, std::uint32_t> &&a) const noexcept -> dot<A> {
	auto steps = a.second;
    steps += inner.get(a.first);
    return dot(a.first, steps);
  }

  auto read() -> std::uint32_t {
    return std::accumulate(
        inner.dots.begin(), inner.dots.end(), 0,
        [](std::uint32_t sum, const auto &n) { return sum + n.second; });
  }
};

} // namespace crdt.

#endif // GCOUNTER_H

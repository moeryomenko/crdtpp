#ifndef LWWREG_H
#define LWWREG_H

#include <concepts>
#include <optional>

#include <crdt_traits.hpp>

namespace crdt {

template <typename T>
concept marker_type = std::totally_ordered<T>;

template <value_type V, marker_type M> struct lwwreg {
  using Op = lwwreg<V, M>;
  V value;
  M marker;

  lwwreg() = default;
  lwwreg(const V &v, const M &m) : value(v), marker(m) {}
  lwwreg(V &&v, M &&m) : value(std::move(v)), marker(std::move(m)) {}
  lwwreg(const lwwreg<V, M> &) = default;
  lwwreg(lwwreg<V, M> &&) = default;
  auto operator<=>(const lwwreg<V, M> &) const = default;

  auto update(V val, M m) noexcept -> lwwreg<V, M> {
    if (marker < m) {
      value = val, marker = m;
    }
    return lwwreg<V, M>{val, m};
  }

  auto validate_update(V &&val, M &&m) const noexcept
      -> std::optional<std::error_condition> {
    if (marker == m && value != val)
      return std::make_error_condition(std::errc::operation_would_block);
    return std::nullopt;
  }

  auto validate_op(lwwreg<V, M> other) const noexcept
      -> std::optional<std::error_condition> {
    return valida_update(other.value, other.marker);
  }

  void apply(const lwwreg<V, M> &other) noexcept {
    update(other.value, other.marker);
  }

  auto validate_merge(lwwreg<V, M> other) const noexcept
      -> std::optional<std::error_condition> {
    return valida_update(other.value, other.marker);
  }

  void merge(const lwwreg<V, M> &other) noexcept {
    update(other.value, other.marker);
  }
};

} // namespace crdt.

#endif // LWWREG_H

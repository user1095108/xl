#ifndef XL_LISTITERATOR_HPP
# define XL_LISTITERATOR_HPP
# pragma once

#include <iterator>
#include <type_traits>

namespace xl
{

constexpr auto assign(auto& ...a) noexcept
{ // assign idiom
  return [&](auto const ...v) noexcept { ((a = v), ...); };
}

constexpr auto conv(auto const ...n) noexcept
{
  return (std::uintptr_t(n) ^ ...);
}

constexpr auto next(auto i, size_type n) noexcept
{
  for (; n; --n, ++i);
  return i;
}

template <typename T>
class listiterator
{
  using iterator_t = listiterator<std::remove_const_t<T>>;
  friend listiterator<T const>;

  template <typename U> requires(!std::is_reference_v<U>) friend class list;

  using node_t = std::remove_const_t<T>;
  node_t* n_, *p_;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::intmax_t;
  using value_type = std::conditional_t<
      std::is_const_v<T>,
      typename T::value_type const,
      typename T::value_type
    >;

  using pointer = value_type*;
  using reference = value_type&;

public:
  listiterator() = default;

  listiterator(node_t* const n, node_t* const p) noexcept:
    n_(n),
    p_(p)
  {
  }

  listiterator(listiterator const&) = default;
  listiterator(listiterator&&) = default;

  listiterator(iterator_t const& o) noexcept
    requires(std::is_const_v<T>):
    n_(o.n_),
    p_(o.p_)
  {
  }

  // assignment
  listiterator& operator=(listiterator const&) = default;
  listiterator& operator=(listiterator&&) = default;

  listiterator& operator=(iterator_t const& o) noexcept
    requires(std::is_const_v<T>)
  {
    n_ = o.n_; p_ = o.p_; return *this;
  }

  // increment, decrement
  auto& operator++() noexcept
  {
    assign(n_, p_)(n_->link(p_), n_); return *this;
  }

  auto& operator--() noexcept
  {
    assign(n_, p_)(p_, p_->link(n_)); return *this;
  }

  auto operator++(int) noexcept { auto const r(*this); ++*this; return r; }
  auto operator--(int) noexcept { auto const r(*this); --*this; return r; }

  // comparison
  bool operator==(listiterator const& o) const noexcept { return n_ == o.n_; }

  // member access
  auto operator->() const noexcept { return &std::add_pointer_t<T>(n_)->v_; }
  auto& operator*() const noexcept { return std::add_pointer_t<T>(n_)->v_; }

  //
  auto n() const noexcept { return n_; }
  auto p() const noexcept { return p_; }
};

}

#endif // XL_LISTITERATOR_HPP

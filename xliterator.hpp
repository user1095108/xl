#ifndef XL_ITERATOR_HPP
# define XL_ITERATOR_HPP
# pragma once

#include <iterator>

#include <type_traits>

namespace xl
{

template <typename T>
class xliterator
{
  using inverse_const_t = std::conditional_t<
    std::is_const_v<T>,
    xliterator<std::remove_const_t<T>>,
    xliterator<T const>
  >;

  friend inverse_const_t;

  using node_t = std::remove_const_t<T>;
  node_t* n_, *p_;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::conditional_t<
    std::is_const_v<T>,
    typename T::value_type const,
    typename T::value_type
  >;

  using pointer = value_type*;
  using reference = value_type&;

public:
  xliterator() = default;

  xliterator(node_t* const n, node_t* const p) noexcept:
    n_(n),
    p_(p)
  {
  }

  xliterator(xliterator const&) = default;
  xliterator(xliterator&&) = default;

  xliterator(inverse_const_t const& o) noexcept requires(std::is_const_v<T>):
    n_(o.n_),
    p_(o.p_)
  {
  }

  // assignment
  xliterator& operator=(xliterator const&) = default;
  xliterator& operator=(xliterator&&) = default;

  // increment, decrement
  auto& operator++() noexcept
  {
    node_t::assign(n_, p_)(n_->xlink(p_), n_); return *this;
  }

  auto& operator--() noexcept
  {
    node_t::assign(n_, p_)(p_, p_->xlink(n_)); return *this;
  }

  auto operator++(int) noexcept { auto const r(*this); ++*this; return r; }
  auto operator--(int) noexcept { auto const r(*this); --*this; return r; }

  // comparison
  bool operator==(xliterator const& o) const noexcept { return o.n_ == n_; }
  bool operator!=(xliterator const&) const = default;

  // member access
  auto operator->() const noexcept { return &std::add_pointer_t<T>(n_)->v_; }
  auto& operator*() const noexcept { return std::add_pointer_t<T>(n_)->v_; }

  //
  auto n() const noexcept { return n_; }
  auto p() const noexcept { return p_; }
};

}

#endif // XL_ITERATOR_HPP

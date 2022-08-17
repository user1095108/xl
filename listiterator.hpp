#ifndef XL_LISTITERATOR_HPP
# define XL_LISTITERATOR_HPP
# pragma once

#include <iterator>

#include <type_traits>

namespace xl
{

template <typename T>
class listiterator
{
  using inverse_const_t = std::conditional_t<
    std::is_const_v<T>,
    listiterator<std::remove_const_t<T>>,
    listiterator<T const>
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
  listiterator() = default;

  constexpr listiterator(node_t* const n, node_t* const p) noexcept:
    n_(n),
    p_(p)
  {
  }

  listiterator(listiterator const&) = default;
  listiterator(listiterator&&) = default;

  constexpr listiterator(inverse_const_t const& o) noexcept
    requires(std::is_const_v<T>):
    n_(o.n_),
    p_(o.p_)
  {
  }

  // assignment
  listiterator& operator=(listiterator const&) = default;
  listiterator& operator=(listiterator&&) = default;

  // increment, decrement
  constexpr auto& operator++() noexcept
  {
    node_t::assign(n_, p_)(n_->link(p_), n_); return *this;
  }

  constexpr auto& operator--() noexcept
  {
    node_t::assign(n_, p_)(p_, p_->link(n_)); return *this;
  }

  constexpr auto operator++(int) noexcept
  {
    auto const r(*this); ++*this; return r;
  }

  constexpr auto operator--(int) noexcept
  {
    auto const r(*this); --*this; return r;
  }

  // comparison
  constexpr bool operator==(listiterator const& o) const noexcept
  {
    return n_ == o.n_;
  }

  // member access
  constexpr auto operator->() const noexcept
  {
    return &std::add_pointer_t<T>(n_)->v_;
  }

  constexpr auto& operator*() const noexcept
  {
    return std::add_pointer_t<T>(n_)->v_;
  }

  //
  constexpr auto n() const noexcept { return n_; }
  constexpr auto p() const noexcept { return p_; }
};

}

#endif // XL_LISTITERATOR_HPP

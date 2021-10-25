#ifndef XL_ITERATOR_HPP
# define XL_ITERATOR_HPP
# pragma once

#include <iterator>

#include <type_traits>

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

  node_t* n_{};
  node_t* prv_;

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

  xliterator(node_t* const n, node_t* const prv) noexcept:
    n_(n),
    prv_(prv)
  {
  }

  xliterator(xliterator const&) = default;
  xliterator(xliterator&&) = default;

  xliterator(inverse_const_t const& o) noexcept requires(std::is_const_v<T>):
    n_(o.n_),
    prv_(o.prv_)
  {
  }

  //
  xliterator& operator=(xliterator const&) = default;
  xliterator& operator=(xliterator&&) = default;

  bool operator==(xliterator const& o) const noexcept { return n_ == o.n_; }
  bool operator!=(xliterator const&) = default;

  // increment, decrement
  auto& operator++() noexcept
  {
    auto const n(n_); n_ = n->next(prv_); prv_ = n; return *this;
  }

  auto& operator--() noexcept
  {
    auto const prv(prv_); prv_ = prv->prev(n_); n_ = prv; return *this;
  }

  auto operator++(int) noexcept { auto const r(*this); ++*this; return r; }
  auto operator--(int) noexcept { auto const r(*this); --*this; return r; }

  // member access
  auto operator->() const noexcept { return &std::add_pointer_t<T>(n_)->v_; }
  auto& operator*() const noexcept { return std::add_pointer_t<T>(n_)->v_; }

  //
  auto node() const noexcept { return n_; }
  auto prev() const noexcept { return prv_; }
};

#endif // XL_ITERATOR_HPP

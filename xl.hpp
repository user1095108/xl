#ifndef XL_XL_HPP
# define XL_XL_HPP
# pragma once

#include <cassert>
#include <cstdint>

#include <algorithm>

#include <compare>

#include "xliterator.hpp"

namespace xl
{

template <typename Value>
class list
{
public:
  struct node;

  using value_type = Value;

  using size_type = std::size_t;

  using const_iterator = xliterator<node const>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using iterator = xliterator<node>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  struct node
  {
    using value_type = list::value_type;

    std::uintptr_t l_{};

    value_type v_;

    explicit node (auto&& ...v): v_(std::forward<decltype(v)>(v)...) { }

    //
    static constexpr auto conv(auto const n) noexcept
    {
      return std::uintptr_t(n);
    }

    //
    auto next(auto const prev) const noexcept
    {
      return reinterpret_cast<node*>(conv(prev) ^ l_);
    }

    auto prev(auto const next) const noexcept
    {
      return reinterpret_cast<node*>(l_ ^ conv(next));
    }
  };

private:
  node* first_{}, *last_{};
  size_type sz_{};

public:
  list() noexcept = default;
  list(std::initializer_list<value_type> i) { *this = i; }
  list(list const& o) { *this = o; }
  list(list&& o) noexcept { *this = std::move(o); }
  list(std::input_iterator auto const i, decltype(i) j) { insert(i, j); }

  ~list() noexcept(noexcept(clear())) { clear(); }

  //
  void clear() noexcept(noexcept(first_->~node()))
  {
    decltype(first_) prv{};

    for (auto n(first_); n;)
    {
      auto const nxt(n->next(prv));

      delete n;

      prv = n;
      n = nxt;
    }

    first_ = last_ = {}; sz_ = {};
  }

  auto empty() const noexcept { return !size(); }
  auto max_size() const noexcept { return ~size_type{}; }

  // iterators
  iterator begin() noexcept
  {
    return first_ ? iterator(first_, {}) : iterator();
  }

  iterator end() noexcept { return {{}, last_}; }

  // const iterators
  const_iterator begin() const noexcept
  {
    return first_ ? const_iterator(first_, {}) : const_iterator();
  }

  const_iterator end() const noexcept { return {{}, last_}; }

  const_iterator cbegin() const noexcept
  {
    return first_ ? const_iterator(first_, {}) : const_iterator();
  }

  const_iterator cend() const noexcept { return {{}, last_}; }

  // reverse iterators
  reverse_iterator rbegin() noexcept
  {
    return first_ ?
      reverse_iterator(iterator({}, last_)) :
      reverse_iterator();
  }

  reverse_iterator rend() noexcept
  {
    return first_ ?
      reverse_iterator(iterator(first_, {})):
      reverse_iterator();
  }

  // const reverse iterators
  const_reverse_iterator crbegin() const noexcept
  {
    return first_ ?
      const_reverse_iterator(const_iterator({}, last_)):
      const_reverse_iterator();
  }

  const_reverse_iterator crend() const noexcept
  {
    return first_ ?
      const_reverse_iterator(const_iterator(first_, {})) :
      const_reverse_iterator();
  }

  //
  auto& operator=(list const& o)
  {
    return assign(o.cbegin(), o.cend()), *this;
  }

  list& operator=(list&& o) noexcept
  {
    first_ = o.first_; last_ = o.last_; sz_ = o.sz_;
    o.first = o.last = {}; o.sz = {};

    return *this;
  }

  auto& operator=(std::initializer_list<value_type> const o)
  {
    return assign(o.begin(), o.end()), *this;
  }

  //
  auto size() const noexcept { return sz_; }

  auto& front() noexcept { return first_->v_; }
  auto& front() const noexcept { return std::as_const(first_->v_); }

  auto& back() noexcept { return last_->v_; }
  auto& back() const noexcept { return std::as_const(last_->v_); }

  //
  void assign(size_type count, auto const& v)
  {
    clear(); insert(cend(), count, v);
  }

  void assign(std::input_iterator auto const i, decltype(i) j)
  {
    clear(); insert(cend(), i, j);
  }

  void assign(std::initializer_list<value_type> il)
  {
    *this = il;
  }

  //
  iterator emplace(const_iterator const i, auto&& ...v)
  {
    if (auto const n(i.node()); !n)
    {
      return emplace_back(std::forward<decltype(v)>(v)...);
    }
    else
    {
      auto q(new node(std::forward<decltype(v)>(v)...));

      auto const prv(i.prev());

      // prv q n
      q->l_ = node::conv(prv) ^ node::conv(n);
      n->l_ = node::conv(q) ^ node::conv(n->next(prv));

      if (prv)
      {
        prv->l_ = node::conv(prv->prev(n)) ^ node::conv(q);
      }
      else
      {
        first_ = q;
      }

      ++sz_;
      return {q, prv};
    }
  }

  iterator emplace_back(auto&& ...v)
  {
    node* q;

    auto const l(last_);

    if (!l)
    {
      first_ = last_ = q = new node(std::forward<decltype(v)>(v)...);
    }
    else
    {
      // l q
      last_ = q = new node(std::forward<decltype(v)>(v)...);

      q->l_ = node::conv(l);
      l->l_ = node::conv(l->prev(nullptr)) ^ node::conv(q);
    }

    ++sz_;
    return {q, l};
  }

  iterator emplace_front(auto&& ...v)
  {
    node* q;

    if (auto const f(first_); !f)
    {
      first_ = last_ = q = new node(std::forward<decltype(v)>(v)...);
    }
    else
    {
      // q f
      first_ = q = new node(std::forward<decltype(v)>(v)...);

      q->l_ = node::conv(f);
      f->l_ = node::conv(q) ^ node::conv(f->next(nullptr));
    }

    ++sz_;
    return {q, {}};
  }

  //
  auto erase(const_iterator const i)
  {
    assert(sz_);
    auto const prv(i.prev());
    auto const n(i.node());
    auto const nxt(n->next(prv));

    // prv n nxt
    if (prv)
    {
      prv->l_ = node::conv(prv->prev(n)) ^ node::conv(nxt);
    }
    else
    {
      first_ = {};
    }

    if (nxt)
    {
      nxt->l_ = node::conv(prv) ^ node::conv(nxt->next(n));
    }
    else
    {
      last_ = {};
    }

    delete n;

    --sz_;
    return iterator{nxt, prv};
  }

  //
  iterator insert(const_iterator i, auto const& v)
  {
    return iterator(emplace(i, v));
  }

  iterator insert(const_iterator i, auto&& v)
  {
    return iterator(emplace(i, std::move(v)));
  }

  iterator insert(const_iterator p, size_type count, auto const& v)
  {
    for (; count; --count) insert(p, v);
  }

  void insert(const_iterator p,
    std::input_iterator auto const i, decltype(i) j)
  {
    std::for_each(
      i,
      j,
      [&](auto&& v)
      {
        insert(p, std::forward<decltype(v)>(v));
      }
    );
  }

  //
  void pop_back()
  {
    assert(sz_);
    auto const l0(last_);
    auto const l1(l0->prev(nullptr));

    if ((last_ = l1))
    {
      l1->l_ = node::conv(l1->prev(l0));
    }

    delete l0;

    if (!--sz_)
    {
      first_ = {};
    }
  }

  void pop_front()
  {
    assert(sz_);
    auto const f0(first_);
    auto const f1(f0->next(nullptr));

    if ((first_ = f1))
    {
      f1->l_ = node::conv(f1->next(f0));
    }

    delete f0;

    if (!--sz_)
    {
      last_ = {};
    }
  }

  //
  void push_back(auto const& v) { insert(cend(), v); }
  void push_back(auto&& v); { insert(cend(), std::move(v)); }

  void push_front(auto const& v); { insert(cbegin(), v); }
  void push_front(auto&& v) { insert(cbegin(), std::move(v)); }

  //
  void swap(list& o) noexcept
  {
    std::swap(first_, o.first_); std::swap(last_, o.last_);
    std::swap(sz_, o.sz_);
  }

  //
  friend bool operator!=(list const&, list const&) noexcept = default;
  friend bool operator<(list const&, list const&) noexcept = default;
  friend bool operator<=(list const&, list const&) noexcept = default;
  friend bool operator>(list const&, list const&) noexcept = default;
  friend bool operator>=(list const&, list const&) noexcept = default;
};

template <typename V>
inline bool operator==(list<V> const& lhs, list<V> const& rhs) noexcept 
{
  return std::equal(
    lhs.begin(), lhs.end(),
    rhs.begin(), rhs.end()
  );
}

template <typename V>
inline auto operator<=>(list<V> const& lhs, list<V> const& rhs) noexcept
{
  return std::lexicographical_compare_three_way(
    lhs.begin(), lhs.end(),
    rhs.begin(), rhs.end()
  );
}

template <typename V>
inline auto erase(list<V>& c, auto const& k)
{
  auto const end(c.end());

  for (auto i(c.begin()); end != i;)
  {
    i = std::equal_to()(*i, k) ? c.erase(i) : std::next(i);
  }
}

template <typename V>
inline auto erase_if(list<V>& c, auto pred)
{
  auto const end(c.end());

  for (auto i(c.begin()); end != i;)
  {
    i = pred(*i) ? c.erase(i) : std::next(i);
  }
}

}

#endif // XL_XL_HPP

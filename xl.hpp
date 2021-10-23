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

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = value_type const&;

  using const_iterator = xliterator<node const>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using iterator = xliterator<node>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  struct node
  {
    using value_type = list::value_type;

    std::uintptr_t l_;

    value_type v_;

    explicit node(auto&& ...v) noexcept(noexcept(
      Value(std::forward<decltype(v)>(v)...))):
      v_(std::forward<decltype(v)>(v)...)
    {
    }

    //
    static constexpr auto conv(auto const n) noexcept
    {
      return decltype(l_)(n);
    }

    //
    auto next(auto const p) const noexcept
    {
      return reinterpret_cast<node*>(conv(p) ^ l_);
    }

    auto prev(auto const p) const noexcept
    {
      return reinterpret_cast<node*>(conv(p) ^ l_);
    }
  };

private:
  node* first_{}, *last_{};
  size_type sz_{};

public:
  list() = default;
  list(std::initializer_list<value_type> i) { *this = i; }
  list(list const& o) { *this = o; }
  list(list&& o) noexcept { *this = std::move(o); }

  list(std::input_iterator auto const i, decltype(i) j)
  {
    insert(cend(), i, j);
  }

  ~list() noexcept(noexcept(clear())) { clear(); }

  //
  list& operator=(list const& o) { return assign(o.begin(), o.end()), *this; }

  list& operator=(list&& o) noexcept
  {
    first_ = o.first_; last_ = o.last_; sz_ = o.sz_;
    o.first_ = o.last_ = {}; o.sz_ = {};

    return *this;
  }

  list& operator=(std::initializer_list<value_type> const o)
  {
    return assign(o.begin(), o.end()), *this;
  }

  //
  void clear() noexcept(noexcept(first_->~node()))
  {
    decltype(first_) prv{};

    for (auto n(first_); n;)
    {
      auto const nn(n);
      n = n->next(prv);

      delete (prv = nn);
    }

    //
    first_ = last_ = {}; sz_ = {};
  }

  bool empty() const noexcept { return !size(); }
  size_type max_size() const noexcept { return ~size_type{}; }

  // iterators
  iterator begin() noexcept { return {first_, {}}; }
  iterator end() noexcept { return {{}, last_}; }

  // const iterators
  const_iterator begin() const noexcept { return {first_, {}}; }
  const_iterator end() const noexcept { return {{}, last_}; }

  const_iterator cbegin() const noexcept { return {first_, {}}; }
  const_iterator cend() const noexcept { return {{}, last_}; }

  // reverse iterators
  reverse_iterator rbegin() noexcept
  {
    return reverse_iterator(iterator({}, last_));
  }

  reverse_iterator rend() noexcept
  {
    return reverse_iterator(iterator(first_, {}));
  }

  // const reverse iterators
  const_reverse_iterator crbegin() const noexcept
  {
    return const_reverse_iterator(const_iterator({}, last_));
  }

  const_reverse_iterator crend() const noexcept
  {
    return const_reverse_iterator(const_iterator(first_, {}));
  }

  //
  auto& operator[](size_type const i) noexcept
  {
    return *std::next(begin(), i);
  }

  auto& operator[](size_type const i) const noexcept
  {
    return *std::next(begin(), i);
  }

  //
  auto& back() noexcept { return last_->v_; }
  auto& back() const noexcept { return std::as_const(last_->v_); }

  auto& front() noexcept { return first_->v_; }
  auto& front() const noexcept { return std::as_const(first_->v_); }

  size_type size() const noexcept { return sz_; }

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
    assign(il.begin(), il.end());
  }

  //
  iterator emplace(const_iterator const i, auto&& ...a)
  {
    if (auto const n(i.node()); !n)
    {
      return emplace_back(std::forward<decltype(a)>(a)...);
    }
    else
    {
      auto const q(new node(std::forward<decltype(a)>(a)...));

      auto const prv(i.prev());

      // prv q n
      q->l_ = node::conv(prv) ^ node::conv(n);
      n->l_ = node::conv(q) ^ node::conv(n->next(prv));

      if (prv)
      {
        prv->l_ = node::conv(q) ^ node::conv(prv->prev(n));
      }
      else
      {
        first_ = q;
      }

      ++sz_;
      return {q, prv};
    }
  }

  iterator emplace_back(auto&& ...a)
  {
    node* q;
    auto const l(last_);

    if (!l)
    {
      first_ = last_ = q = new node(std::forward<decltype(a)>(a)...);
      q->l_ = {};
    }
    else
    {
      // l q
      last_ = q = new node(std::forward<decltype(a)>(a)...);

      q->l_ = node::conv(l);
      l->l_ = node::conv(q) ^ node::conv(l->prev(nullptr));
    }

    ++sz_;
    return {q, l};
  }

  iterator emplace_front(auto&& ...a)
  {
    node* q;

    if (auto const f(first_); !f)
    {
      first_ = last_ = q = new node(std::forward<decltype(a)>(a)...);
      q->l_ = {};
    }
    else
    {
      // q f
      first_ = q = new node(std::forward<decltype(a)>(a)...);

      q->l_ = node::conv(f);
      f->l_ = node::conv(q) ^ node::conv(f->next(nullptr));
    }

    ++sz_;
    return {q, {}};
  }

  //
  iterator erase(const_iterator const i)
  {
    assert(sz_);
    auto const prv(i.prev());
    auto const n(i.node());
    auto const nxt(n->next(prv));

    // prv n nxt
    if (prv)
    {
      prv->l_ = node::conv(nxt) ^ node::conv(prv->prev(n));
    }
    else
    {
      first_ = nxt;
    }

    if (nxt)
    {
      nxt->l_ = node::conv(prv) ^ node::conv(nxt->next(n));
    }
    else
    {
      last_ = prv;
    }

    --sz_;
    delete n;

    return iterator{nxt, prv};
  }

  iterator erase(const_iterator a, const_iterator const b)
  {
    iterator i(b);

    for (; a != b; i = erase(a), a = i);

    return i;
  }

  //
  iterator insert(const_iterator i, auto const& v) { return emplace(i, v); }

  iterator insert(const_iterator i, auto&& v)
  {
    return emplace(i, std::move(v));
  }

  iterator insert(const_iterator p, size_type count, auto const& v)
  {
    if (count)
    {
      auto const r(insert(p, v));

      for (--count; count; --count) insert(p, v);

      return r;
    }
    else
    {
      return {p.node(), p.prev()};
    }
  }

  iterator insert(const_iterator const p,
    std::input_iterator auto const i, decltype(i) j)
  {
    if (i == j)
    {
      return {p.node(), p.prev()};
    }
    else
    {
      auto const r(insert(p, *i));

      std::for_each(
        std::next(i),
        j,
        [&](auto&& v)
        {
          insert(p, std::forward<decltype(v)>(v));
        }
      );

      return r;
    }
  }

  iterator insert(const_iterator const p,
    std::initializer_list<value_type> il)
  {
    return insert(p, il.begin(), il.end());
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
    else
    {
      first_ = {};
    }

    --sz_;
    delete l0;
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
    else
    {
      last_ = {};
    }

    --sz_;
    delete f0;
  }

  //
  void push_back(value_type const& v) { insert(cend(), v); }
  void push_back(value_type&& v) { insert(cend(), std::move(v)); }

  void push_front(value_type const& v) { insert(cbegin(), v); }
  void push_front(value_type&& v) { insert(cbegin(), std::move(v)); }

  //
  void swap(list& o) noexcept
  {
    std::swap(first_, o.first_); std::swap(last_, o.last_);
    std::swap(sz_, o.sz_);
  }

  //
  void reverse() noexcept { std::swap(first_, last_); }

  void sort() { sort(std::less<value_type>()); }

  void sort(auto cmp)
  {
    auto const s([&](auto&& s,
      auto const begin, auto const end, auto const sz) -> void
      {
        if (sz > 1)
        {
          auto const hsz(sz / 2);
          auto const m(std::next(begin, hsz));

          s(s, begin, m, hsz);
          s(s, m, end, sz - hsz);

          std::inplace_merge(begin, m, end, cmp);
        }
      }
    );

    s(s, begin(), end(), size());
  }

  //
  friend bool operator!=(list const&, list const&) = default;
  friend bool operator<(list const&, list const&) = default;
  friend bool operator<=(list const&, list const&) = default;
  friend bool operator>(list const&, list const&) = default;
  friend bool operator>=(list const&, list const&) = default;
};

template <typename V>
inline bool operator==(list<V> const& lhs, list<V> const& rhs) noexcept 
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename V>
inline auto operator<=>(list<V> const& lhs, list<V> const& rhs) noexcept
{
  return std::lexicographical_compare_three_way(
    lhs.begin(), lhs.end(), rhs.begin(), rhs.end()
  );
}

template <typename V>
constexpr auto erase(list<V>& c, auto const& k)
{
  return erase_if(c, [&](auto&& v) noexcept {return std::equal_to()(v, k);});
}

template <typename V>
constexpr auto erase_if(list<V>& c, auto pred)
{
  typename list<V>::size_type r{};

  auto const end(c.end());

  for (auto i(c.begin()); end != i;)
  {
    i = pred(*i) ? (++r, c.erase(i)) : std::next(i);
  }

  return r;
}

}

#endif // XL_XL_HPP

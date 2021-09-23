#ifndef XL_XL_HPP
# define XL_XL_HPP
# pragma once

#include <cassert>
#include <cstdint>

#include <algorithm>

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

    explicit node (auto&& ...v):
      v_(std::forward<decltype(v)>(v)...)
    {
    }

    //
    static constexpr auto conv(auto const n) noexcept
    {
      return std::uintptr_t(n);
    }

    static iterator emplace_front(auto& li, auto&& ...v)
    {
      node* q;

      if (auto const f(li.first_); !f)
      {
        li.first_ = li.last_ = q = new node(std::forward<decltype(v)>(v)...);
      }
      else
      {
        // q f
        li.first_ = q = new node(std::forward<decltype(v)>(v)...);

        q->l_ = conv(f);
        f->l_ = conv(q) ^ conv(f->next(nullptr));
      }

      ++li.sz_;
      return {q, {}};
    }

    static iterator emplace_back(auto& li, auto&& ...v)
    {
      node* q;

      auto const l(li.last_);

      if (!l)
      {
        li.first_ = li.last_ = q = new node(std::forward<decltype(v)>(v)...);
      }
      else
      {
        // l q
        li.last_ = q = new node(std::forward<decltype(v)>(v)...);

        q->l_ = conv(l);
        l->l_ = conv(l->prev(nullptr)) ^ conv(q);
      }

      ++li.sz_;
      return {q, l};
    }

    static iterator emplace(auto& li, const_iterator const i, auto&& ...v)
    {
      node* q;

      auto const prv(i.prev());

      if (auto const n(i.node()); !n)
      {
        return emplace_back(li, std::forward<decltype(v)>(v)...);
      }
      else
      {
        // prv q n
        q = new node(std::forward<decltype(v)>(v)...);

        // setup links
        q->l_ = conv(prv) ^ conv(n);
        n->l_ = conv(q) ^ conv(n->next(prv));

        if (prv)
        {
          prv->l_ = conv(prv->prev(n)) ^ conv(q);
        }
      }

      ++li.sz_;
      return {q, prv};
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

  ~list() noexcept(noexcept(first_->~node()))
  {
    clear();
  }

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
    clear();
    insert(cend(), o.begin(), o.end());

    return *this;
  }

  list& operator=(list&& o) noexcept
  {
    first_ = o.first_; last_ = o.last_; sz_ = o.sz_;
    o.first = o.last = {}; o.sz = {};

    return *this;
  }

  auto& operator=(std::initializer_list<value_type> const o)
  {
    clear();
    insert(cend(), o.begin(), o.end());

    return *this;
  }

  //
  auto size() const noexcept { return sz_; }

  auto& front() noexcept { return first_->v_; }
  auto& front() const noexcept { return std::as_const(first_->v_); }

  auto& back() noexcept { return last_->v_; }
  auto& back() const noexcept { return std::as_const(last_->v_); }

  //
  auto emplace_back(auto&& ...v)
  {
    return iterator(
      node::emplace_back(
        *this,
        std::forward<decltype(v)>(v)...
      )
    );
  }

  auto emplace_front(auto&& ...v)
  {
    return iterator(
      node::emplace_back(
        *this,
        std::forward<decltype(v)>(v)...
      )
    );
  }

  //
  auto erase(const_iterator const i)
  {
    auto const prv(i.prev());
    auto const n(i.node());
    auto const nxt(n->next(prv));

    // prv n nxt
    if (prv)
    {
      prv->l_ = node::conv(prv->prev(n)) ^ node::conv(nxt);
    }

    if (nxt)
    {
      nxt->l_ = node::conv(prv) ^ node::conv(nxt->next(n));
    }

    delete n;

    --sz_;
    return iterator{nxt, prv};
  }

  //
  iterator insert(const_iterator i, auto const& v)
  {
    return iterator(node::emplace(*this, i, v));
  }

  iterator insert(const_iterator i, auto&& v)
  {
    return iterator(node::emplace(*this, i, std::move(v)));
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
    auto const b(last_);

    auto const last(b->prev(nullptr));
    last->l_ = node::conv(last->prev(b));

    delete b;

    --sz_;
    last_ = last;
  }

  void pop_front()
  {
    auto const f(first_);

    auto const first(f->next(nullptr));
    first->l_ = node::conv(first->next(f));

    delete f;

    --sz_;
    first_ = first;
  }
};

}

#endif // XL_XL_HPP

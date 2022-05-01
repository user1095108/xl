#ifndef XL_LIST_HPP
# define XL_LIST_HPP
# pragma once

#include <cstdint>

#include <algorithm>
#include <compare>

#include "listiterator.hpp"

namespace xl
{

template <typename T>
class list
{
  struct node;

  friend class listiterator<node>;
  friend class listiterator<node const>;

public:
  using value_type = T;

  using difference_type = std::ptrdiff_t;
  using size_type = std::uintmax_t;
  using reference = value_type&;
  using const_reference = value_type const&;

  using const_iterator = listiterator<node const>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using iterator = listiterator<node>;
  using reverse_iterator = std::reverse_iterator<iterator>;

private:
  struct node
  {
    using value_type = list::value_type;

    std::uintptr_t l_;
    value_type v_;

    explicit node(auto&& ...a)
      noexcept(noexcept(T(std::forward<decltype(a)>(a)...))):
      v_(std::forward<decltype(a)>(a)...)
    {
    }

    //
    static constexpr auto assign(auto& ...a) noexcept
    { // assign idiom
      return [&](auto const ...v) noexcept { ((a = v), ...); };
    }

    static constexpr auto conv(auto const ...n) noexcept
    {
      return (std::uintptr_t(n) ^ ...);
    }

    static void destroy(auto n) noexcept(noexcept(delete n))
    {
      for (decltype(n) p{}; n;)
      {
        assign(n, p)(n->link(p), n);

        delete p;
      }
    }

    //
    auto link() const noexcept { return reinterpret_cast<node*>(l_); }

    auto link(auto const n) const noexcept
    {
      return reinterpret_cast<node*>(std::uintptr_t(n) ^ l_);
    }

    //
    static void sort(auto const b, decltype(b) e, size_type const sz,
      auto&& cmp)
    {
      if (sz > 1)
      {
        auto const hsz(sz / 2);
        auto const m(std::next(b, hsz));

        sort(b, m, hsz, std::forward<decltype(cmp)>(cmp));
        sort(m, e, sz - hsz, std::forward<decltype(cmp)>(cmp));

        std::inplace_merge(b, m, e, cmp);
      }
    }
  };

private:
  node* first_{}, *last_{};

public:
  list() = default;

  list(std::initializer_list<value_type> const l)
    requires(std::is_copy_constructible_v<value_type>)
  {
    assign(l.begin(), l.end());
  }

  list(list const& o)
    requires(std::is_copy_constructible_v<value_type>)
  {
    assign(o.begin(), o.end());
  }

  list(list&& o)
  {
    *this = std::move(o);
  }

  list(std::input_iterator auto const i, decltype(i) j)
    requires(std::is_constructible_v<value_type, decltype(*i)>)
  {
    std::for_each(
      i,
      j,
      [&](auto&& v)
      {
        emplace_back(std::forward<decltype(v)>(v));
      }
    );
  }

  ~list() noexcept(noexcept(node::destroy(first_))) { node::destroy(first_); }

  // self-assign neglected
  auto& operator=(list const& o)
    requires(std::is_copy_constructible_v<value_type>)
  {
    assign(o.begin(), o.end());

    return *this;
  }

  auto& operator=(list&& o)
    noexcept(noexcept(delete first_))
  {
    clear();

    first_ = o.first_; last_ = o.last_;
    o.first_ = o.last_ = {};

    return *this;
  }

  auto& operator=(std::initializer_list<value_type> const l)
    requires(std::is_copy_constructible_v<value_type>)
  {
    assign(l.begin(), l.end());

    return *this;
  }

  //
  friend bool operator==(list const& lhs, list const& rhs) noexcept
  {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  friend auto operator<=>(list const& lhs, list const& rhs) noexcept
  {
    return std::lexicographical_compare_three_way(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end()
    );
  }

  //
  bool empty() const noexcept { return !first_; }
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
  constexpr auto& operator[](size_type const i) noexcept
  {
    return *std::next(begin(), i);
  }

  constexpr auto& operator[](size_type const i) const noexcept
  {
    return *std::next(begin(), i);
  }

  //
  constexpr const_reference at(size_type const i) noexcept
  {
    return *std::next(cbegin(), i);
  }

  constexpr const_reference at(size_type const i) const noexcept
  {
    return *std::next(cbegin(), i);
  }

  constexpr auto& back() noexcept { return last_->v_; }
  constexpr auto const& back() const noexcept { return last_->v_; }

  constexpr auto& front() noexcept { return first_->v_; }
  constexpr auto const& front() const noexcept { return first_->v_; }

  constexpr size_type size() const noexcept
  {
    size_type sz{};

    for (decltype(first_) n(first_), p{}; n; ++sz)
    {
      node::assign(n, p)(n->link(p), n);
    }

    return sz;
  }

  //
  void assign(size_type count, value_type const& v)
    requires(std::is_copy_constructible_v<value_type>)
  {
    clear();

    for (; count; --count) emplace_back(v);
  }

  void assign(std::input_iterator auto const i, decltype(i) j)
    requires(std::is_constructible_v<value_type, decltype(*i)>)
  {
    clear();

    std::for_each(
      i,
      j,
      [&](auto&& v)
      {
        emplace_back(std::forward<decltype(v)>(v));
      }
    );
  }

  void assign(std::initializer_list<value_type> const l)
    requires(std::is_copy_constructible_v<value_type>)
  {
    assign(l.begin(), l.end());
  }

  //
  void clear() noexcept(noexcept(delete first_))
  {
    node::destroy(first_);

    //
    first_ = last_ = {};
  }

  //
  iterator emplace(const_iterator const i, auto&& ...a)
    requires(std::is_constructible_v<value_type, decltype(a)&&...>)
  {
    auto const n(i.n());
    auto const p(n ? i.p() : last_);

    // p q n
    auto const q(new node(std::forward<decltype(a)>(a)...));
    q->l_ = node::conv(n, p);

    if (n)
    {
      n->l_ = node::conv(q, n->link(p));
    }
    else
    {
      last_ = q;
    }

    if (p)
    {
      p->l_ = node::conv(q, p->link(n));
    }
    else
    {
      first_ = q;
    }

    return {q, p};
  }

  iterator emplace_back(auto&& ...a)
    requires(std::is_constructible_v<value_type, decltype(a)&&...>)
  {
    auto const l(last_);

    auto const q(last_ = new node(std::forward<decltype(a)>(a)...));
    q->l_ = node::conv(l);

    if (l)
    {
      // l q
      l->l_ ^= node::conv(q);
    }
    else
    {
      first_ = q;
    }

    return {q, l};
  }

  iterator emplace_front(auto&& ...a)
    requires(std::is_constructible_v<value_type, decltype(a)&&...>)
  {
    auto const f(first_);

    auto const q(first_ = new node(std::forward<decltype(a)>(a)...));
    q->l_ = node::conv(f);

    if (f)
    {
      // q f
      f->l_ ^= node::conv(q);
    }
    else
    {
      last_ = q;
    }

    return {q, {}};
  }

  //
  iterator erase(const_iterator const i)
    noexcept(noexcept(delete first_))
  {
    auto const n(i.n());
    auto const p(i.p());
    auto const nxt(n->link(p));

    // p n nxt
    if (p)
    {
      p->l_ = node::conv(nxt, p->link(n));
    }
    else
    {
      first_ = nxt;
    }

    if (nxt)
    {
      nxt->l_ = node::conv(p, nxt->link(n));
    }
    else
    {
      last_ = p;
    }

    delete n;

    return iterator{nxt, p};
  }

  iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    iterator i(b);

    for (; a != b; i = erase(a), a = i);

    return i;
  }

  //
  iterator insert(const_iterator const i, value_type const& v)
    requires(std::is_copy_constructible_v<value_type>)
  {
    return emplace(i, v);
  }

  iterator insert(const_iterator const i, value_type&& v)
    requires(std::is_move_constructible_v<value_type>)
  {
    return emplace(i, std::move(v));
  }

  iterator insert(const_iterator const i, size_type count,
    value_type const& v)
    requires(std::is_copy_constructible_v<value_type>)
  {
    if (count)
    {
      auto const r(insert(i, v));

      for (--count; count; --count) insert(i, v);

      return r;
    }
    else
    {
      return {i.n(), i.p()};
    }
  }

  iterator insert(const_iterator const i,
    std::input_iterator auto const j, decltype(j) k)
    requires(std::is_constructible_v<value_type, decltype(*j)>)
  {
    if (j == k)
    {
      return {i.n(), i.p()};
    }
    else
    {
      auto const r(emplace(i, *j));

      std::for_each(
        std::next(j),
        k,
        [&](auto&& v)
        {
          emplace(i, std::forward<decltype(v)>(v));
        }
      );

      return r;
    }
  }

  iterator insert(const_iterator const i,
    std::initializer_list<value_type> const l)
    requires(std::is_copy_constructible_v<value_type>)
  {
    return insert(i, l.begin(), l.end());
  }

  //
  void pop_back()
    noexcept(noexcept(delete first_))
  {
    auto const l0(last_);

    if (auto const l1(last_ = l0->link()); l1)
    {
      l1->l_ = node::conv(l1->link(l0));
    }
    else
    {
      first_ = {};
    }

    delete l0;
  }

  void pop_front()
    noexcept(noexcept(delete first_))
  {
    auto const f0(first_);

    if (auto const f1(first_ = f0->link()); f1)
    {
      f1->l_ = node::conv(f1->link(f0));
    }
    else
    {
      last_ = {};
    }

    delete f0;
  }

  //
  void push_back(value_type const& v)
    requires(std::is_copy_constructible_v<value_type>)
  {
    emplace_back(v);
  }

  void push_back(value_type&& v)
    requires(std::is_move_constructible_v<value_type>)
  {
    emplace_back(std::move(v));
  }

  void push_front(value_type const& v)
    requires(std::is_copy_constructible_v<value_type>)
  {
    emplace_front(v);
  }

  void push_front(value_type&& v)
    requires(std::is_move_constructible_v<value_type>)
  {
    emplace_front(std::move(v));
  }

  //
  void swap(list& o) noexcept
  {
    std::swap(first_, o.first_);
    std::swap(last_, o.last_);
  }

  //
  void reverse() noexcept { std::swap(first_, last_); }
  void sort(auto cmp) { node::sort(begin(), end(), size(), cmp); }
  void sort() { sort(std::less<value_type>()); }

  //
  friend auto erase(list& c, value_type const& k)
    noexcept(noexcept(delete first_))
  {
    return erase_if(c, [&](auto&& v) noexcept{return std::equal_to()(v, k);});
  }

  friend auto erase(list& c, auto const& k)
    noexcept(noexcept(delete first_))
  {
    return erase_if(c, [&](auto&& v) noexcept{return std::equal_to()(v, k);});
  }

  friend auto erase_if(list& c, auto pred)
    noexcept(noexcept(delete first_))
  {
    size_type r{};

    for (auto i(c.begin()); i.n();)
    {
      i = pred(*i) ? (++r, c.erase(i)) : std::next(i);
    }

    return r;
  }

  friend void sort(auto const b, decltype(b) e)
    requires(std::is_same_v<iterator, std::remove_const_t<decltype(b)>> ||
      std::is_same_v<reverse_iterator, std::remove_const_t<decltype(b)>>)
  {
    node::sort(b, e, std::distance(b, e), std::less<value_type>());
  }

  friend void sort(auto const b, decltype(b) e, auto cmp)
    requires(std::is_same_v<iterator, std::remove_const_t<decltype(b)>> ||
      std::is_same_v<reverse_iterator, std::remove_const_t<decltype(b)>>)
  {
    node::sort(b, e, std::distance(b, e), cmp);
  }

  friend void swap(list& lhs, decltype(lhs) rhs) noexcept { lhs.swap(rhs); }
};

}

#endif // XL_LIST_HPP
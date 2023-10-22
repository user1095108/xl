#ifndef XL_LIST_HPP
# define XL_LIST_HPP
# pragma once

#include <cstdint> // std::uintptr_t
#include <algorithm> // std::move()
#include <compare> // std::three_way_comparable
#include <initializer_list>

#include "listiterator.hpp"

namespace xl
{

struct push_t{};

template <typename T>
  requires(!std::is_reference_v<T>)
class list
{
  struct node;

  friend class listiterator<node>;
  friend class listiterator<node const>;

public:
  using value_type = T;

  using difference_type = std::intmax_t;
  using size_type = std::uintmax_t;
  using reference = value_type&;
  using const_reference = value_type const&;

  using iterator = listiterator<node>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_iterator = listiterator<node const>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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

    static void destroy(node* n) noexcept(noexcept(delete n))
    {
      for (decltype(n) p{}; n; assign(n, p)(n->link(p), n), delete p);
    }

    //
    auto link() const noexcept { return (node*)(l_); }

    auto link(auto const n) const noexcept
    {
      return (node*)(std::uintptr_t(n) ^ l_);
    }

    //
    static void sort(auto const b, decltype(b) e, size_type const sz,
      auto&& c)
      noexcept(noexcept(
          std::inplace_merge(b, b, e, std::forward<decltype(c)>(c))
        )
      )
    {
      if (sz > 1)
      {
        auto const hsz(sz / 2);
        auto const m(std::next(b, hsz));

        sort(b, m, hsz, std::forward<decltype(c)>(c));
        sort(m, e, sz - hsz, std::forward<decltype(c)>(c));

        std::inplace_merge(b, m, e, std::forward<decltype(c)>(c));
      }
    }
  };

private:
  node* f_{}, *l_{};

public:
  list() = default;

  list(list const& o)
    noexcept(noexcept(list(o.begin(), o.end())))
    requires(std::is_copy_constructible_v<value_type>):
    list(o.begin(), o.end())
  {
  }

  list(list&& o) noexcept:
    f_(o.f_), l_(o.l_)
  { // we are empty, so no need for clear()
    o.f_ = o.l_ = {};
  }

  list(push_t, auto&& ...a)
    noexcept(noexcept((push_back(std::forward<decltype(a)>(a)), ...)))
  {
    (push_back(std::forward<decltype(a)>(a)), ...);
  }

  list(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(emplace_back(*i)))
  {
    std::for_each(
      i,
      j,
      [&](auto&& a) noexcept(noexcept(emplace_back(*i)))
      {
        emplace_back(std::forward<decltype(a)>(a));
      }
    );
  }

  list(std::initializer_list<value_type> l)
    noexcept(noexcept(list(l.begin(), l.end()))):
    list(l.begin(), l.end())
  {
  }

  ~list() noexcept(noexcept(node::destroy(f_))) { node::destroy(f_); }

  //
  auto& operator=(list const& o)
    noexcept(noexcept(assign(o.begin(), o.end())))
    requires(std::is_copy_constructible_v<value_type>)
  { // self-assign neglected
    assign(o.begin(), o.end());

    return *this;
  }

  auto& operator=(list&& o) noexcept(noexcept(node::destroy(f_)))
  {
    node::destroy(f_); // we are not necessarily empty

    f_ = o.f_; l_ = o.l_;
    o.f_ = o.l_ = {};

    return *this;
  }

  auto& operator=(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l)))
  {
    assign(l);

    return *this;
  }

  //
  friend auto operator==(list const& l, list const& r)
    noexcept(noexcept(
        std::equal(l.begin(), l.end(), r.begin(), r.end())
      )
    )
  {
    return std::equal(l.begin(), l.end(), r.begin(), r.end());
  }

  friend auto operator<=>(list const& l, list const& r)
    noexcept(noexcept(
        std::lexicographical_compare_three_way(
          l.begin(), l.end(), r.begin(), r.end()
        )
      )
    )
  {
    return std::lexicographical_compare_three_way(
        l.begin(), l.end(), r.begin(), r.end()
      );
  }

  //
  static constexpr size_type max_size() noexcept { return ~size_type{}; }

  bool empty() const noexcept { return !f_; }

  size_type size() const noexcept
  {
    size_type sz{};

    for (decltype(f_) n(f_), p{}; n;
      ++sz, node::assign(n, p)(n->link(p), n));

    return sz;
  }

  // iterators
  iterator begin() noexcept { return {f_, {}}; }
  iterator end() noexcept { return {{}, l_}; }

  // const iterators
  const_iterator begin() const noexcept { return {f_, {}}; }
  const_iterator end() const noexcept { return {{}, l_}; }

  const_iterator cbegin() const noexcept { return {f_, {}}; }
  const_iterator cend() const noexcept { return {{}, l_}; }

  // reverse iterators
  reverse_iterator rbegin() noexcept
  {
    return reverse_iterator(iterator({}, l_));
  }

  reverse_iterator rend() noexcept
  {
    return reverse_iterator(iterator(f_, {}));
  }

  // const reverse iterators
  const_reverse_iterator crbegin() const noexcept
  {
    return const_reverse_iterator(const_iterator({}, l_));
  }

  const_reverse_iterator crend() const noexcept
  {
    return const_reverse_iterator(const_iterator(f_, {}));
  }

  //
  auto& operator[](size_type i) noexcept
  {
    auto n(f_);

    for (decltype(n) p{}; i; --i) node::assign(n, p)(n->link(p), n);

    return n->v_;
  }

  auto const& operator[](size_type i) const noexcept
  {
    auto n(f_);

    for (decltype(n) p{}; i; --i) node::assign(n, p)(n->link(p), n);

    return n->v_;
  }

  auto& at(size_type const i) noexcept { return (*this)[i]; }
  auto& at(size_type const i) const noexcept { return (*this)[i]; }

  auto& back() noexcept { return l_->v_; }
  auto const& back() const noexcept { return l_->v_; }

  auto& front() noexcept { return f_->v_; }
  auto const& front() const noexcept { return f_->v_; }

  //
  template <int = 0>
  void assign(size_type count, auto const& v)
    noexcept(noexcept(clear(), emplace_back(v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    clear(); while (count--) emplace_back(v);
  }

  void assign(size_type const count, value_type v)
    noexcept(noexcept(assign<0>(count, std::move(v))))
  {
    assign<0>(count, std::move(v));
  }

  void assign(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(clear(), emplace_back(*i)))
    requires(std::is_assignable_v<value_type&, decltype(*i)>)
  {
    clear();

    std::for_each(
      i,
      j,
      [&](auto&& v) noexcept(noexcept(emplace_back(*i)))
      {
        emplace_back(std::forward<decltype(v)>(v));
      }
    );
  }

  void assign(std::initializer_list<value_type> l)
    noexcept(noexcept(assign(l.begin(), l.end())))
  {
    assign(l.begin(), l.end());
  }

  //
  void clear() noexcept(noexcept(node::destroy(f_)))
  {
    node::destroy(f_); f_ = l_ = {};
  }

  //
  template <int = 0>
  iterator emplace(const_iterator const i, auto&& ...a)
    noexcept(noexcept(new node{std::forward<decltype(a)>(a)...}))
    requires(std::is_constructible_v<value_type, decltype(a)...>)
  {
    auto const n(i.n()), p(n ? i.p() : l_);

    // p q n
    auto const q(new node{std::forward<decltype(a)>(a)...});
    q->l_ = node::conv(n, p);

    n ? void(n->l_ = node::conv(q, n->link(p))) : void(l_ = q);
    p ? void(p->l_ = node::conv(q, p->link(n))) : void(f_ = q);

    return {q, p}; // return iterator to created node
  }

  auto emplace(const_iterator const i, value_type v)
    noexcept(noexcept(emplace<0>(i, std::move(v))))
  {
    return emplace<0>(i, std::move(v));
  }

  template <int = 0>
  iterator emplace_back(auto&& ...a)
    noexcept(noexcept(new node{std::forward<decltype(a)>(a)...}))
    requires(std::is_constructible_v<value_type, decltype(a)...>)
  {
    // l q
    auto const l(l_), q(l_ = new node{std::forward<decltype(a)>(a)...});
    q->l_ = node::conv(l);

    l ? void(l->l_ ^= node::conv(q)) : void(f_ = q);

    return {q, l}; // return iterator to created node
  }

  auto emplace_back(value_type v)
    noexcept(noexcept(emplace_back<0>(std::move(v))))
  {
    return emplace_back<0>(std::move(v));
  }

  template <int = 0>
  iterator emplace_front(auto&& ...a)
    noexcept(noexcept(new node{std::forward<decltype(a)>(a)...}))
    requires(std::is_constructible_v<value_type, decltype(a)...>)
  {
    // q f
    auto const f(f_), q(f_ = new node{std::forward<decltype(a)>(a)...});
    q->l_ = node::conv(f);

    f ? void(f->l_ ^= node::conv(q)) : void(l_ = q);

    return {q, {}}; // return iterator to created node
  }

  auto emplace_front(value_type v)
    noexcept(noexcept(emplace_front<0>(std::move(v))))
  {
    return emplace_front<0>(std::move(v));
  }

  //
  iterator erase(const_iterator const i)
    noexcept(noexcept(delete f_))
  {
    auto const n(i.n()), p(i.p()), nxt(n->link(p));

    // p n nxt
    p ? void(p->l_ = node::conv(nxt, p->link(n))) : void(f_ = nxt);
    nxt ? void(nxt->l_ = node::conv(p, nxt->link(n))) : void(l_ = p);

    delete n;

    return {nxt, p};
  }

  iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    while (a != b) { a = erase(a); } return a;
  }

  //
  template <int = 0>
  iterator insert(const_iterator const i, auto&& v)
    noexcept(noexcept(emplace(i, std::forward<decltype(v)>(v))))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    return emplace(i, std::forward<decltype(v)>(v));
  }

  auto insert(const_iterator const i, value_type v)
    noexcept(noexcept(insert<0>(i, std::move(v))))
  {
    return insert<0>(i, std::move(v));
  }

  template <int = 0>
  iterator insert(const_iterator i, size_type count, auto const& v)
    noexcept(noexcept(emplace(i, v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    if (count) [[likely]]
    { // the parent node of i.n() changes
      auto const r(emplace(i, v));
      i = {i.n(), r.n()};

      while (--count) i = {i.n(), emplace(i, v).n()};

      return r;
    }
    else [[unlikely]]
    {
      return {i.n(), i.p()};
    }
  }

  auto insert(const_iterator i, size_type const count, value_type v)
    noexcept(noexcept(insert<0>(i, count, std::move(v))))
  {
    return insert<0>(i, count, std::move(v));
  }

  iterator insert(const_iterator i,
    std::input_iterator auto const j, decltype(j) k)
    noexcept(noexcept(emplace(i, *j)))
  {
    if (j == k) [[unlikely]]
    {
      return {i.n(), i.p()};
    }
    else [[likely]]
    {
      auto const r(emplace(i, *j));
      i = {i.n(), r.n()};

      std::for_each(
        std::next(j),
        k,
        [&](auto&& v)
          noexcept(noexcept(emplace(i, std::forward<decltype(v)>(v))))
        { // the parent node of i.n() changes
          i = {i.n(), emplace(i, std::forward<decltype(v)>(v)).n()};
        }
      );

      return r;
    }
  }

  auto insert(const_iterator const i, std::initializer_list<value_type> l)
    noexcept(noexcept(insert(i, l.begin(), l.end())))
  {
    return insert(i, l.begin(), l.end());
  }

  //
  void pop_back() noexcept(noexcept(delete f_))
  {
    auto const l0(l_), l1(l_ = l0->link());

    l1 ? void(l1->l_ = node::conv(l1->link(l0))) : void(f_ = {});

    delete l0;
  }

  void pop_front() noexcept(noexcept(delete f_))
  {
    auto const f0(f_), f1(f_ = f0->link());

    f1 ? void(f1->l_ = node::conv(f1->link(f0))) : void(l_ = {});

    delete f0;
  }

  //
  template <int = 0>
  void push_back(auto&& v)
    noexcept(noexcept(emplace_back(std::forward<decltype(v)>(v))))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    emplace_back(std::forward<decltype(v)>(v));
  }

  void push_back(value_type v)
    noexcept(noexcept(push_back<0>(std::move(v))))
  {
    push_back<0>(std::move(v));
  }

  template <int = 0>
  void push_front(auto&& v)
    noexcept(noexcept(emplace_front(std::forward<decltype(v)>(v))))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    emplace_front(std::forward<decltype(v)>(v));
  }

  void push_front(value_type v)
    noexcept(noexcept(push_front<0>(std::move(v))))
  {
    push_front<0>(std::move(v));
  }

  //
  void reverse() noexcept { std::swap(f_, l_); }

  //
  void sort(auto cmp)
    noexcept(noexcept(node::sort(begin(), end(), size(), cmp)))
  {
    node::sort(begin(), end(), size(), cmp);
  }

  void sort() noexcept(noexcept(sort(std::less<value_type>())))
  {
    sort(std::less<value_type>());
  }

  //
  void swap(list& o) noexcept
  {
    std::swap(f_, o.f_);
    std::swap(l_, o.l_);
  }
};

//////////////////////////////////////////////////////////////////////////////
template <int = 0, typename T>
inline auto erase(list<T>& c, auto const& k)
  noexcept(noexcept(std::equal_to()(std::declval<T>(), k)))
  requires(requires{std::equal_to()(std::declval<T>(), k);})
{
  return erase_if(
      c,
      [&](auto&& v) noexcept(noexcept(std::equal_to()(v, k)))
      {
        return std::equal_to()(v, k);
      }
    );
}

template <typename T>
inline auto erase(list<T>& c, T k)
  noexcept(noexcept(erase<0>(c, k)))
{
  return erase<0>(c, k);
}

template <typename T>
inline auto erase_if(list<T>& c, auto pred)
  noexcept(noexcept(pred(std::declval<T>()), c.erase(c.begin())))
{
  typename std::remove_reference_t<decltype(c)>::size_type r{};

  for (auto i(c.begin()); i.n(); pred(*i) ? ++r, i = c.erase(i) : ++i);

  return r;
}

//////////////////////////////////////////////////////////////////////////////
template <typename T>
inline void swap(list<T>& lhs, decltype(lhs) rhs) noexcept { lhs.swap(rhs); }

}

#endif // XL_LIST_HPP

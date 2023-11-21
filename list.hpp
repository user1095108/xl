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

struct multi_t{};

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
    static void append_node(auto& i, auto&& j) noexcept
    { // i  j
      i.n_->l_ = conv(i.p_, j.n_); // set-up link to j
      j.n_->l_ = conv(i.n_); // change parent of j to i
      j.p_ = i.n_;

      i = j;
    }

    static void sort(iterator& b, decltype(b) e, size_type const sz,
      auto&& c) noexcept(noexcept(c(*b, *b)))
    { // merge sort
      if (sz > 1)
      {
        iterator m;

        {
          auto const hsz(sz / 2);
          m = std::next(b, hsz);

          sort(b, m, hsz, std::forward<decltype(c)>(c));
          sort(m, e, sz - hsz, std::forward<decltype(c)>(c));
        }

        //
        iterator ni;

        {
          auto i(b), j(m);
          ni = c(*i, *j) ? i++ : j++;

          // relink b and ni
          if (b.p_) b.p_->l_ ^= conv(b.n_, ni.n_);
          (b.n_ = ni.n_)->l_ = conv(ni.p_ = b.p_);

          //
          while ((i != m) && (j != e)) append_node(ni, c(*i, *j) ? i++ : j++);
          while (i != m) append_node(ni, i++);
          while (j != e) append_node(ni, j++);
        }

        // relink ni and e
        ni.n_->l_ ^= conv(e.n_); // ni - e
        if (e.n_) e.n_->l_ ^= conv(e.p_, ni.n_); // ni - e
        e.p_ = ni.n_;
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
  { // we are empty, so no need to clear()
    o.f_ = o.l_ = {};
  }

  list(multi_t, auto&& ...a)
    noexcept(noexcept(push_back(std::forward<decltype(a)>(a)...)))
  {
    push_back(std::forward<decltype(a)>(a)...);
  }

  list(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(std::copy(i, j, std::back_inserter(*this))))
  {
    std::copy(i, j, std::back_inserter(*this));
  }

  list(std::initializer_list<value_type> l)
    noexcept(noexcept(list(l.begin(), l.end()))):
    list(l.begin(), l.end())
  {
  }

  explicit list(size_type c)
    noexcept(noexcept(emplace_back()))
    requires(std::is_default_constructible_v<value_type>)
  {
    while (c--) emplace_back();
  }

  explicit list(size_type c, auto const& v, multi_t = {})
    noexcept(noexcept(emplace_back(v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    while (c--) emplace_back(v);
  }

  explicit list(size_type const c, value_type const v)
    noexcept(noexcept(list(c, v, multi_t{}))):
    list(c, v, multi_t{})
  {
  }

  explicit list(auto&& c)
    noexcept((std::is_rvalue_reference_v<decltype(c)> &&
      noexcept(std::move(std::begin(c), std::end(c),
        std::back_inserter(*this)))) ||
      noexcept(std::copy(std::begin(c), std::end(c),
        std::back_inserter(*this))))
    requires(requires{std::begin(c), std::end(c), std::size(c);} &&
      !std::same_as<list, std::remove_cvref_t<decltype(c)>> &&
      !std::same_as<std::initializer_list<value_type>,
        std::remove_cvref_t<decltype(c)>> &&
      (std::is_constructible_v<T, decltype(*std::begin(c))> ||
      std::is_constructible_v<T, decltype(std::move(*std::begin(c)))>))
  {
    if constexpr(std::is_rvalue_reference_v<decltype(c)>)
    {
      std::move(std::begin(c), std::end(c), std::back_inserter(*this));
    }
    else
    {
      std::copy(std::begin(c), std::end(c), std::back_inserter(*this));
    }
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

  auto& operator=(auto&& c)
    noexcept((std::is_rvalue_reference_v<decltype(c)> &&
      noexcept(clear(), std::move(std::begin(c), std::end(c),
        std::back_inserter(*this)))) ||
      noexcept(clear(), std::copy(std::begin(c), std::end(c),
        std::back_inserter(*this))))
    requires(requires{std::begin(c), std::end(c), std::size(c);} &&
      !std::same_as<list, std::remove_cvref_t<decltype(c)>> &&
      !std::same_as<std::initializer_list<value_type>,
        std::remove_cvref_t<decltype(c)>> &&
      (std::is_constructible_v<T, decltype(*std::begin(c))> ||
      std::is_constructible_v<T, decltype(std::move(*std::begin(c)))>))
  {
    clear();

    if constexpr(std::is_rvalue_reference_v<decltype(c)>)
    {
      std::move(std::begin(c), std::end(c), std::back_inserter(*this));
    }
    else
    {
      std::copy(std::begin(c), std::end(c), std::back_inserter(*this));
    }

    return *this;
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

  void assign(size_type const count, value_type const v)
    noexcept(noexcept(assign<0>(count, v)))
  {
    assign<0>(count, v);
  }

  void assign(std::input_iterator auto const i, decltype(i) j)
    noexcept(noexcept(clear(), std::copy(i, j, std::back_inserter(*this))))
  {
    clear(); std::copy(i, j, std::back_inserter(*this));
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

    n ? n->l_ = node::conv(q, n->link(p)) : bool(l_ = q);
    p ? p->l_ = node::conv(q, p->link(n)) : bool(f_ = q);

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

    l ? l->l_ ^= node::conv(q) : bool(f_ = q);

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

    f ? f->l_ ^= node::conv(q) : bool(l_ = q);

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
    p ? p->l_ = node::conv(nxt, p->link(n)) : bool(f_ = nxt);
    nxt ? nxt->l_ = node::conv(p, nxt->link(n)) : bool(l_ = p);

    delete n;

    return {nxt, p};
  }

  iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    while (a != b) { a = erase(a); } return {a.n(), a.p()};
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
  iterator insert(multi_t, const_iterator i, auto&& a, auto&& ...b)
    noexcept(noexcept(emplace(i, std::forward<decltype(a)>(a)),
      (emplace(i, std::forward<decltype(b)>(b)), ...)))
    requires(requires{emplace(i, std::forward<decltype(a)>(a)),
      (emplace(i, std::forward<decltype(b)>(b)), ...);})
  {
    auto const r(emplace(i, std::forward<decltype(a)>(a)));
    i.p_ = r.n();

    ((i.p_ = emplace(i, std::forward<decltype(b)>(b)).n()), ...);

    return r;
  }

  auto insert(multi_t, const_iterator const i, value_type a)
    noexcept(noexcept(insert<0>(multi_t{}, i, std::move(a))))
  {
    return insert<0>(multi_t{}, i, std::move(a));
  }

  template <int = 0>
  iterator insert(const_iterator i, size_type count, auto const& v)
    noexcept(noexcept(emplace(i, v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    if (count) [[likely]]
    {
      auto const r(emplace(i, v));
      i.p_ = r.n(); // the parent node of i.n() changes

      while (--count) i.p_ = emplace(i, v).n();

      return r;
    }
    else [[unlikely]]
    {
      return {i.n(), i.p()};
    }
  }

  auto insert(const_iterator const i, size_type const count,
    value_type const v)
    noexcept(noexcept(insert<0>(i, count, v)))
  {
    return insert<0>(i, count, v);
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
      i.p_ = r.n();

      std::for_each(
        std::next(j),
        k,
        [&](auto&& v)
          noexcept(noexcept(emplace(i, std::forward<decltype(v)>(v))))
        { // the parent node of i.n() changes
          i.p_ = emplace(i, std::forward<decltype(v)>(v)).n();
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

    l1 ? l1->l_ = node::conv(l1->link(l0)) : bool(f_ = {});

    delete l0;
  }

  void pop_front() noexcept(noexcept(delete f_))
  {
    auto const f0(f_), f1(f_ = f0->link());

    f1 ? f1->l_ = node::conv(f1->link(f0)) : bool(l_ = {});

    delete f0;
  }

  //
  template <int = 0>
  void push_back(auto&& ...v)
    noexcept(noexcept((emplace_back(std::forward<decltype(v)>(v)), ...)))
    requires(requires{(emplace_back(std::forward<decltype(v)>(v)), ...);})
  {
    (emplace_back(std::forward<decltype(v)>(v)), ...);
  }

  void push_back(value_type v)
    noexcept(noexcept(push_back<0>(std::move(v))))
  {
    push_back<0>(std::move(v));
  }

  template <int = 0>
  void push_front(auto&& ...v)
    noexcept(noexcept((emplace_front(std::forward<decltype(v)>(v)), ...)))
    requires(requires{(emplace_front(std::forward<decltype(v)>(v)), ...);})
  {
    (emplace_front(std::forward<decltype(v)>(v)), ...);
  }

  void push_front(value_type v)
    noexcept(noexcept(push_front<0>(std::move(v))))
  {
    push_front<0>(std::move(v));
  }

  size_type remove_if(auto pred)
    noexcept(noexcept(erase(cbegin()), pred(*cbegin())))
    requires(requires{pred(*cbegin());})
  {
    size_type r{};

    for (auto i(cbegin()); i.n(); pred(*i) ? ++r, i = erase(i) : ++i);

    return r;
  }

  template <int = 0>
  auto remove(auto const& ...k)
    noexcept(noexcept(erase(cbegin()),
      (std::equal_to<>()(*cbegin(), k), ...)))
    requires(requires{(std::equal_to<>()(*cbegin(), k), ...);})
  {
    return remove_if(
        [eq(std::equal_to<>()), &k...](auto const& v)
          noexcept(noexcept((std::declval<std::equal_to<>>()(v, k), ...)))
        {
          return (eq(v, k) || ...);
        }
      );
  }

  auto remove(value_type const k) noexcept(noexcept(remove<0>(k)))
  {
    return remove<0>(k);
  }

  //
  void reverse() noexcept { node::assign(f_, l_)(l_, f_); } // swap

  //
  void sort(auto cmp)
    noexcept(noexcept(node::sort(
      std::declval<iterator&>(), std::declval<iterator&>(), size(), cmp)))
  {
    auto b(begin()), e(end());
    node::sort(b, e, size(), cmp);
    node::assign(f_, l_)(b.n(), e.p());
  }

  void sort() noexcept(noexcept(sort(std::less<value_type>())))
  {
    sort(std::less<value_type>());
  }

  //
  void swap(list& o) noexcept
  { // swap state
    node::assign(f_, l_, o.f_, o.l_)(o.f_, o.l_, f_, l_);
  }

  //
  size_type unique(auto pred)
    noexcept(noexcept(erase(cbegin()), pred(*cbegin(), *cbegin())))
    requires(requires{pred(*cbegin(), *cbegin());})
  {
    size_type r{};

    if (!empty()) [[likely]]
    {
      for (auto a(cbegin()), b(std::next(a)); b.n();)
      {
        pred(*a, *b) ? ++r, b = erase(b) : a = b++;
      }
    }

    return r;
  }

  auto unique()
    noexcept(noexcept(unique(std::equal_to<>())))
  {
    return unique(std::equal_to<>());
  }
};

//////////////////////////////////////////////////////////////////////////////
template <typename T>
inline auto erase_if(list<T>& c, auto pred)
  noexcept(noexcept(c.erase(c.cbegin()), pred(*c.cbegin())))
  requires(requires{pred(*c.cbegin());})
{
  typename std::remove_reference_t<decltype(c)>::size_type r{};

  for (auto i(c.cbegin()); i.n(); pred(*i) ? ++r, i = c.erase(i) : ++i);

  return r;
}

template <int = 0, typename T>
inline auto erase(list<T>& c, auto const& ...k)
  noexcept(noexcept(c.erase(c.cbegin()),
    (std::equal_to<>()(*c.cbegin(), k), ...)))
  requires(requires{(std::equal_to<>()(*c.cbegin(), k), ...);})
{
  return erase_if(
      c,
      [eq(std::equal_to<>()), &k...](auto const& v)
        noexcept(noexcept((std::declval<std::equal_to<>>()(v, k), ...)))
      {
        return (eq(v, k) || ...);
      }
    );
}

template <typename T>
inline auto erase(list<T>& c, T const k) noexcept(noexcept(erase<0>(c, k)))
{
  return erase<0>(c, k);
}

//////////////////////////////////////////////////////////////////////////////
template <typename U, typename V>
inline auto operator==(list<U> const& l, list<V> const& r)
  noexcept(noexcept(std::equal(l.begin(), l.end(), r.begin(), r.end())))
{
  return std::equal(l.begin(), l.end(), r.begin(), r.end());
}

template <typename U, typename V>
inline auto operator<=>(list<U> const& l, list<V> const& r)
  noexcept(noexcept(std::lexicographical_compare_three_way(
    l.begin(), l.end(), r.begin(), r.end())))
{
  return std::lexicographical_compare_three_way(
    l.begin(), l.end(), r.begin(), r.end());
}

template <typename T>
inline void swap(list<T>& l, decltype(l) r) noexcept { l.swap(r); }

}

#endif // XL_LIST_HPP

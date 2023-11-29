#ifndef XL_LIST_HPP
# define XL_LIST_HPP
# pragma once

#include <cstdint> // std::uintptr_t
#include <algorithm> // std::move()
#include <compare> // std::three_way_comparable
#include <initializer_list>
#include <ranges>

#include "listiterator.hpp"

namespace xl
{

struct from_range_t { explicit from_range_t() = default; };
inline constexpr from_range_t from_range{};

struct multi_t { explicit multi_t() = default; };
inline constexpr multi_t multi{};

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

    static auto next(auto* n, decltype(n) p, size_type i) noexcept
    {
      for (; i; --i, assign(n, p)(n->link(p), n));
      return n;
    }

    static auto next(auto i, size_type n) noexcept
    {
      for (; n; --n, ++i);
      return i;
    }

    //
    static void link_node(auto& i, auto&& j) noexcept
    { // i  j
      i.n_->l_ = conv(i.p_, j.n_); // set-up link to j
      j.n_->l_ = conv(j.p_ = i.n_); // change parent of j to i

      i = j;
    }

    static void merge(const_iterator& b, const_iterator const& m,
      decltype(b) e, auto c) noexcept(noexcept(c(*b, *b)))
    {
      auto i(b), j(m), ni(c(*i, *j) ? i++ : j++);

      // relink b and ni
      if (b.p_) b.p_->l_ ^= conv(b.n_, ni.n_);
      (b.n_ = ni.n_)->l_ = conv(ni.p_ = b.p_);

      //
      while ((i != m) && (j != e)) link_node(ni, c(*i, *j) ? i++ : j++);
      while (i != m) link_node(ni, i++);
      while (j != e) link_node(ni, j++);

      // relink ni and e
      if (e.n_) e.n_->l_ ^= conv(e.p_, ni.n_); // ni - e
      (e.p_ = ni.n_)->l_ ^= conv(e.n_); // ni - e
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

  explicit list(size_type c, auto const& v, multi_t = multi)
    noexcept(noexcept(emplace_back(v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    while (c--) emplace_back(v);
  }

  explicit list(size_type const c, value_type const v)
    noexcept(noexcept(list(c, v, multi))):
    list(c, v, multi)
  {
  }

  list(from_range_t, std::ranges::input_range auto&& rg)
    noexcept(noexcept(list(std::ranges::begin(rg), std::ranges::end(rg)))):
    list(std::ranges::begin(rg), std::ranges::end(rg))
  {
  }

  ~list() noexcept(noexcept(node::destroy(f_))) { node::destroy(f_); }

  //
  auto& operator=(list const& o)
    noexcept(noexcept(assign(o.begin(), o.end())))
    requires(std::is_copy_constructible_v<value_type>)
  { // self-assign neglected
    assign(o.begin(), o.end()); return *this;
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
    assign(l); return *this;
  }

  auto& operator=(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign_range(std::forward<decltype(rg)>(rg))))
  {
    assign_range(std::forward<decltype(rg)>(rg)); return *this;
  }

  //
  static constexpr size_type max_size() noexcept { return ~size_type{}; }

  bool empty() const noexcept { return !f_; }

  size_type size() const noexcept
  {
    auto i(cbegin()), j(cend());
    size_type sz1(i != j), sz2{};

    for (; (i.n_ != j.p_) && (i.n_ != (++sz2, --j).p_); ++sz1, ++i);

    return sz1 + sz2;
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
  auto& operator[](size_type const i) noexcept
  {
    return node::next(f_, {}, i)->v_;
  }

  auto const& operator[](size_type const i) const noexcept
  {
    return node::next(f_, {}, i)->v_;
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

  void assign_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign(std::ranges::begin(rg), std::ranges::end(rg))))
  {
    assign(std::ranges::begin(rg), std::ranges::end(rg));
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
    // i.p_, q, i.n_
    auto const q(new node{std::forward<decltype(a)>(a)...});
    q->l_ = node::conv(i.n_, i.p_);

    i.n_ ? i.n_->l_ ^= node::conv(q, i.p_) : bool(l_ = q);
    i.p_ ? i.p_->l_ ^= node::conv(q, i.n_) : bool(f_ = q);

    return {q, i.p_}; // return iterator to created node
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
    auto const l(l_), q(new node{std::forward<decltype(a)>(a)...});
    q->l_ = node::conv(l);

    l ? l->l_ ^= node::conv(q) : bool(f_ = q);

    return {l_ = q, l}; // return iterator to created node
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
    auto const f(f_), q(new node{std::forward<decltype(a)>(a)...});
    q->l_ = node::conv(f);

    f ? f->l_ ^= node::conv(q) : bool(l_ = q);

    return {f_ = q, {}}; // return iterator to created node
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
    auto const nxt(i.n_->link(i.p_));

    // i.p_, i.n_, nxt
    nxt ? nxt->l_ ^= node::conv(i.n_, i.p_) : bool(l_ = i.p_);
    i.p_ ? i.p_->l_ ^= node::conv(i.n_, nxt) : bool(f_ = nxt);

    delete i.n_;

    return {nxt, i.p_};
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
    i.p_ = r.n(); // fix iterator

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
    std::input_iterator auto j, decltype(j) k)
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
        ++j,
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
  iterator append_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(
      insert(cend(), std::ranges::begin(rg), std::ranges::end(rg))))
  {
    return insert(cend(), std::ranges::begin(rg), std::ranges::end(rg));
  }

  iterator insert_range(const_iterator const pos,
    std::ranges::input_range auto&& rg)
    noexcept(noexcept(
      insert(pos, std::ranges::begin(rg), std::ranges::end(rg))))
  {
    return insert(pos, std::ranges::begin(rg), std::ranges::end(rg));
  }

  iterator prepend_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(
      insert(cbegin(), std::ranges::begin(rg), std::ranges::end(rg))))
  {
    return insert(cbegin(), std::ranges::begin(rg), std::ranges::end(rg));
  }

  //
  void pop_back() noexcept(noexcept(delete f_))
  {
    auto const l(l_->link());

    l ? l->l_ ^= node::conv(l_) : bool(f_ = {});

    delete l_; l_ = l;
  }

  void pop_front() noexcept(noexcept(delete f_))
  {
    auto const f(f_->link());

    f ? f->l_ ^= node::conv(f_) : bool(l_ = {});

    delete f_; f_ = f;
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
  template <class Comp = std::less<value_type>>
  void merge(auto&& o, Comp cmp = Comp())
    noexcept(noexcept(node::merge(std::declval<const_iterator&>(),
      std::declval<const_iterator const&>(), std::declval<const_iterator&>(),
      cmp)))
    requires(std::same_as<list, std::remove_reference_t<decltype(o)>>)
  {
    if (empty())
    {
      f_ = o.f_; l_ = o.l_;
    }
    else if (!o.empty())
    {
      l_->l_ ^= node::conv(o.f_); // link this and o
      o.f_->l_ ^= node::conv(l_);

      auto b(cbegin()), e(o.cend()), m(o.cbegin());
      m.p_ = l_; // fix iterator

      node::merge(b, m, e, cmp);
      node::assign(f_, l_)(b.n_, e.p_);
    }

    o.f_ = o.l_ = {}; // reset o
  }

  //
  void resize(size_type const c)
    noexcept(noexcept(emplace_back(), pop_back()))
    requires(std::is_default_constructible_v<value_type>)
  {
    auto sz(size());
    for (; c > sz; ++sz, emplace_back());
    for (; c < sz; --sz, pop_back());
  }

  template <int = 0>
  void resize(size_type const c, auto const& v)
    noexcept(noexcept(emplace_back(v), pop_back()))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    auto sz(size());
    for (; c > sz; ++sz, emplace_back(v));
    for (; c < sz; --sz, pop_back());
  }

  void resize(size_type const c, value_type const v)
    noexcept(noexcept(resize<0>(c, v)))
  {
    resize<0>(c, v);
  }

  //
  template <class Comp = std::less<value_type>>
  void sort(Comp cmp = Comp()) noexcept(noexcept(cmp(*begin(), *begin())))
  { // bottom-up merge sort
    auto const next([](auto i, size_type n) noexcept
      {
        for (; n && i.n_; --n, ++i);
        return i;
      }
    );

    size_type bsize(1);

    for (auto i(cbegin());; i = cbegin(), bsize *= 2)
    {
      for (;;)
      {
        if (auto const m(next(i, bsize)); !m.n_)
        {
          if (!i.p_) return; else break;
        }
        else
        {
          auto j(next(m, bsize));

          node::merge(i, m, j, cmp);

          if (!i.p_) { f_ = i.n_; }
          if (!j.n_) { l_ = j.p_; if (!i.p_) return; else break; }

          //
          i = j;
        }
      }
    }
  }

  void splice(const_iterator const i, auto&& o, const_iterator const b,
    const_iterator const e) noexcept
    requires(std::same_as<list, std::remove_reference_t<decltype(o)>>)
  {
    if (b != e)
    { // relink i, b, e
      i.n_ ? i.n_->l_ ^= node::conv(i.p_, e.p_) : bool(l_ = e.p_);
      i.p_ ? i.p_->l_ ^= node::conv(i.n_, b.n_) : bool(f_ = b.n_);

      //
      b.n_->l_ ^= node::conv(b.p_, i.p_);
      b.p_ ? b.p_->l_ ^= node::conv(b.n_, e.n_) : bool(o.f_ = e.n_);

      //
      e.n_ ? e.n_->l_ ^= node::conv(e.p_, b.p_) : bool(o.l_ = b.p_);
      e.p_->l_ ^= node::conv(e.n_, i.n_);
    }
  }

  void splice(const_iterator const i, auto&& o,
    const_iterator const b) noexcept
  {
    i.n_ ? i.n_->l_ ^= node::conv(i.p_, b.n_) : bool(l_ = b.n_);
    i.p_ ? i.p_->l_ ^= node::conv(i.n_, b.n_) : bool(f_ = b.n_);

    // i.p_ b.n_ i.n_
    auto const e(b.n_->link(b.p_));
    b.p_ ? b.p_->l_ ^= node::conv(b.n_, e) : bool(o.f_ = e);

    // b.p_ b.n_ e
    e ? e->l_ ^= node::conv(b.n_, b.p_) : bool(o.l_ = b.p_);
    b.n_->l_ = node::conv(i.p_, i.n_);
  }

  void splice(const_iterator const i, auto&& o) noexcept
  {
    splice(i, std::forward<decltype(o)>(o), o.cbegin(), o.cend());
  }

  //
  void swap(list& o) noexcept
  { // swap state
    node::assign(f_, l_, o.f_, o.l_)(o.f_, o.l_, f_, l_);
  }

  //
  template <class Comp = std::equal_to<value_type>>
  size_type unique(Comp pred = Comp())
    noexcept(noexcept(erase(cbegin()), pred(*cbegin(), *cbegin())))
    requires(requires{pred(*cbegin(), *cbegin());})
  {
    size_type r{};

    if (!empty()) [[likely]]
    {
      for (auto b(cbegin()), a(b++); b.n();)
      {
        pred(*a, *b) ? ++r, b = erase(b) : a = b++;
      }
    }

    return r;
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

//
template <class It>
list(It, It) -> list<typename std::iterator_traits<It>::value_type>;

template <std::ranges::input_range R>
list(from_range_t, R&& rg) -> list<std::ranges::range_value_t<R>>;

}

#endif // XL_LIST_HPP

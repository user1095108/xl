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

  enum {xl_list_tag};

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
    static void destroy(const_iterator i) noexcept(noexcept(delete i.n_))
    {
      while (i) delete (++i).p_;
    }

    //
    auto link(auto* const ...n) const noexcept requires(sizeof...(n) < 2)
    {
      if constexpr(sizeof...(n)) return (node*)(
        std::uintptr_t((n, ...)) ^ l_); else return (node*)(l_);
    }

    //
    static void merge(const_iterator& b, const_iterator const m,
      decltype(b) e, auto&& c) noexcept(noexcept(c(*b, *b)))
    {
      auto i(b), j(m), ni(c(*i, *j) ? i++ : j++);

      // relink b and ni
      if (b.p_) b.p_->l_ ^= detail::conv(b.n_, ni.n_);
      (b.n_ = ni.n_)->l_ = detail::conv(ni.p_ = b.p_);

      {
        auto const link_node([&](auto&& j) noexcept
          { // ni j
            ni.n_->l_ = detail::conv(ni.p_, j.n_); // ni j
            j.n_->l_ = detail::conv(j.p_ = ni.n_); // ni j

            ni = j;
          }
        );

        while ((i != m) && (j != e)) link_node(c(*i, *j) ? i++ : j++);
        while (i != m) link_node(i++);
        while (j != e) link_node(j++);
      }

      // relink ni and e
      if (e.n_) e.n_->l_ ^= detail::conv(e.p_, ni.n_); // ni e
      (e.p_ = ni.n_)->l_ ^= detail::conv(e.n_); // ni e
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
    detail::assign(o.f_, o.l_)(nullptr, nullptr);
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
    while (c) --c, emplace_back();
  }

  explicit list(size_type c, auto const& v, int = 0)
    noexcept(noexcept(emplace_back(v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    while (c) --c, emplace_back(v);
  }

  explicit list(size_type const c, value_type const v)
    noexcept(noexcept(list(c, v, 0))):
    list(c, v, 0)
  {
  }

  list(from_range_t, std::ranges::input_range auto&& rg)
    noexcept(noexcept(list(std::ranges::begin(rg), std::ranges::end(rg)))):
    list(std::ranges::begin(rg), std::ranges::end(rg))
  {
  }

  ~list() noexcept(noexcept(node::destroy({}))) { node::destroy(cbegin()); }

  //
  auto& operator=(list const& o)
    noexcept(noexcept(assign(o.begin(), o.end())))
    requires(std::is_copy_constructible_v<value_type>)
  { // self-assign neglected
    assign(o.begin(), o.end()); return *this;
  }

  auto& operator=(list&& o) noexcept(noexcept(node::destroy({})))
  {
    node::destroy(cbegin());
    detail::assign(f_, l_, o.f_, o.l_)(o.f_, o.l_, nullptr, nullptr);

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
    size_type sz(!empty());

    for (auto i(begin()), j(end()); i.n_ != j.p_; ++sz, --j)
      if (++sz; (++i).n_ == j.p_) break;

    return sz;
  }

  // iterators
  iterator after_begin() noexcept { return {f_->link(), f_}; }
  iterator before_end() noexcept { return {l_, l_->link()}; }
  iterator begin() noexcept { return {f_, {}}; }
  iterator end() noexcept { return {{}, l_}; }

  // const iterators
  const_iterator after_begin() const noexcept { return {f_->link(), f_}; }
  const_iterator before_end() const noexcept { return {l_, l_->link()}; }
  const_iterator begin() const noexcept { return {f_, {}}; }
  const_iterator end() const noexcept { return {{}, l_}; }

  auto cafter_begin() const noexcept { return after_begin(); }
  auto cbefore_end() const noexcept { return before_end(); }
  auto cbegin() const noexcept { return begin(); }
  auto cend() const noexcept { return end(); }

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
  const_reverse_iterator rbegin() const noexcept
  {
    return const_reverse_iterator(const_iterator({}, l_));
  }

  const_reverse_iterator rend() const noexcept
  {
    return const_reverse_iterator(const_iterator(f_, {}));
  }

  auto crbegin() const noexcept { return rbegin(); }
  auto crend() const noexcept { return rend(); }

  //
  auto& operator[](size_type const i) noexcept
  {
    return *detail::next(begin(), i);
  }

  auto const& operator[](size_type const i) const noexcept
  {
    return *detail::next(begin(), i);
  }

  auto& at(size_type const i) noexcept { return (*this)[i]; }
  auto& at(size_type const i) const noexcept { return (*this)[i]; }

  auto& back() noexcept { return l_->v_; }
  auto const& back() const noexcept { return l_->v_; }

  auto& front() noexcept { return f_->v_; }
  auto const& front() const noexcept { return f_->v_; }

  //
  template <int = 0>
  void assign(size_type c, auto const& v)
    noexcept(noexcept(clear(), emplace_back(v)))
    requires(std::is_constructible_v<value_type, decltype(v)>)
  {
    clear(); while (c) --c, emplace_back(v);
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
  void clear() noexcept(noexcept(node::destroy({})))
  {
    node::destroy(cbegin()); detail::assign(f_, l_)(nullptr, nullptr);
  }

  //
  template <int = 0>
  iterator emplace(const_iterator const i, auto&& ...a)
    noexcept(noexcept(new node{std::forward<decltype(a)>(a)...}))
    requires(std::is_constructible_v<value_type, decltype(a)...>)
  { // i.p_, q, i.n_
    auto const q(new node{std::forward<decltype(a)>(a)...});
    q->l_ = detail::conv(i.n_, i.p_);

    i.n_ ? i.n_->l_ ^= detail::conv(q, i.p_) : bool(l_ = q);
    i.p_ ? i.p_->l_ ^= detail::conv(q, i.n_) : bool(f_ = q);

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
  { // l q
    auto const l(l_), q(new node{std::forward<decltype(a)>(a)...});
    q->l_ = detail::conv(l);

    l ? l->l_ ^= detail::conv(q) : bool(f_ = q);

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
  { // q f
    auto const f(f_), q(new node{std::forward<decltype(a)>(a)...});
    q->l_ = detail::conv(f);

    f ? f->l_ ^= detail::conv(q) : bool(l_ = q);

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
  { // i.p_, i.n_, nxt
    auto const nxt(i.n_->link(i.p_));

    nxt ? nxt->l_ ^= detail::conv(i.n_, i.p_) : bool(l_ = i.p_);
    i.p_ ? i.p_->l_ ^= detail::conv(i.n_, nxt) : bool(f_ = nxt);

    delete i.n_; return {nxt, i.p_};
  }

  iterator erase(const_iterator a, const_iterator const b)
    noexcept(noexcept(erase(a)))
  {
    for (; a != b; a = erase(a));
    return {a.n_, a.p_};
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
    i.p_ = r.n_; // fix iterator

    ((i.p_ = emplace(i, std::forward<decltype(b)>(b)).n_), ...);

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
      i.p_ = r.n_; // the parent node of i changes

      while (--count) i.p_ = emplace(i, v).n_;

      return r;
    }
    else [[unlikely]]
    {
      return {i.n_, i.p_};
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
      return {i.n_, i.p_};
    }
    else [[likely]]
    {
      auto const r(emplace(i, *j));
      i.p_ = r.n_;

      std::for_each(
        ++j,
        k,
        [&](auto&& v)
          noexcept(noexcept(emplace(i, std::forward<decltype(v)>(v))))
        { // the parent node of i changes
          i.p_ = emplace(i, std::forward<decltype(v)>(v)).n_;
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
  auto insert_range(const_iterator const pos,
    std::ranges::input_range auto&& rg)
    noexcept(noexcept(
      insert(pos, std::ranges::begin(rg), std::ranges::end(rg))))
  {
    return insert(pos, std::ranges::begin(rg), std::ranges::end(rg));
  }

  void append_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(std::ranges::copy(rg, std::back_inserter(*this))))
  {
    std::ranges::copy(rg, std::back_inserter(*this));
  }

  void prepend_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(std::ranges::copy(std::ranges::views::reverse(rg),
      std::front_inserter(*this))))
  {
    std::ranges::copy(std::ranges::views::reverse(rg),
      std::front_inserter(*this));
  }

  //
  void pop_back() noexcept(noexcept(delete f_))
  {
    auto const l(l_->link());

    l ? l->l_ ^= detail::conv(l_) : bool(f_ = {});

    delete l_; l_ = l;
  }

  void pop_front() noexcept(noexcept(delete f_))
  {
    auto const f(f_->link());

    f ? f->l_ ^= detail::conv(f_) : bool(l_ = {});

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

    if (!empty()) [[likely]]
    {
      auto i(cbegin());

      for (auto j(cbefore_end()); i != j;)
        if ((pred(*i) ? ++r, i = erase(i) : ++i) == j) break;
        else pred(*j) ? ++r, j = --erase(j) : --j;

      if (pred(*i)) ++r, erase(i);
    }

    return r;
  }

  template <int = 0>
  auto remove(auto const& ...k)
    noexcept(noexcept(erase(cbegin()), ((*cbegin() == k), ...)))
    requires(requires{((*cbegin() == k), ...);})
  {
    return remove_if(
        [&k...](auto& a) noexcept(noexcept(((a == k), ...)))
        {
          return ((a == k) || ...);
        }
      );
  }

  auto remove(value_type const k) noexcept(noexcept(remove<0>(k)))
  {
    return remove<0>(k);
  }

  //
  void reverse() noexcept { detail::assign(f_, l_)(l_, f_); } // swap

  //
  template <class Comp = std::less<value_type>>
  void merge(auto&& o, Comp cmp = Comp())
    noexcept(noexcept(node::merge(std::declval<const_iterator&>(),
      std::declval<const_iterator>(), std::declval<const_iterator&>(),
      cmp)))
    requires(std::same_as<list, std::remove_reference_t<decltype(o)>>)
  {
    if (empty())
    {
      detail::assign(f_, l_)(o.f_, o.l_);
    }
    else if (!o.empty())
    {
      l_->l_ ^= detail::conv(o.f_); // link this and o
      o.f_->l_ ^= detail::conv(l_);

      auto b(cbegin()), e(o.cend()), m(o.cbegin());
      m.p_ = l_; // fix iterator

      node::merge(b, m, e, cmp);
      detail::assign(f_, l_)(b.n_, e.p_);
    }

    detail::assign(o.f_, o.l_)(nullptr, nullptr); // reset o
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
  void resize(size_type const c, auto const& a)
    noexcept(noexcept(emplace_back(a), pop_back()))
    requires(std::is_constructible_v<value_type, decltype(a)>)
  {
    auto sz(size());
    for (; c > sz; ++sz, emplace_back(a));
    for (; c < sz; --sz, pop_back());
  }

  void resize(size_type const c, value_type const a)
    noexcept(noexcept(resize<0>(c, a)))
  {
    resize<0>(c, a);
  }

  //
  template <class Comp = std::less<value_type>>
  void sort(Comp cmp = Comp()) noexcept(noexcept(cmp(*cbegin(), *cbegin())))
  { // classic merge sort
    auto b(cbegin()), e(cend());

    {
      auto const sort([&](auto& s, auto& i, decltype(i) j)
        noexcept(noexcept(cmp(*b, *b))) -> void
        {
          auto m(i);

          for (auto n(j); m != n; ++m) if (m == --n) break;

          if (i != m)
          {
            s(s, i, m);
            s(s, m, j);

            node::merge(i, m, j, cmp);
          }
        }
      );

      sort(sort, b, e);
    }

    detail::assign(f_, l_)(b.n_, e.p_);
  }

  template <class Comp = std::less<value_type>>
  void sort2(Comp cmp = Comp()) noexcept(noexcept(cmp(*cbegin(), *cbegin())))
  { // bottom-up merge sort
    auto const next([](auto i, size_type n) noexcept
      {
        for (; n && i; --n, ++i);
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
    if (b != e) [[likely]]
    { // relink i, b, e
      i.n_ ? i.n_->l_ ^= detail::conv(i.p_, e.p_) : bool(l_ = e.p_);
      i.p_ ? i.p_->l_ ^= detail::conv(i.n_, b.n_) : bool(f_ = b.n_);

      //
      b.n_->l_ ^= detail::conv(b.p_, i.p_);
      b.p_ ? b.p_->l_ ^= detail::conv(b.n_, e.n_) : bool(o.f_ = e.n_);

      //
      e.n_ ? e.n_->l_ ^= detail::conv(e.p_, b.p_) : bool(o.l_ = b.p_);
      e.p_->l_ ^= detail::conv(e.n_, i.n_);
    }
  }

  void splice(const_iterator const i, auto&& o,
    const_iterator const b) noexcept
    requires(std::same_as<list, std::remove_reference_t<decltype(o)>>)
  {
    i.n_ ? i.n_->l_ ^= detail::conv(i.p_, b.n_) : bool(l_ = b.n_);
    i.p_ ? i.p_->l_ ^= detail::conv(i.n_, b.n_) : bool(f_ = b.n_);

    // i.p_ b.n_ i.n_
    auto const e(b.n_->link(b.p_));
    b.p_ ? b.p_->l_ ^= detail::conv(b.n_, e) : bool(o.f_ = e);

    // b.p_ b.n_ e
    e ? e->l_ ^= detail::conv(b.n_, b.p_) : bool(o.l_ = b.p_);
    b.n_->l_ = detail::conv(i.p_, i.n_);
  }

  void splice(const_iterator const i, auto&& o) noexcept
  {
    splice(i, std::forward<decltype(o)>(o), o.cbegin(), o.cend());
  }

  //
  void swap(list& o) noexcept
  { // swap state
    detail::assign(f_, l_, o.f_, o.l_)(o.f_, o.l_, f_, l_);
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
      for (auto a(cbegin()), b(cafter_begin()); b;
        pred(*a, *b) ? ++r, b = erase(b) : a = b++);
    }

    return r;
  }
};

//////////////////////////////////////////////////////////////////////////////
template <typename T>
inline auto erase_if(list<T>& c, auto&& pred)
  noexcept(noexcept(c.remove_if(std::forward<decltype(pred)>(pred))))
{
  return c.remove_if(std::forward<decltype(pred)>(pred));
}

template <int = 0, typename T>
inline auto erase(list<T>& c, auto const& ...k)
  noexcept(noexcept(c.remove(k...)))
{
  return c.remove(k...);
}

template <typename T>
inline auto erase(list<T>& c, T const k) noexcept(noexcept(erase<0>(c, k)))
{
  return erase<0>(c, k);
}

inline auto find_if(auto&& c, auto pred)
  noexcept(noexcept(pred(*c.begin())))
  requires(requires{std::remove_cvref_t<decltype(c)>::xl_list_tag;})
{
  if (!c.empty()) [[likely]]
  {
    auto i(c.begin());

    for (auto j(c.before_end()); i != j; --j)
      if (pred(std::as_const(*i))) return i;
      else if (pred(std::as_const(*j))) return j;
      else if (++i == j) return c.end(); // *j has already been tried

    if (pred(std::as_const(*i))) return i;
  }

  return c.end();
}

template <int = 0>
inline auto find(auto&& c, auto const& ...k)
  noexcept(noexcept(((*c.cbegin() == k), ...)))
  requires(requires{((*c.cbegin() == k), ...);})
{
  return find_if(
      std::forward<decltype(c)>(c),
      [&k...](auto const& a) noexcept(noexcept(((a == k), ...)))
      {
        return ((a == k) || ...);
      }
    );
}

template <typename T>
inline auto find(list<T>& c, T const k)
  noexcept(noexcept(find<0>(c, k)))
{
  return find<0>(c, k);
}

template <typename T>
inline auto find(list<T> const& c, T const k)
  noexcept(noexcept(find<0>(c, k)))
{
  return find<0>(c, k);
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

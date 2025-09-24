#ifndef XL_LIST_HPP
# define XL_LIST_HPP
# pragma once

#include <cstdint> // std::uintptr_t
#include <algorithm> // std::move()
#include <bit> // std::bit_width()
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
private:
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
    static void destroy(const_iterator i) noexcept(noexcept(delete i.p_))
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
      decltype(b) e, auto c) noexcept(noexcept(c(*b, *b)))
    {
      auto i(b), j(m), ni((c(*i, *j) ? ++i :
        (i.p_ ? i.p_->l_ ^= detail::conv(i.n_, j.n_) : 0,
        b.n_ = j.n_, ++j), b)); // ni = b, relink and fix parent of b, if necessary

      for (const_iterator k; (i != m) && (j != e);)
      {
        c(*i, *j) ? k = i, ++i : (k = j, ++j);

        k.n_->l_ = detail::conv(k.p_ = ni.n_); // link k to ni
        ni.n_->l_ = detail::conv(ni.p_, k.n_); // link ni to k, ni.p_ is valid

        ni = k;
      }

      // select the first remaining element k of the 2 ranges,
      // if the remaining element is i, relink e to m.p_ and fix e
      auto const k(i == m ? j :
        (e.n_ ? e.n_->l_ ^= detail::conv(e.p_, m.p_) : 0,
        (e.p_ = m.p_)->l_ ^= detail::conv(m.n_, e.n_), i));

      // link k and ni
      k.n_->l_ ^= detail::conv(k.p_, ni.n_); // link k to n
      ni.n_->l_ = detail::conv(ni.p_, k.n_); // link ni to k, ni.p_ is valid
    }

    static void insertion_sort(auto& i, decltype(i) j, auto cmp)
      noexcept(noexcept(cmp(*i, *i)))
    {
      // if ((j.p_ == i.n_) || (i == j)) return;

      for (auto m(std::next(i));;)
      {
        // find insertion point
        decltype(m) ip{}; // insertion point

        for (auto n(m); cmp(*m, n.p_->v_);)
          if ((ip = --n) == i) break;

        // splice or skip
        if (ip)
        { // splice
          auto mm(m);
          m = splice(ip, mm);

          // fix i, j range
          if (ip == i) i.n_ = mm.n_;
          if (j.p_ == mm.n_) { j.p_ = m.p_; break; }
        }
        else
          if (j.p_ == m.n_) break; else ++m; // skip
      }
    }

    static auto iter_swap(auto& a, decltype(a) b) noexcept
    {
      // a.p_ a nxta ... b.p_ b nxtb ... a.p_ a nxta
      auto nxta(a.n_->link(a.p_));
      auto nxtb(b.n_->link(b.p_));

      std::swap(a.n_->l_, b.n_->l_);
      std::swap(a.n_, b.n_);

      // fix parent
      if (b.n_ == b.p_) // ... a b ...
        nxta = b.n_, b.p_ = a.n_;
      else if (a.n_ == a.p_) // ... b a ...
        nxtb = a.n_, a.p_ = b.n_;

      // fix neighbors
      auto const anbn(detail::conv(a.n_, b.n_));

      if (a.p_) a.p_->l_ ^= anbn;
      if (b.p_) b.p_->l_ ^= anbn;
      if (nxta) nxta->l_ ^= anbn;
      if (nxtb) nxtb->l_ ^= anbn;

      //
      return std::array<decltype(nxta), 2>{nxta, nxtb};
    }

    static auto splice(auto&& i, decltype(i) b) noexcept
    {
      // if (i == b) return std::next(b);

      // i.p_ b.n_ i.n_
      if (i.n_) i.n_->l_ ^= detail::conv(i.p_, b.n_);
      if (i.p_) i.p_->l_ ^= detail::conv(i.n_, b.n_);

      // b.p_ b.n_ e
      auto const e(b.n_->link(b.p_));
      if (e) e->l_ ^= detail::conv(b.n_, b.p_);
      if (b.p_) b.p_->l_ ^= detail::conv(b.n_, e);
      b.n_->l_ = detail::conv(i.p_, i.n_);

      std::remove_cvref_t<decltype(i)> const r(e, b.p_);

      detail::assign(b.p_, i.p_)(i.p_, b.n_);

      return r;
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

  list(list&& o) noexcept
  { // we are empty, so no need to clear()
    detail::assign(f_, l_, o.f_, o.l_)(o.f_, o.l_, nullptr, nullptr);
  }

  list(multi_t, auto&& ...a)
    noexcept(noexcept(push_back(std::forward<decltype(a)>(a)...)))
    requires(!!sizeof...(a))
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
  {
    while (c) --c, emplace_back();
  }

  explicit list(size_type c, auto const& v, int = 0)
    noexcept(noexcept(emplace_back(v)))
  {
    while (c) --c, emplace_back(v);
  }

  explicit list(size_type const c, value_type const v)
    noexcept(noexcept(list(c, v, 0))):
    list(c, v, 0)
  {
  }

  explicit list(std::ranges::input_range auto&& rg)
    noexcept(noexcept(list(std::ranges::begin(rg), std::ranges::end(rg))))
    requires(!std::is_same_v<std::remove_cvref_t<decltype(rg)>, list>):
    list(std::ranges::begin(rg), std::ranges::end(rg))
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
  {
    if (this != &o) assign(o.begin(), o.end());
    return *this;
  }

  auto& operator=(list&& o) noexcept(noexcept(node::destroy({})))
  {
    if (this != &o)
      node::destroy(cbegin()),
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
    requires(!std::is_same_v<std::remove_cvref_t<decltype(rg)>, list>)
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
      if (++sz, (++i).n_ == j.p_) break;

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
    requires(!std::is_same_v<std::remove_cvref_t<decltype(rg)>, list>)
  {
    assign(std::ranges::begin(rg), std::ranges::end(rg));
  }

  void assign_range(std::ranges::input_range auto&& rg)
    noexcept(noexcept(assign(std::ranges::begin(rg), std::ranges::end(rg))))
    requires(std::is_same_v<std::remove_cvref_t<decltype(rg)>, list>)
  {
    if (this != &rg) assign(std::ranges::begin(rg), std::ranges::end(rg));
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
  iterator insert(const_iterator const i, auto&& a)
    noexcept(noexcept(emplace(i, std::forward<decltype(a)>(a))))
    requires(std::is_constructible_v<value_type, decltype(a)>)
  {
    return emplace(i, std::forward<decltype(a)>(a));
  }

  auto insert(const_iterator const i, value_type a)
    noexcept(noexcept(insert<0>(i, std::move(a))))
  {
    return insert<0>(i, std::move(a));
  }

  template <int = 0>
  iterator insert(multi_t, const_iterator i, auto&& a, auto&& ...b)
    noexcept(noexcept(emplace(i, std::forward<decltype(a)>(a)),
      (emplace(i, std::forward<decltype(b)>(b)), ...)))
  {
    auto const r(emplace(i, std::forward<decltype(a)>(a)));
    i.p_ = r.n_; // fix iterator

    ((i.p_ = emplace(i, std::forward<decltype(b)>(b)).n_), ...);

    return r;
  }

  auto insert(multi_t, const_iterator const i, value_type a)
    noexcept(noexcept(insert<0>(i, std::move(a))))
  {
    return insert<0>(i, std::move(a));
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
  void push_back(auto&& ...a)
    noexcept(noexcept((emplace_back(std::forward<decltype(a)>(a)), ...)))
    requires(!!sizeof...(a))
  {
    (emplace_back(std::forward<decltype(a)>(a)), ...);
  }

  void push_back(value_type v)
    noexcept(noexcept(push_back<0>(std::move(v))))
  {
    push_back<0>(std::move(v));
  }

  template <int = 0>
  void push_front(auto&& ...a)
    noexcept(noexcept((emplace_front(std::forward<decltype(a)>(a)), ...)))
    requires(!!sizeof...(a))
  {
    auto i(cbegin());
    (++(i = emplace(i, std::forward<decltype(a)>(a))), ...);
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
    requires(!!sizeof...(k))
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
  template <class Cmp = std::less<value_type>>
  void merge(auto&& o, Cmp cmp = Cmp())
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

      if (cmp(*m, m.p_->v_))
        node::merge(b, m, e, cmp);

      detail::assign(f_, l_)(b.n_, e.p_);
    }

    detail::assign(o.f_, o.l_)(nullptr, nullptr); // reset o
  }

  //
  template <int = 0>
  void resize(size_type const c, auto const& ...a)
    noexcept(noexcept(emplace_back(a...), pop_back()))
    requires(sizeof...(a) <= 1)
  {
    auto sz(size());
    for (; c > sz; ++sz, emplace_back(a...));
    for (; c < sz; --sz, pop_back());
  }

  void resize(size_type const c, value_type const a)
    noexcept(noexcept(resize<0>(c, a)))
  {
    resize<0>(c, a);
  }

  //
  void iter_swap(auto&& a, decltype(a) b) noexcept
      requires(
        std::same_as<const_iterator, std::remove_reference_t<decltype(a)>> ||
        std::same_as<iterator, std::remove_reference_t<decltype(a)>>)
  {
    if (a != b) [[likely]]
    {
      auto const [nxta, nxtb](node::iter_swap(a, b));

      //
      if (!a.p_) f_ = a.n_; else if (!b.p_) f_ = b.n_;
      if (!nxta) l_ = a.n_; else if (!nxtb) l_ = b.n_;
    }
  }

  //
  template <int I = 0, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp()) noexcept(noexcept(node::merge(
    std::declval<const_iterator&>(), std::declval<const_iterator>(),
    std::declval<const_iterator&>(), cmp)))
    requires(0 == I)
  { // recursive bottom-up merge sort
    struct S
    {
      struct run
      {
        struct run *prev_;

        const_iterator a_, b_;
        unsigned short sz_{};
      };

      decltype((cmp)) cmp_;
      node *b_{}, *e_{};

      static auto detach(const_iterator& i, const_iterator& j) noexcept
      {
        i.n_->l_ ^= detail::conv(i.p_);
        if (j.n_) [[likely]]
          j.n_->l_ ^= detail::conv(j.p_),
          j.p_->l_ ^= detail::conv(j.n_);

        auto const n(j.n_);
        i.p_ = j.n_ = {};

        return const_iterator{n, {}};
      }

      void merge(const_iterator& a, const_iterator& b,
        const_iterator c, const_iterator d)
        noexcept(noexcept(node::merge(a, a, a, cmp_)))
      { // merge runs [a, b) and [c, d)
        b.p_->l_ ^= detail::conv(c.n_);
        c.n_->l_ ^= detail::conv(b.p_);
        c.p_ = b.p_;

        if (cmp_(*c, c.p_->v_))
          node::merge(a, c, d, cmp_);

        b = d;
      }

      static auto next(const_iterator i, size_type n) noexcept
      {
        // assert(n && i);
        do --n, ++i; while (n && i);

        return i;
      }

      const_iterator merge_sort(struct run* const prun,
        const_iterator i)
        noexcept(noexcept(node::merge(i, i, i, cmp)))
      { // recursive bottom-up merge sort
        constexpr size_type bsize0(16);

        for (;;)
        {
          if (prun && !prun->a_) return i; // pop run
          else if (!i) [[unlikely]] break;

          auto j(next(i, bsize0));

          if (j.p_ != i.n_) [[likely]]
            node::insertion_sort(i, j, cmp_);

          auto const m(detach(i, j));

          // try to merge run with a previous run
          {
            decltype(run::sz_) sz{};
            struct run* p{};

            for (auto r(prun); r && (r->sz_ == sz);)
            { // merge with a previous run
              //assert(r->a_);
              ++sz;

              merge(i, j, r->a_, r->b_);
              r->a_ = const_iterator{}; // invalidate run, it's merged

              detail::assign(p, r)(r, r->prev_);
            }

            if (p)
            { // merge success, no need to push run
              detail::assign(p->a_, p->b_, p->sz_, i)(i, j, sz, m); // store

              continue;
            }
          }

          //assert(std::is_sorted(i, j, cmp_));
          struct run run(prun, i, j);

          i = merge_sort(&run, m); // push run
        }

        if (prun) [[likely]]
        { // merge remaining runs
          if (auto const p(prun->prev_); p) [[likely]]
            merge(p->a_, p->b_, prun->a_, prun->b_);
          else
            detail::assign(b_, e_)(prun->a_.n_, prun->b_.p_);
        }

        return const_iterator{}; // clear the stack
      }
    } s{cmp};

    s.merge_sort({}, cbegin());

    //
    detail::assign(f_, l_)(s.b_, s.e_);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp()) noexcept(noexcept(node::merge(
    std::declval<const_iterator&>(), std::declval<const_iterator>(),
    std::declval<const_iterator&>(), cmp)))
    requires(1 == I)
  { // bottom-up merge sort
    struct S
    {
      static auto detach(const_iterator& i, const_iterator& j) noexcept
      {
        i.n_->l_ ^= detail::conv(i.p_);
        if (j.n_) [[likely]]
          j.n_->l_ ^= detail::conv(j.p_),
          j.p_->l_ ^= detail::conv(j.n_);

        auto const n(j.n_);
        i.p_ = j.n_ = {};

        return const_iterator{n, {}};
      }

      static void merge(const_iterator& a, const_iterator& b,
        const_iterator c, const_iterator d, decltype((cmp)) cmp)
        noexcept(noexcept(node::merge(a, a, a, cmp)))
      { // merge runs [a, b) and [i, j)
        b.p_->l_ ^= detail::conv(c.n_);
        c.n_->l_ ^= detail::conv(b.p_);
        c.p_ = b.p_;

        if (cmp(*c, c.p_->v_))
          node::merge(a, c, d, cmp);

        b = d;
      }

      static auto next(const_iterator i, size_type n) noexcept
      {
        // assert(n && i);
        do --n, ++i; while (n && i);

        return i;
      }

      static auto merge_sort(const_iterator i, decltype((cmp)) cmp)
        noexcept(noexcept(node::merge(i, i, i, cmp)))
      { // bottom-up merge sort
        constexpr size_type bsize0(16);

        unsigned short szhi{};

        std::pair<const_iterator, const_iterator> runs[28]{};

        do
        {
          auto j(next(i, bsize0)); // advance

          if (j.p_ != i.n_) [[likely]]
            node::insertion_sort(i, j, cmp); // sort run [i, j)

          auto const m(detach(i, j)); // detach run [i, j)

          {
            decltype(szhi) sz{};

            // try to merge run [i, j) with a previous run
            for (auto r(std::begin(runs));;)
            {
              if (auto& [a, b](*r); a)
              { // merge with a valid run
                ++sz; ++r; // increase size rank

                merge(i, j, a, b, cmp); // merged run is in [i, j)
                a = const_iterator{}; // invalidate stored run
              }
              else
              {
                if (sz > szhi) szhi = sz; // update highest size rank
                detail::assign(a, b)(i, j); // store run

                break;
              }
            }
          }

          i = m;
        }
        while (i);

        { // merge remaining runs
          auto const i(std::ranges::find_if(runs,
            [](auto&& a) noexcept { return bool(std::get<0>(a)); }));
          // assert(std::end(runs) != i);

          auto& [a, b](*i); // i points to the first valid stored run

          for (auto& j: std::ranges::subrange(
            std::next(decltype(&std::as_const(*i))(i)),
            std::next(std::cbegin(runs), szhi + 1)))
            if (auto& [c, d](j); c) // merge valid runs into *i
              merge(a, b, c, d, cmp);

          return std::pair(a.n_, b.p_);
        }
      }
    };

    //
    if (!empty()) [[likely]]
      std::tie(f_, l_) = S::merge_sort(cbegin(), cmp);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp()) noexcept(noexcept(node::merge(
    std::declval<const_iterator&>(), std::declval<const_iterator>(),
    std::declval<const_iterator&>(), cmp)))
    requires(2 == I)
  { // classic merge sort
    auto b(cbegin()), m(b), e(cend());

    {
      size_type sz1{}, sz2{};

      for (auto n(e); m != n; ++sz1, ++m)
        if (++sz2, m == --n) break;

      if (!sz1) [[unlikely]] return;

      //
      struct S
      {
        static void merge_sort(const_iterator& i, decltype(i) j,
          size_type const sz, decltype((cmp)) cmp)
          noexcept(noexcept(node::merge(i, i, j, cmp)))
        {
          if (16 < sz) [[likely]]
          {
            auto m(i);

            {
              auto const hsz(sz / 2);

              { auto n(hsz); do ++m; while (--n); }

              merge_sort(i, m, hsz, cmp);
              merge_sort(m, j, sz - hsz, cmp);
            }

            if (cmp(*m, m.p_->v_))
              node::merge(i, m, j, cmp);
          }
          else if (sz > 1)
            node::insertion_sort(i, j, cmp);
        }
      };

      S::merge_sort(b, m, sz1, cmp);
      S::merge_sort(m, e, sz2, cmp);
    }

    if (cmp(*m, m.p_->v_))
      node::merge(b, m, e, cmp);

    //
    detail::assign(f_, l_)(b.n_, e.p_);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp()) noexcept(noexcept(node::merge(
    std::declval<const_iterator&>(), std::declval<const_iterator>(),
    std::declval<const_iterator&>(), cmp)))
    requires(3 == I)
  {
    auto b(cbegin()), e(cend());

    //
    struct S
    {
      static void merge_sort(const_iterator& i, decltype(i) j,
        decltype((cmp)) cmp)
        noexcept(noexcept(node::merge(i, i, j, cmp)))
      {
        if ((j.p_ != i.n_) && (i != j)) [[likely]]
        {
          auto m(i);

          {
            size_type sz(1);

            for (auto n(j); m.n_ != n.p_; ++sz, ++m)
              if (++sz, m.n_ == (--n).p_) break;

            if (16 >= sz) { node::insertion_sort(i, j, cmp); return; }
          }

          merge_sort(i, m, cmp);
          merge_sort(m, j, cmp);

          if (cmp(*m, m.p_->v_))
            node::merge(i, m, j, cmp);
        }
      }
    };

    S::merge_sort(b, e, cmp);

    //
    detail::assign(f_, l_)(b.n_, e.p_);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp()) noexcept(noexcept(node::merge(
    std::declval<const_iterator&>(), std::declval<const_iterator>(),
    std::declval<const_iterator&>(), cmp)))
    requires(4 == I)
  { // bottom-up merge sort
    auto b(cbegin()), e(cend());

    //
    struct S
    {
      static auto next(const_iterator i, size_type n, decltype(i) const e)
        noexcept
      {
        // assert(i != e);
        do --n, ++i; while (n && (i != e));

        return i;
      }

      static void nonrecursive_sort(const_iterator& b, decltype(b) e,
        decltype((cmp)) cmp)
        noexcept(noexcept(node::merge(b, b, e, cmp)))
      { // bottom-up merge sort
        constexpr size_type bsize0(16);

        for (auto i(b);;)
        { // sort blocks of bsize0 elements or less
          auto m(next(i, bsize0, e));

          if (m.p_ != i.n_) [[likely]]
          {
            node::insertion_sort(i, m, cmp);

            if (i.p_ == b.p_) [[unlikely]] b.n_ = i.n_; // fix b
          }

          if (m == e) [[unlikely]] { e.p_ = m.p_; break; } // fix e

          i = m; // advance
        }

        size_type bsize(bsize0);

        for (auto i(b);; bsize *= 2, i = b)
        {
          for (;;) // merge runs
          {
            if (auto m(next(i, bsize, e)); m != e) [[likely]] // !!!
            {
              auto j(next(m, bsize, e));

              if (cmp(*m, m.p_->v_)) // skip merge?
              {
                node::merge(i, m, j, cmp);
                // assert(std::is_sorted(i, j, cmp));

                if (i.p_ == b.p_) [[unlikely]] b.n_ = i.n_; // fix b
              }

              if (j == e) [[unlikely]] { e.p_ = j.p_; break; } // fix e

              i = j; // advance
            }
            else [[unlikely]]
              break;
          }

          if (i == b) [[unlikely]] break;
        }
        // assert(std::is_sorted(b, e, cmp));
      }

      static void merge_sort(const_iterator& i, decltype(i) j,
        decltype((cmp)) cmp, unsigned short depth = {})
        noexcept(noexcept(node::merge(i, i, j, cmp)))
      {
        if ((j.p_ != i.n_) && (i != j)) [[likely]]
        {
          if (10 == depth) { nonrecursive_sort(i, j, cmp); return; }

          auto m(i);

          {
            size_type sz(1);

            for (auto n(j); m.n_ != n.p_; ++sz, ++m)
              if (++sz, m.n_ == (--n).p_) break;

            if (16 >= sz) { node::insertion_sort(i, j, cmp); return; }
          }
 
          merge_sort(i, m, cmp, ++depth);
          merge_sort(m, j, cmp, depth);

          if (cmp(*m, m.p_->v_))
            node::merge(i, m, j, cmp);
        }
      }
    };

    S::merge_sort(b, e, cmp);

    //
    detail::assign(f_, l_)(b.n_, e.p_);
  }

  //
  void splice(const_iterator const i, auto&& o, const_iterator const b,
    const_iterator const e) noexcept
    requires(std::same_as<list, std::remove_reference_t<decltype(o)>>)
  {
    if ((b == e) || ((i == e) && (this == &o))) [[unlikely]] return;

    // relink i, b, e
    i.n_ ? i.n_->l_ ^= detail::conv(i.p_, e.p_) : bool(l_ = e.p_);
    i.p_ ? i.p_->l_ ^= detail::conv(i.n_, b.n_) : bool(f_ = b.n_);

    //
    b.n_->l_ ^= detail::conv(b.p_, i.p_);
    b.p_ ? b.p_->l_ ^= detail::conv(b.n_, e.n_) : bool(o.f_ = e.n_);

    //
    e.n_ ? e.n_->l_ ^= detail::conv(e.p_, b.p_) : bool(o.l_ = b.p_);
    e.p_->l_ ^= detail::conv(e.n_, i.n_);
  }

  void splice(const_iterator const i, auto&& o,
    const_iterator const b) noexcept
    requires(std::same_as<list, std::remove_reference_t<decltype(o)>>)
  {
    if ((i == b) || (i.p_ == b.n_)) [[unlikely]] return;

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
  template <class Cmp = std::equal_to<value_type>>
  size_type unique(Cmp pred = Cmp())
    noexcept(noexcept(erase(cbegin()), pred(*cbegin(), *cbegin())))
    requires(requires{pred(*cbegin(), *cbegin());})
  {
    size_type r{};

    if (!empty()) [[likely]]
      for (auto a(cbegin()), b(cafter_begin()); b;
        pred(*a, *b) ? ++r, b = erase(b) : (a = b, ++b));

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
  requires(!!sizeof...(k))
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
inline auto operator==(std::ranges::input_range auto const& l,
  std::ranges::input_range auto const& r)
  noexcept(noexcept(std::equal(l.begin(), l.end(), r.begin(), r.end())))
  requires(requires{std::remove_cvref_t<decltype(l)>::xl_list_tag;} ||
    requires{std::remove_cvref_t<decltype(r)>::xl_list_tag;})
{
  return std::equal(l.begin(), l.end(), r.begin(), r.end());
}

inline auto operator<=>(std::ranges::input_range auto const& l,
  std::ranges::input_range auto const& r)
  noexcept(noexcept(std::lexicographical_compare_three_way(
    l.begin(), l.end(), r.begin(), r.end())))
  requires(requires{std::remove_cvref_t<decltype(l)>::xl_list_tag;} ||
    requires{std::remove_cvref_t<decltype(r)>::xl_list_tag;})
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
list(R&&) -> list<std::ranges::range_value_t<R>>;

template <std::ranges::input_range R>
list(from_range_t, R&&) -> list<std::ranges::range_value_t<R>>;

}

#endif // XL_LIST_HPP

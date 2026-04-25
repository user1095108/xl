private:
  struct merge_sort
  { // non-recursive bottom-up merge sort
    enum: size_type { bsize0 = 16 };

    static void merge(const_iterator& a, const_iterator& b,
      const_iterator& c, const_iterator& d, auto&& cmp)
      noexcept(noexcept(node::merge(a, b, c, d, cmp)))
    { // merge runs [a, b) and [c, d)
      if (cmp(*c, b.p_->v_))
        node::merge(a, b, c, d, cmp);
      else
        b.p_->l_ ^= detail::conv(c.n_),
        c.n_->l_ ^= detail::conv(b.p_);

      detail::assign(b, c)(d, a);
    }

    static auto sort(const_iterator i, decltype(i) const e, auto&& cmp)
      noexcept(noexcept(node::merge(i, i, i, cmp)))
    {
      std::pair<const_iterator, const_iterator> runs[
        sizeof(unsigned) * CHAR_BIT];

      unsigned mask{}; // occupancy mask

      do
      {
        auto j(i);

        [&]<auto ...Is>(std::index_sequence<Is...>) noexcept
        { // advance
          void((((void(Is), e) != ++j) && ...));
        }(std::make_index_sequence<bsize0>{});

        if (j.p_ != i.n_) [[likely]]
          node::insertion_sort(i, j, cmp); // sort run [i, j)

        auto const m(node::detach(i, j)); // detach run [i, j)

        // merge run [i, j) with valid stored runs
        auto r(runs);

        for (auto const end(runs + std::countr_one(mask++)); end != r;)
        {
          auto& [a, b](*r++);
          merge(a, b, i, j, cmp);
        }

        detail::assign(r->first, r->second, i)(i, j, m); // *r = {i, j}, i = m
      }
      while (e != i);

      auto& [a, b](runs[std::countr_zero(mask)]); // first valid stored run

      while (mask &= mask - 1)
      { // merge remaining valid stored runs
        auto& [c, d](runs[std::countr_zero(mask)]);
        merge(c, d, a, b, cmp);
      }

      return std::pair(a.n_, b.p_);
    }
  };

  template <class Cmp>
  struct merge_sort1
  { // recursive bottom-up merge sort
    enum: size_type { bsize0 = 16 };

    struct run
    {
      struct run *prev_;

      const_iterator a_, b_;
      unsigned short sz_{};
    };

    Cmp cmp_;
    const_iterator const e_;

    node *f_, *l_;

    void merge(const_iterator& a, const_iterator& b,
      const_iterator& c, const_iterator& d)
      noexcept(noexcept(node::merge(a, b, c, d, cmp_)))
    { // merge runs [a, b) and [c, d)
      if (cmp_(*c, b.p_->v_))
        node::merge(a, b, c, d, cmp_);
      else
        b.p_->l_ ^= detail::conv(c.n_),
        c.n_->l_ ^= detail::conv(b.p_);

      detail::assign(b, c)(d, a);
    }

    const_iterator operator()(struct run* const prun, const_iterator i)
      noexcept(noexcept(node::merge(i, i, i, cmp_)))
    { // recursive bottom-up merge sort
      for (;;)
      { // we are locked in this loop until e_ is encountered
        if (prun && !prun->a_) return i; // pop invalid stored run
        else if (e_ == i) [[unlikely]] break;

        auto j(i);

        [&]<auto ...Is>(std::index_sequence<Is...>) noexcept
        { // advance
          void((((void(Is), e_) != ++j) && ...));
        }(std::make_index_sequence<bsize0>{});

        if (j.p_ != i.n_) [[likely]]
          node::insertion_sort(i, j, cmp_);

        auto const m(node::detach(i, j));

        { // try to merge run with a valid stored run
          decltype(run::sz_) sz{};
          struct run* p{};

          for (auto r(prun); r && (r->sz_ == sz);)
          { // merge with a previous valid stored run
            // assert(r->a_);
            ++sz;

            merge(r->a_, r->b_, i, j);
            r->a_ = {}; // invalidate stored run, it's merged

            detail::assign(p, r)(r, r->prev_);
          }

          if (p)
          { // merge success, no need to push run
            detail::assign(p->a_, p->b_, p->sz_, i)(i, j, sz, m); // store

            continue;
          }
        }

        // assert(std::is_sorted(i, j, cmp_));
        struct run run(prun, i, j);

        i = (*this)(&run, m); // push run
      }

      if (prun) [[likely]] // merge remaining runs
      {
        if (auto const p(prun->prev_); p) [[likely]]
          merge(p->a_, p->b_, prun->a_, prun->b_);
        else
          detail::assign(f_, l_)(prun->a_.n_, prun->b_.p_);
      }

      return e_; // clear the stack
    }
  };

  struct merge_sort2
  { // recursive top-down merge sort
    enum: size_type { bsize0 = 16 };

    static void sort(const_iterator& i, decltype(i) j, size_type const sz,
      auto&& cmp)
      noexcept(noexcept(node::merge(i, i, j, cmp)))
    {
      if (bsize0 < sz) [[likely]]
      {
        auto m(i);

        {
          auto const hsz(sz / 2);

          { auto n(hsz); do ++m; while (--n); }

          sort(i, m, hsz, std::forward<decltype(cmp)>(cmp));
          sort(m, j, sz - hsz, std::forward<decltype(cmp)>(cmp));
        }

        if (cmp(*m, m.p_->v_))
          node::merge(i, m, j, cmp);
      }
      else if (sz > 1)
        node::insertion_sort(i, j, cmp);
    }
  };

  struct merge_sort3
  { // recursive top-down merge sort
    enum: size_type { bsize0 = 16 };

    static void sort(const_iterator& i, decltype(i) j, auto&& cmp)
      noexcept(noexcept(node::merge(i, i, j, cmp)))
    {
      if ((j.p_ == i.n_) && (i == j)) return;

      auto m(i);

      {
        size_type sz(1);

        for (auto n(j); m.n_ != n.p_; ++sz, ++m)
          if (++sz, m.n_ == (--n).p_) break;

        if (bsize0 >= sz) { node::insertion_sort(i, j, cmp); return; }
      }

      sort(i, m, std::forward<decltype(cmp)>(cmp));
      sort(m, j, std::forward<decltype(cmp)>(cmp));

      if (cmp(*m, m.p_->v_))
        node::merge(i, m, j, cmp);
    }
  };

  struct merge_sort4
  { // non-recursive bottom-up merge sort
    enum: size_type { bsize0 = 16 };

    static const_iterator next(const_iterator i, size_type n,
      const_iterator const e) noexcept
    {
      // assert(n && i);
      do --n, ++i; while (n && (e != i));

      return i;
    }

    static void sort(const_iterator& b, decltype(b) e, auto&& cmp)
      noexcept(noexcept(node::merge(b, b, e, cmp)))
    { // bottom-up merge sort
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
  };

public:
  template <int I = 0, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
  noexcept(noexcept(merge_sort::sort(cbegin(), cend(), cmp)))
  requires(0 == I)
  { // bottom-up merge sort
    if (!empty()) [[likely]]
      std::tie(f_, l_) = merge_sort::sort(cbegin(), cend(), cmp);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
  noexcept(noexcept(merge_sort1<Cmp&&>{std::forward<Cmp>(cmp), cend(), {}, {}}
    ({}, cbegin())))
  requires(1 == I)
  {
    auto s(merge_sort1<Cmp&&>{std::forward<Cmp>(cmp), cend(), {}, {}});
    s({}, cbegin());
    detail::assign(f_, l_)(s.f_, s.l_);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
  noexcept(noexcept(merge_sort2::sort(std::declval<const_iterator&>(),
    std::declval<const_iterator&>(), {}, cmp)))
  requires(2 == I)
  { // classic merge sort
    auto b(cbegin()), m(b), e(cend());

    {
      size_type sz1{}, sz2{};

      for (auto n(e); m != n; ++sz1, ++m)
        if (++sz2, m == --n) break;

      if (!sz1) [[unlikely]] return;

      merge_sort2::sort(b, m, sz1, cmp);
      merge_sort2::sort(m, e, sz2, cmp);
    }

    if (cmp(*m, m.p_->v_))
      node::merge(b, m, e, cmp);

    detail::assign(f_, l_)(b.n_, e.p_);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
  noexcept(noexcept(merge_sort3::sort(std::declval<const_iterator&>(),
    std::declval<const_iterator&>(), cmp)))
  requires(3 == I)
  {
    auto b(cbegin()), e(cend());

    merge_sort3::sort(b, e, cmp);

    detail::assign(f_, l_)(b.n_, e.p_);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
  noexcept(noexcept(merge_sort4::sort(std::declval<const_iterator&>(),
    std::declval<const_iterator&>(), cmp)))
  requires(4 == I)
  {
    if (empty()) [[unlikely]] return;

    auto b(cbegin()), e(cend());

    merge_sort4::sort(b, e, cmp);

    detail::assign(f_, l_)(b.n_, e.p_);
  }

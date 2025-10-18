private:
  struct merge_sort1
  { // non-recursive bottom-up merge sort
    static auto next(const_iterator i, size_type n,
      decltype(i) const e) noexcept
    {
      // assert(n && i);
      do --n, ++i; while (n && (i != e));

      return i;
    }

    static void merge(const_iterator& a, const_iterator& b,
      const_iterator& c, const_iterator& d, auto&& cmp)
      noexcept(noexcept(node::merge(a, a, a, cmp)))
    { // merge runs [a, b) and [c, d)
      b.p_->l_ ^= detail::conv(c.n_);
      c.n_->l_ ^= detail::conv(b.p_);

      if (cmp(*c, (c.p_ = b.p_)->v_))
        node::merge(a, c, d, cmp);

      detail::assign(b, c)(d, a);
    }

    static auto sort(const_iterator i, decltype(i) const e, auto&& cmp)
      noexcept(noexcept(node::merge(i, i, i, cmp)))
    {
      constexpr size_type bsize0(16);

      std::pair<const_iterator, const_iterator> const* endr{};

      std::pair<const_iterator, const_iterator> runs[
        sizeof(std::size_t) * CHAR_BIT]{};

      do
      {
        auto j(next(i, bsize0, e)); // advance

        if (j.p_ != i.n_) [[likely]]
          node::insertion_sort(i, j, cmp); // sort run [i, j)

        auto const m(node::detach(i, j)); // detach run [i, j)

        // try to merge run [i, j) with a valid stored run
        for (auto r(std::begin(runs));; ++r)
          if (auto& [a, b](*r); a)
          { // merge run [i, j) with a valid stored run
            merge(a, b, i, j, cmp); // merged run is in [i, j)
            a = {}; // invalidate stored run
          }
          else
          {
            detail::assign(a, b)(i, j); // store run
            if (r > endr) endr = r; // update highest size rank

            break;
          }

        i = m;
      }
      while (i);

      // merge remaining runs
      auto j(std::ranges::find_if(runs,
        [](auto&& a) noexcept { return bool(std::get<0>(a)); }));
      // assert(std::end(runs) != j);

      auto& [a, b](*j); // first valid stored run

      for (++j, ++endr; endr != j; ++j) // merge valid stored runs
        if (auto& [c, d](*j); c) merge(c, d, a, b, cmp);

      return std::pair(a.n_, b.p_);
    }
  };

  struct merge_sort2
  { // recursive top-down merge sort
    static void sort(const_iterator& i, decltype(i) j, size_type const sz,
      auto&& cmp)
      noexcept(noexcept(node::merge(i, i, j, cmp)))
    {
      if (16 < sz) [[likely]]
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
    static void sort(const_iterator& i, decltype(i) j, auto&& cmp)
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

        sort(i, m, std::forward<decltype(cmp)>(cmp));
        sort(m, j, std::forward<decltype(cmp)>(cmp));

        if (cmp(*m, m.p_->v_))
          node::merge(i, m, j, cmp);
      }
    }
  };

  struct merge_sort4
  { // non-recursive bottom-up merge sort
    static auto next(const_iterator i, size_type n,
      decltype(i) const e) noexcept
    {
      // assert(n && i);
      do --n, ++i; while (n && (i != e));

      return i;
    }

    static void sort(const_iterator& b, decltype(b) e, auto&& cmp)
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

      auto bsize(bsize0);

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
  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
    noexcept(noexcept(node::merge(std::declval<const_iterator&>(),
    std::declval<const_iterator>(), std::declval<const_iterator&>(), cmp)))
    requires(1 == I)
  { // bottom-up merge sort
    if (!empty()) [[likely]]
      std::tie(f_, l_) = merge_sort1::sort(cbegin(), cend(), cmp);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
    noexcept(noexcept(node::merge(std::declval<const_iterator&>(),
    std::declval<const_iterator>(), std::declval<const_iterator&>(), cmp)))
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
    noexcept(noexcept(node::merge(std::declval<const_iterator&>(),
      std::declval<const_iterator>(), std::declval<const_iterator&>(), cmp)))
    requires(3 == I)
  {
    auto b(cbegin()), e(cend());

    merge_sort3::sort(b, e, cmp);

    detail::assign(f_, l_)(b.n_, e.p_);
  }

  template <int I, class Cmp = std::less<value_type>>
  void sort(Cmp&& cmp = Cmp())
    noexcept(noexcept(node::merge(std::declval<const_iterator&>(),
      std::declval<const_iterator>(), std::declval<const_iterator&>(), cmp)))
    requires(4 == I)
  {
    if (empty()) [[unlikely]] return;

    auto b(cbegin()), e(cend());

    merge_sort4::sort(b, e, cmp);

    detail::assign(f_, l_)(b.n_, e.p_);
  }

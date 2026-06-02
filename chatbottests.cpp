#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <random>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "list.hpp"

void test()
{
  // TC-01  Type Traits, Concepts, and Aliases
  {
    static_assert(std::is_same_v<xl::list<int>::value_type,      int>);
    static_assert(std::is_same_v<xl::list<int>::reference,       int&>);
    static_assert(std::is_same_v<xl::list<int>::const_reference, const int&>);
    static_assert(std::bidirectional_iterator<xl::list<int>::iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::const_iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::reverse_iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::const_reverse_iterator>);

    static_assert(std::ranges::bidirectional_range<xl::list<int>>);
    static_assert(std::ranges::bidirectional_range<xl::list<int> const>);
  }

  // TC-02  Construction and Destruction
  {
    // default
    xl::list<int> l1; assert(l1.empty());

    // fill: (count) and (count, value)
    xl::list<int> l2(5);     assert(l2.size() == 5);
    xl::list<int> l3(5, 42); assert(l3.size() == 5 && l3.front() == 42);
    for (const auto& e : l3) assert(e == 42);

    // fill edge cases
    xl::list<int> fill1(1, 77);
    assert(fill1.size() == 1 && fill1.front() == 77 && fill1.back() == 77);
    xl::list<int> fill0(0, 99);
    assert(fill0.empty());

    // initialiser-list
    xl::list l4 = {1, 2, 3, 4, 5};
    assert(l4.size() == 5 && l4.back() == 5);

    // iterator-range
    int arr[] = {1, 2, 3, 4, 5};
    xl::list<int> l5(std::begin(arr), std::end(arr));
    assert(l5.size() == 5 && l5.front() == 1 && l5.back() == 5);

    // from_range tag + C array
    xl::list l6(xl::from_range, arr);
    assert(l6.size() == 5 && l6.front() == 1 && l6.back() == 5);

    // from_range + string_view (palindrome detection)
    xl::list pal(xl::from_range, std::string_view("racecar"));
    while (pal.size() > 1) {
      assert(pal.front() == pal.back());
      pal.pop_front(); pal.pop_back();
    }
    assert(pal.size() == 1);

    // odd-length palindrome
    xl::list pal2(xl::from_range, std::string_view("abcba"));
    while (pal2.size() > 1) {
      assert(pal2.front() == pal2.back());
      pal2.pop_front(); pal2.pop_back();
    }
    assert(pal2.size() == 1 && pal2.front() == 'c');

    // multi_t constructor
    xl::list lm{xl::multi, 1, 2, 3, 4, 5};
    assert(lm.size() == 5 && lm.front() == 1 && lm.back() == 5);

    // views::iota range constructor + range assignment
    xl::list l7(std::views::iota(0, 100));
    assert(l7.size() == 100);
    l7 = std::views::iota(0, 10);
    assert(l7.size() == 10);

    // empty range
    std::vector<int> ev;
    xl::list<int> el(ev.begin(), ev.end());
    assert(el.empty());

    // single-pointer range
    int single = 42;
    xl::list<int> sl(&single, &single + 1);
    assert(sl.size() == 1 && sl.front() == 42);

    // istream_iterator range
    std::istringstream iss("1 2 3 4");
    xl::list<int> il(std::istream_iterator<int>(iss), std::istream_iterator<int>{});
    assert(il.size() == 4 && il.back() == 4);

    // range-constructed equals C array
    int const ca[10]{};
    assert(std::ranges::equal(ca, xl::list(ca)));

    // from std::vector iterator range
    std::vector<int> src_vec = {10, 20, 30, 40, 50};
    xl::list<int> from_vec(src_vec.begin(), src_vec.end());
    assert(std::ranges::equal(from_vec, src_vec));

    // copy of empty list is empty
    xl::list<int> empty_src;
    xl::list<int> empty_copy(empty_src);
    assert(empty_copy.empty());

    // move from empty list
    {
      xl::list<int> src;
      xl::list<int> dst(std::move(src));
      assert(src.empty());
      assert(dst.empty());
    }
  }

  // TC-03  Assignment, Swap, and Self-Operations
  {
    // copy constructor
    xl::list l1{1, 2, 3, 4, 5};
    xl::list l2(l1);
    assert(l2.size() == 5 && l2.front() == 1 && l2.back() == 5);

    // move constructor; source is emptied
    xl::list l3(std::move(l1));
    assert(l1.empty()); // NOLINT
    assert(l3.size() == 5 && l3.front() == 1 && l3.back() == 5);

    // copy assignment
    l1 = l2;
    assert(l1.size() == 5 && l1.front() == 1 && l1.back() == 5);

    // move assignment; source is emptied
    xl::list<int> l4;
    l4 = std::move(l2);
    assert(l2.empty()); // NOLINT
    assert(l4.size() == 5);

    // self copy-assignment guard
    l4 = l4;
    assert(l4.size() == 5 && l4.front() == 1 && l4.back() == 5);

    // self move-assignment guard (implementation-defined state, but no crash/leak)
    {
      xl::list l5{11, 22, 33};
      auto& ref = l5;
      l5 = std::move(ref); // must not crash or corrupt memory
    }

    // self-swap
    {
      xl::list<int> l = {1, 2, 3};
      l.swap(l);
      assert((l == xl::list<int>{1, 2, 3}));
    }

    // swap member and std::swap
    xl::list a = {1, 2, 3}, b = {4, 5};
    a.swap(b);
    assert(a.size() == 2 && a.front() == 4 && a.back() == 5);
    assert(b.size() == 3 && b.front() == 1 && b.back() == 3);
    std::swap(a, b);
    assert(a.size() == 3 && a.front() == 1 && a.back() == 3);
    assert(b.size() == 2 && b.front() == 4 && b.back() == 5);

    // swap two empty lists
    {
      xl::list<int> e1, e2;
      e1.swap(e2);
      assert(e1.empty() && e2.empty());
    }

    // swap empty with non-empty
    {
      xl::list ne = {1, 2, 3};
      xl::list<int> em;
      ne.swap(em);
      assert(ne.empty());
      assert(em.size() == 3 && em.front() == 1 && em.back() == 3);
    }

    // copy assignment from empty
    {
      xl::list<int> src;
      xl::list<int> dst = {1, 2, 3};
      dst = src;
      assert(dst.empty());
    }

    // copy-assign to non-empty destination (destination is fully replaced)
    {
      xl::list dst3 = {100, 200, 300, 400};
      xl::list src3 = {1, 2};
      dst3 = src3;
      assert(dst3.size() == 2 && dst3.front() == 1 && dst3.back() == 2);
      assert(src3.size() == 2 && src3.front() == 1);
    }

    // move-assign to non-empty destination
    {
      xl::list dst4 = {100, 200, 300};
      xl::list src4 = {7, 8, 9, 10};
      dst4 = std::move(src4);
      assert(src4.empty());
      assert(dst4.size() == 4 && dst4.front() == 7 && dst4.back() == 10);
    }

    // iterator stability: copy produces independent clone; move transfers ownership
    xl::list orig = {10, 20, 30, 40, 50};
    auto it = std::next(orig.begin(), 2); assert(*it == 30);
    xl::list copy = orig;
    *it = 99;
    assert((orig == xl::list{10, 20, 99, 40, 50}));
    assert((copy == xl::list{10, 20, 30, 40, 50}));
    xl::list moved = std::move(orig);
    assert(orig.empty()); // NOLINT
    assert((moved == xl::list{10, 20, 99, 40, 50}));

    // assign from initialiser list to empty list
    xl::list<int> lst;
    lst.assign({7, 8, 9});
    assert(lst.size() == 3 && lst.front() == 7 && lst.back() == 9);

    // assign(ilist) replaces content
    lst.assign({4, 5, 6, 7});
    assert(lst.size() == 4 && lst.front() == 4 && lst.back() == 7);

    // operator=(ilist) replaces content
    lst = {1, 2, 3, 4, 5};
    assert(lst.size() == 5 && lst.back() == 5);

    // assign(count, value)
    lst.assign(3, 200);
    assert(lst.size() == 3 && lst.front() == 200);

    // assign(iter, iter) from array
    int arr[] = {7, 8, 9, 10};
    lst.assign(std::begin(arr), std::end(arr));
    assert(lst.size() == 4 && lst.front() == 7 && lst.back() == 10);

    // assign(count=0, value) produces empty list
    lst.assign(0, 999);
    assert(lst.empty());

    // assign(count=1, value) produces single-element list
    lst.assign(1, 42);
    assert(lst.size() == 1 && lst.front() == 42 && lst.back() == 42);

    // assign replaces list that is larger than new count
    xl::list<int> big(10, 5);
    big.assign(3, 1);
    assert(big.size() == 3);
    for (auto v : big) assert(v == 1);

    // assign replaces list that is smaller than new count
    xl::list<int> small(2, 5);
    small.assign(6, 7);
    assert(small.size() == 6);
    for (auto v : small) assert(v == 7);

    // move-only type (unique_ptr)
    xl::list<std::unique_ptr<int>> pl;
    pl.push_back(std::make_unique<int>(42));
    pl.emplace_back(new int(100));
    assert(*pl.front() == 42);
    xl::list ml = std::move(pl);
    assert(pl.empty() && ml.size() == 2);
    xl::list nl(std::move(ml));
    assert(ml.empty() && nl.size() == 2);

    // large-element type uses move, not copy
    struct LargeData {
      std::array<int, 1024> buffer;
      LargeData(int val) { buffer.fill(val); }
    };
    xl::list<LargeData> lsrc;
    lsrc.emplace_back(1); lsrc.emplace_back(2);
    xl::list<LargeData> ldst = std::move(lsrc);
    assert(lsrc.empty() && ldst.size() == 2);
    assert(ldst.front().buffer[0] == 1 && ldst.back().buffer[0] == 2);

    // std::string: push_back, copy, move
    xl::list<std::string> sl;
    sl.push_back("hello"); sl.push_back("world");
    assert(sl.size() == 2);
    xl::list<std::string> sl2 = std::move(sl);
    assert(sl.empty() && sl2.size() == 2);
    sl = std::move(sl2);
    assert(sl2.empty() && sl.size() == 2);
    xl::list<std::string> sl3 = sl; // copy
    assert(sl3.size() == 2 && sl.size() == 2);

    // move-construct list of strings: strings are actually moved
    xl::list<std::string> src2;
    src2.emplace_back("hello"); src2.emplace_back("world");
    xl::list<std::string> dst2 = std::move(src2);
    assert(src2.empty());
    assert(dst2.size() == 2 && dst2.front() == "hello" && dst2.back() == "world");
  }

  // TC-04  Element Access and Capacity
  {
    xl::list<int> l;
    assert(l.empty() && l.size() == 0);

    l.push_back(1, 2, 3);
    assert(l.size() == 3 && l[0] == 1 && l[1] == 2 && l[2] == 3);
    l[1] = 42;
    assert(l[1] == 42);

    // front / back
    assert(l.front() == 1 && l.back() == 3);

    // single-element list: front == back
    l.clear();
    l.push_back(42);
    assert(l.front() == l.back());

    // at() and operator[]
    xl::list lst = {10, 20, 30, 40, 50};
    assert(lst.at(0) == 10 && lst.at(2) == 30 && lst.at(4) == 50);
    lst.at(2) = 99;
    assert(lst.at(2) == 99);
    assert(lst[0] == 10 && lst[4] == 50);

    const xl::list clst = {1, 2, 3};
    assert(clst.at(0) == 1 && clst.at(2) == 3);
    assert(clst[0] == 1 && clst[1] == 2 && clst[2] == 3);

    const xl::list cl = {5, 10, 15};
    assert(cl[0] == 5 && cl[1] == 10 && cl[2] == 15);

    // max_size
    xl::list<int> empty;
    assert(empty.max_size() > 0);

    // at() and operator[] agree on all indices
    xl::list al = {3, 6, 9, 12, 15};
    for (std::size_t i = 0; i < al.size(); ++i)
      assert(al.at(i) == al[i]);

    // const list: size, front, back, empty, cbegin/cend
    const xl::list cl2 = {10, 20, 30};
    assert(cl2.size() == 3 && cl2.front() == 10 && cl2.back() == 30 && !cl2.empty());
    int sum = 0;
    for (auto it = cl2.cbegin(); it != cl2.cend(); ++it) sum += *it;
    assert(sum == 60);
  }

  // TC-05  Push, Pop, Emplace, and Modifiers
  {
    // variadic push_back / push_front
    xl::list<int> l;
    l.push_back(1, 2, 3);
    assert(l.size() == 3 && l[0] == 1 && l[1] == 2 && l[2] == 3);

    l.push_front(100);
    assert(l.front() == 100 && l.back() == 3);

    l.clear();
    l.push_back(1, 2, 3);
    l.push_front(0);
    assert(l.size() == 4 && (l == std::array{0, 1, 2, 3}));

    // pop_back / pop_front
    l.clear();
    l.push_back(1, 2, 3);
    l.pop_back();  assert(l.size() == 2 && l.back()  == 2);
    l.pop_front(); assert(l.size() == 1 && l.front() == 2);
    l.pop_back(); assert(l.empty());

    // push_front + push_back in alternation; check structural integrity
    {
      xl::list<int> alt;
      for (int i = 1; i <= 5; ++i) {
        alt.push_back(i);
        alt.push_front(-i);
      }
      assert(alt.size() == 10);
      assert(alt.front() == -5 && alt.back() == 5);
      std::vector<int> fwd2, bwd2;
      for (auto v : alt) fwd2.push_back(v);
      for (auto it2 = alt.rbegin(); it2 != alt.rend(); ++it2) bwd2.push_back(*it2);
      assert(fwd2.size() == bwd2.size());
      for (std::size_t i = 0; i < fwd2.size(); ++i)
        assert(fwd2[i] == bwd2[fwd2.size() - 1 - i]);
    }

    // emplace_back / emplace_front return reference
    xl::list<int> li;
    auto& rb = li.emplace_back(10);  assert(rb == 10 && li.back()  == 10);
    auto& rf = li.emplace_front(5);  assert(rf == 5  && li.front() == 5);

    // emplace returns iterator to new element
    auto it = li.emplace(std::next(li.begin()), 7);
    assert(*it == 7 && li.size() == 3);
    assert((li == std::array{5, 7, 10}));

    // emplace with multi-argument constructor (Person)
    struct Person {
      std::string name; int age;
      Person(std::string n, int a) : name(std::move(n)), age(a) {}
    };
    xl::list<Person> plist;
    plist.emplace_back("Alice", 30);
    plist.emplace_front("Bob", 25);
    auto p2 = plist.begin(); ++p2;
    p2 = plist.emplace(p2, "Charlie", 40);
    assert(plist.size() == 3);
    assert(plist.front().name == "Bob" && plist.back().name == "Alice");
    assert(p2->name == "Charlie" && p2->age == 40);

    // emplace_back / emplace_front with pair (multi-arg constructor)
    xl::list<std::pair<int, int>> pl;
    pl.emplace_back(1, 2); pl.emplace_front(3, 4);
    auto pit = pl.begin(); ++pit;
    pit = pl.emplace(pit, 5, 6);
    assert(pl.size()  == 3);
    assert(pl.front() == std::pair(3,4));
    assert(pl.back()  == std::pair(1,2));
    assert(*pit       == std::pair(5,6));

    // emplace at begin / end / middle with std::string
    xl::list<std::string> sl = {"world"};
    sl.emplace(sl.begin(), "hello");
    sl.emplace(sl.end(),   "!");
    assert(sl.size() == 3 && sl.front() == "hello" && sl.back() == "!");
    auto sit = sl.begin(); ++sit;
    sit = sl.emplace(sit, "there");
    assert(*sit == "there");
    assert((sl == xl::list<std::string>{"hello", "there", "world", "!"}));

    // non-default-constructible type (emplace-only)
    struct NoDef {
      int x;
      explicit NoDef(int v) : x(v) {}
      NoDef(const NoDef&) = delete;
      NoDef(NoDef&&) = default;
      NoDef& operator=(NoDef&&) = default;
    };
    xl::list<NoDef> lst;
    lst.emplace_back(1);
    lst.emplace_back(3);
    lst.emplace_front(0);
    lst.emplace(std::next(lst.begin(), 2), 2); // insert 2 before 3
    assert(lst.size() == 4);
    int expected = 0;
    for (const auto& v : lst) assert(v.x == expected++);
  }

  // TC-06  Insert and Erase
  {
    // insert returning iterator and erase returning iterator
    xl::list<int> l;
    l.push_back(4);
    auto it = l.insert(std::next(l.begin()), 5);
    assert(l.size() == 2 && *it == 5);
    it = l.erase(l.begin());
    assert(l.size() == 1 && *it == 5);

    // insert before second element; push_front
    l.clear();
    l.push_back(1);
    l.push_front(2);
    l.insert(++l.begin(), 3);
    assert(l.size() == 3);
    it = l.begin();
    assert(*it == 2); ++it;
    assert(*it == 3); ++it;
    assert(*it == 1);

    // range-based for, insert at begin/end, reverse iterator
    l.clear();
    l.push_back(1, 2, 3);
    l.insert(l.begin(), 0);
    l.insert(l.end(), 4);
    assert(l.size() == 5);
    l.pop_back();  assert(l.size() == 4);
    l.erase(l.begin()); assert(l.size() == 3);
    {
      xl::list<int>::reverse_iterator rit = l.rbegin();
      assert(*rit == 3); ++rit;
      assert(*rit == 2); ++rit;
      assert(*rit == 1);
    }

    // single insert at begin / end / middle
    xl::list myList = {1, 2, 3, 4, 5};
    myList.insert(myList.begin(), 0);    assert(myList.front() == 0);
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), 6);      assert(myList.back() == 6);
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), 10); assert(myList[2] == 10);

    // ilist at begin / end / middle
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.begin(), {0, 0, 0});
    assert(myList.front() == 0 && myList.size() == 8);
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), {6, 6, 6});
    assert(myList.back() == 6 && myList.size() == 8);
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), {10, 10, 10});
    assert(myList[2] == 10 && myList.size() == 8);

    // insert(pos, count, value) at begin, end, and middle
    xl::list lst = {1, 5};
    lst.insert(lst.begin(), 2, 0);
    assert(lst.size() == 4 && lst.front() == 0);
    lst.insert(lst.end(), 2, 9);
    assert(lst.size() == 6 && lst.back() == 9);
    auto mid = std::next(lst.begin(), 3);
    lst.insert(mid, 3, 4);
    assert(lst.size() == 9);

    // insert(pos, count=0, value) is a no-op
    xl::list lst2 = {1, 2, 3};
    auto it2 = std::next(lst2.begin());
    lst2.insert(it2, 0, 99);
    assert(lst2.size() == 3 && (lst2 == xl::list<int>{1, 2, 3}));

    // insert(pos, count, value) multi-element fill in middle
    xl::list lst3 = {1, 2, 3};
    lst3.insert(std::next(lst3.begin()), 3, 99);
    assert(lst3.size() == 6 && lst3.front() == 1);
    auto it3 = std::next(lst3.begin());
    assert(*it3 == 99); ++it3; assert(*it3 == 99); ++it3;
    assert(*it3 == 99); ++it3; assert(*it3 == 2);

    // insert(pos, iter, iter) iterator-range overload
    xl::list lst4 = {1, 5};
    int mid4[] = {2, 3, 4};
    lst4.insert(std::next(lst4.begin()), std::begin(mid4), std::end(mid4));
    assert(lst4.size() == 5 && std::is_sorted(lst4.begin(), lst4.end()));

    // insert count/value into empty list
    {
      xl::list<int> l_empty;
      l_empty.insert(l_empty.begin(), 5, 42);
      assert(l_empty.size() == 5);
    }

    // insert iterator range at end
    {
      xl::list l = {1,2};
      int arr[] = {3,4,5};
      l.insert(l.end(), std::begin(arr), std::end(arr));
      assert((l == xl::list{1,2,3,4,5}));
    }

    // insert single element into empty list result is a one-element list
    {
      xl::list<int> empty_l;
      auto ins = empty_l.insert(empty_l.end(), 42);
      assert(empty_l.size() == 1 && *ins == 42);
      assert(empty_l.front() == 42 && empty_l.back() == 42);
    }

    // insert(ilist) into empty list
    {
      xl::list<int> empty_l2;
      empty_l2.insert(empty_l2.end(), {1, 2, 3});
      assert((empty_l2 == xl::list<int>{1, 2, 3}));
    }

    // insert(iter,iter) empty range is a no-op
    {
      xl::list<int> ins_noop = {1, 2, 3};
      std::vector<int> empty_src2;
      ins_noop.insert(ins_noop.begin(), empty_src2.begin(), empty_src2.end());
      assert((ins_noop == xl::list<int>{1, 2, 3}));
    }

    // returned iterator from single insert points to the inserted element
    {
      xl::list<int> ri = {10, 30};
      auto ret_it = ri.insert(std::next(ri.begin()), 20);
      assert(*ret_it == 20);
      assert((ri == xl::list<int>{10, 20, 30}));
    }

    // variadic insert via multi_t
    {
      xl::list lst = {1, 5};
      auto pos = std::next(lst.begin()); // points to 5
      lst.insert(xl::multi, pos, 2, 3, 4);
      assert(lst.size() == 5 && std::is_sorted(lst.begin(), lst.end()));
      assert((lst == xl::list{1, 2, 3, 4, 5}));

      // variadic insert at begin
      xl::list vi = {4, 5};
      vi.insert(xl::multi, vi.begin(), 1, 2, 3);
      assert((vi == xl::list{1, 2, 3, 4, 5}));

      // variadic insert at end
      xl::list vi2 = {1, 2};
      vi2.insert(xl::multi, vi2.end(), 3, 4, 5);
      assert((vi2 == xl::list{1, 2, 3, 4, 5}));

      // variadic insert single value (same as regular insert)
      xl::list vi3 = {1, 3};
      vi3.insert(xl::multi, std::next(vi3.begin()), 2);
      assert((vi3 == xl::list{1, 2, 3}));
    }

    // erase middle of three-element list
    {
      xl::list<int> l = {1,2,3};
      auto ret = l.erase(std::next(l.begin()));
      assert(*ret == 3);
      assert((l == xl::list<int>{1,3}));
    }

    // erase range leaving endpoints
    {
      xl::list<int> l = {1,2,3,4,5};
      l.erase(std::next(l.begin()), std::prev(l.end()));
      assert((l == xl::list<int>{1,5}));
    }

    // erase two elements in sequence
    {
      xl::list l{1, 2, 3, 4, 5};
      auto it = l.begin();
      l.erase(std::next(it, 2));
      l.erase(it);
      assert(l.size() == 3 && l.front() == 2 && l.back() == 5);
    }

    // erase(begin, begin) is a no-op (empty range)
    {
      xl::list lst_era = {1, 2, 3};
      auto ret = lst_era.erase(lst_era.begin(), lst_era.begin());
      assert(lst_era.size() == 3 && ret == lst_era.begin());
    }

    // erase range in the middle; return iterator and endpoints correct
    {
      xl::list mid = {1, 2, 3, 4, 5, 6, 7};
      auto first = std::next(mid.begin(), 2); // points to 3
      auto last  = std::next(first, 3);        // points to 6
      auto ret = mid.erase(first, last);
      assert(mid.size() == 4 && mid.front() == 1 && mid.back() == 7 && *ret == 6);
    }

    // erase range spanning entire list
    {
      xl::list all = {1, 2, 3, 4, 5};
      auto ret = all.erase(all.begin(), all.end());
      assert(all.empty() && ret == all.end());
    }

    // erase first element; new front is correct
    {
      xl::list ef = {1, 2, 3, 4, 5};
      ef.erase(ef.begin());
      assert(ef.size() == 4 && ef.front() == 2);
    }

    // erase last element; new back is correct
    {
      xl::list el = {1, 2, 3, 4, 5};
      el.erase(std::prev(el.end()));
      assert(el.size() == 4 && el.back() == 4);
    }

    // returned iterator from range erase points to element after the range
    {
      xl::list re = {1, 2, 3, 4, 5};
      auto b2 = std::next(re.begin());   // 2
      auto e2 = std::next(b2, 2);        // 4
      auto r2 = re.erase(b2, e2);
      assert(*r2 == 4 && re.size() == 3);
      assert((re == xl::list<int>{1, 4, 5}));
    }

    // erase last element via iterator, then push_back, verify sequence
    {
      xl::list el = {1, 2, 3};
      el.erase(std::prev(el.end()));
      assert(el.size() == 2 && el.back() == 2);
      el.push_back(99);
      assert(el.size() == 3 && el.back() == 99);
      int exp = 0; int vals[] = {1, 2, 99};
      for (auto v : el) assert(v == vals[exp++]);
    }

    // clear
    l.clear(); assert(l.empty() && l.size() == 0);
  }

  // TC-07  Iterators, Sentinels, and Traversal
  {
    // sequential forward walk
    {
      xl::list<int> l = {0, 1, 2, 3, 4};
      int expected = 0;
      for (auto v : l) assert(v == expected++);
    }

    // bidirectional traversal: forward and backward yield mirror
    xl::list lbidi = {10, 20, 30, 40, 50};
    std::vector<int> fwd, bwd;
    for (auto it = lbidi.begin();  it != lbidi.end();  ++it) fwd.push_back(*it);
    for (auto it = lbidi.rbegin(); it != lbidi.rend(); ++it) bwd.push_back(*it);
    for (std::size_t i = 0; i < fwd.size(); ++i)
      assert(fwd[i] == bwd[fwd.size() - 1 - i]);

    // reverse_iterator full traversal on const list
    const xl::list clst2 = {1, 2, 3, 4, 5};
    std::vector<int> rev;
    for (auto it = clst2.crbegin(); it != clst2.crend(); ++it)
      rev.push_back(*it);
    assert((rev == std::vector<int>{5, 4, 3, 2, 1}));

    // reverse iterators accumulate digits (54321)
    xl::list rdig = {1, 2, 3, 4, 5};
    int rsum = 0;
    for (auto ri = rdig.rbegin(); ri != rdig.rend(); ++ri)
      rsum = rsum * 10 + *ri;
    assert(rsum == 54321);

    // std::distance between const_iterators equals size
    const xl::list clst = {1, 2, 3, 4, 5};
    assert(std::distance(clst.cbegin(), clst.cend()) == (std::ptrdiff_t)clst.size());

    // cbegin / cend on non-const list
    xl::list ncl = {10, 20, 30};
    int sum = 0;
    for (auto it = ncl.cbegin(); it != ncl.cend(); ++it) sum += *it;
    assert(sum == 60);
    assert(ncl.cbegin() == ncl.begin() && ncl.cend() == ncl.end());

    // iterator arithmetic: decrement from end, std::distance
    xl::list arith = {1, 2, 3, 4, 5};
    auto end_it = arith.end(); --end_it;
    assert(*end_it == 5);
    ++end_it; assert(end_it == arith.end());
    assert(std::distance(arith.begin(), arith.end()) == 5);

    // std::advance forward and backward; make_reverse_iterator
    xl::list adv = {1, 2, 3, 4, 5};
    auto ait = adv.begin();
    std::advance(ait,  3); assert(*ait == 4);
    std::advance(ait, -2); assert(*ait == 2);
    auto rit = std::make_reverse_iterator(ait);
    assert(*rit == 1);
    std::advance(rit, -2); assert(*rit == 3);

    // after_begin / before_end / rafter_begin / rbefore_end
    xl::list lst = {1, 2, 3, 4, 5};
    assert(*lst.after_begin() == 2);
    assert(*lst.before_end()  == 5);

    // cafter_begin / cbefore_end on const
    const xl::list clst3 = {10, 20, 30};
    assert(*clst3.cafter_begin() == 20);
    assert(*clst3.cbefore_end()  == 30);

    // rafter_begin / rbefore_end: reverse sentinels
    auto rab = lst.rafter_begin(); // wraps before_end; derefs to second-from-last (4)
    assert(*rab == 4);
    auto rbe = lst.rbefore_end();  // wraps after_begin; derefs to first element (1)
    assert(*rbe == 1);

    // after_begin on two-element list is back()
    {
      xl::list two = {10, 20};
      assert(*two.after_begin() == 20);
      assert(*two.before_end()  == 20);
    }

    // insert at end does not invalidate existing iterators
    {
      xl::list l{1, 2, 3};
      auto itm = l.begin(); ++itm;       // points to 2
      l.insert(l.end(), 4);
      assert(l.size() == 4 && *itm == 2);
    }

    // erase does not invalidate other iterators
    {
      xl::list lst = {1, 2, 3, 4, 5};
      auto it1 = lst.begin(); ++it1;
      auto it2 = std::next(it1);
      lst.erase(it1);
      assert(*it2 == 3);
      auto it3 = lst.begin();
      lst.insert(std::next(it3), 10);
      assert(*it3 == 1 && *it2 == 3);
    }

    // iterator validity around erase, insert, splice
    {
      xl::list lst2 = {1, 2, 3, 4, 5};
      auto ia = lst2.begin();
      auto ib = std::next(ia);
      auto ic = std::next(ib);
      ic = lst2.erase(ib);
      assert(*ia == 1 && *ic == 3);
      auto id = lst2.insert(ic, 10);
      assert(*ic == 3 && *id == 10);
      xl::list<int> lst3 = {20, 30};
      lst2.splice(ic = std::next(id), lst3);
      assert(*ic == 3 && lst2.size() == 7);
    }

    // iterator validity after erase of middle element (saved iterators)
    {
      xl::list lstv = {1, 2, 3, 4, 5};
      std::vector<decltype(lstv.begin())> iters;
      for (auto it = lstv.begin(); it != lstv.end(); ++it) iters.push_back(it);
      lstv.erase(std::next(lstv.begin(), 2));
      assert(*iters[0] == 1 && *iters[1] == 2);
      assert(*iters[3] == 4 && *iters[4] == 5);
    }

    // iterator dereference via arrow operator on list of pairs
    {
      xl::list<std::pair<int, std::string>> pl2 = {{1,"a"},{2,"b"},{3,"c"}};
      auto pit2 = pl2.begin();
      assert(pit2->first == 1 && pit2->second == "a");
      ++pit2;
      assert(pit2->first == 2 && pit2->second == "b");
    }

    // post-increment and post-decrement return old value
    {
      xl::list pid = {10, 20, 30};
      auto it_a = pid.begin();
      auto it_b = it_a++;           // it_b == begin (10), it_a now points to 20
      assert(*it_b == 10 && *it_a == 20);
      auto it_c = it_a--;           // it_c == 20, it_a back to 10
      assert(*it_c == 20 && *it_a == 10);
    }

    // const_iterator constructed from iterator (implicit conversion)
    {
      xl::list conv_lst = {1, 2, 3};
      xl::list<int>::iterator mut_it = conv_lst.begin();
      xl::list<int>::const_iterator const_it = mut_it;
      assert(*const_it == *mut_it);
      ++const_it; ++mut_it;
      assert(*const_it == *mut_it);
    }

    // iterator bool conversion: valid iterator is truthy, end is falsy
    {
      xl::list ib = {1, 2};
      auto it_ib = ib.begin();
      assert(static_cast<bool>(it_ib));   // valid node
      it_ib = ib.end();
      assert(!static_cast<bool>(it_ib));  // end sentinel
    }

    // clear then re-populate and verify fresh iterators work correctly
    {
      xl::list lst = {1, 2, 3};
      lst.clear();
      assert(lst.empty());
      lst.push_back(7, 8, 9);
      assert(lst.size() == 3 && lst.front() == 7 && lst.back() == 9);
      int chk = 7;
      for (auto v : lst) assert(v == chk++);
    }
  }

  // TC-08  List Operations: sort, merge, splice, reverse, unique, remove, resize, iter_swap
  {
    // --- resize ---
    {
      xl::list l = {1, 2, 3, 4, 5};
      l.resize(3); assert(l.size() == 3);
      l.resize(3); assert(l.size() == 3);   // same size is a no-op
      l.resize(5); assert(l.size() == 5);
      l.resize(8, 10); assert(l.size() == 8);

      // grow with default value produces 0s
      xl::list<int> l2;
      l2.push_back(1); l2.push_back(2);
      l2.resize(3);
      assert(l2.size() == 3 && l2.front() == 1 && l2.back() == 0);
      assert(l2.at(1) == 2);

      // grow with explicit value
      xl::list l3 = {1, 2, 3};
      l3.resize(5, 100);
      assert(l3.size() == 5 && l3.back() == 100);

      // resize to 0
      xl::list l4 = {1, 2, 3, 4, 5};
      l4.resize(0);
      assert(l4.empty() && !l4.size());

      // resize empty list to non-zero
      {
        xl::list<int> re;
        re.resize(4, 7);
        assert(re.size() == 4);
        for (auto v : re) assert(v == 7);
      }

      // resize(1) on a list of 3 only front survives
      {
        xl::list r1 = {10, 20, 30};
        r1.resize(1);
        assert(r1.size() == 1 && r1.front() == 10 && r1.back() == 10);
      }

      // consecutive shrink–grow cycle preserves final size
      {
        xl::list rsg(10, 5);
        rsg.resize(3);
        rsg.resize(8, 9);
        assert(rsg.size() == 8);
        auto it_rsg = rsg.begin();
        for (int i = 0; i < 3; ++i, ++it_rsg) assert(*it_rsg == 5);
        for (int i = 0; i < 5; ++i, ++it_rsg) assert(*it_rsg == 9);
      }
    }

    // --- reverse ---
    {
      xl::list lst{1, 2, 3, 4, 5};
      lst.reverse();
      assert((lst == std::array{5, 4, 3, 2, 1}));

      // std::ranges::reverse
      xl::list rng{1, 2, 3, 4, 5};
      std::ranges::reverse(rng);
      assert((rng == std::array{5, 4, 3, 2, 1}));

      // double reverse is identity
      xl::list dbl{1, 2, 3, 4, 5};
      dbl.reverse(); dbl.reverse();
      assert((dbl == std::array{1, 2, 3, 4, 5}));

      // reverse empty list
      xl::list<int> empty; empty.reverse(); assert(empty.empty());

      // reverse single-element list
      xl::list<int> one = {42}; one.reverse();
      assert(one.size() == 1 && one.front() == 42);

      // reverse two-element list
      {
        xl::list two = {1, 2};
        two.reverse();
        assert(two.front() == 2 && two.back() == 1);
      }

      // reverse then iterate: verify all original elements present in opposite order
      {
        xl::list orig = {10, 20, 30, 40, 50};
        std::vector before(orig.begin(), orig.end());
        orig.reverse();
        std::vector after(orig.begin(), orig.end());
        for (std::size_t i = 0; i < before.size(); ++i)
          assert(before[i] == after[before.size() - 1 - i]);
      }
    }

    // --- sort ---
    {
      // sort empty list is a no-op
      xl::list<int> empty; empty.sort();

      // sort single-element list
      xl::list<int> single = {5}; single.sort();
      assert(single.size() == 1 && single.front() == 5);

      // two-element list (ascending and descending input)
      xl::list<int> two_asc = {1, 2}; two_asc.sort();
      assert((two_asc == xl::list<int>{1, 2}));
      xl::list<int> two_desc = {2, 1}; two_desc.sort();
      assert((two_desc == xl::list<int>{1, 2}));

      // basic ascending sort
      xl::list lst = {5, 2, 8, 1, 9};
      assert(!std::is_sorted(lst.begin(), lst.end()));
      lst.sort();
      assert(std::is_sorted(lst.begin(), lst.end()));

      // custom descending comparator
      xl::list desc = {3, 1, 4, 2, 5};
      desc.sort(std::greater<int>());
      assert((desc == xl::list<int>{5, 4, 3, 2, 1}));

      // custom descending comparator lambda
      xl::list dlam = {3, 1, 4, 2, 5};
      dlam.sort([](int a, int b){ return a > b; });
      assert((dlam == xl::list<int>{5, 4, 3, 2, 1}));

      // chained: sort + unique + reverse
      xl::list chain = {5, 1, 3, 1, 4, 2, 5, 3};
      chain.sort(); chain.unique(); chain.reverse();
      assert((chain == std::array{5, 4, 3, 2, 1}));

      // reverse-sorted 10 000 elements
      {
        xl::list<int> big;
        const int N = 10000;
        for (int i = N; i > 0; --i) big.push_back(i);
        big.sort();
        assert(big.front() == 1 && big.back() == N);
        int expected = 1;
        for (const auto& v : big) assert(v == expected++);
      }

      // case-insensitive string sort
      {
        auto ci = [](const std::string& a, const std::string& b) {
          return std::lexicographical_compare(
            a.begin(), a.end(), b.begin(), b.end(),
            [](char c1, char c2){ return std::tolower(c1) < std::tolower(c2); });
        };
        xl::list<std::string> words = {"Banana", "apple", "Carrot", "date"};
        words.sort(ci);
        assert((words == xl::list<std::string>{"apple", "Banana", "Carrot", "date"}));
      }

      // stateful comparator (partition by threshold)
      {
        struct StatefulComparator {
          int threshold;
          bool operator()(int a, int b) const {
            return (a > threshold) < (b > threshold);
          }
        };
        xl::list sfc = {10, 2, 8, 1, 6, 12};
        sfc.sort(StatefulComparator{5});
        auto it = sfc.begin();
        while (it != sfc.end() && *it <= 5) ++it;
        while (it != sfc.end()) { assert(*it > 5); ++it; }
      }

      // list of pairs: sort by second field, then by first
      {
        xl::list<std::pair<int,int>> pairs = {{3,1},{1,3},{2,2},{1,1},{2,1}};
        pairs.sort([](const auto& a, const auto& b){
          return a.second != b.second ? a.second < b.second : a.first < b.first;
        });
        assert(std::is_sorted(pairs.begin(), pairs.end(),
          [](const auto& a, const auto& b){
            return a.second != b.second ? a.second < b.second : a.first < b.first;
          }));
      }

      // sort sub-range via iterator pair (middle three only)
      {
        xl::list sr = {5, 3, 1, 4, 2};
        auto b = std::next(sr.begin());
        auto e = std::next(b, 3);
        sr.sort(b, e);
        assert(sr.front() == 5 && sr.back() == 2);
        int prev = std::numeric_limits<int>::min();
        auto it = std::next(sr.begin()); int cnt = 0;
        while (it != std::prev(sr.end())) { assert(*it >= prev); prev = *it; ++it; ++cnt; }
        assert(cnt == 3);
      }

      // sort variants I=0..4 all produce same result on random and on already-/reverse-sorted input
      {
        auto make = []() -> xl::list<int> { return {5,3,1,4,2}; };
        auto ref = make(); ref.sort();
        auto l1 = make(); l1.sort<1>();
        auto l2 = make(); l2.sort<2>();
        auto l3 = make(); l3.sort<3>();
        auto l4 = make(); l4.sort<4>();
        assert(l1 == ref && l2 == ref && l3 == ref && l4 == ref);

        auto sorted_asc  = []{ return xl::list{1,2,3,4,5,6,7,8}; };
        auto sorted_desc = []{ return xl::list{8,7,6,5,4,3,2,1}; };
        for (auto& make2 : std::initializer_list<std::function<xl::list<int>()>>{sorted_asc, sorted_desc}) {
          auto s0 = make2(); s0.sort<0>();
          auto s1 = make2(); s1.sort<1>();
          auto s2 = make2(); s2.sort<2>();
          auto s3 = make2(); s3.sort<3>();
          auto s4 = make2(); s4.sort<4>();
          assert(std::is_sorted(s0.begin(), s0.end()));
          assert(s0 == s1 && s1 == s2 && s2 == s3 && s3 == s4);
        }
      }

      // sort stability across all five variants
      {
        struct Item { int key, order; };
        auto check_stable = [](xl::list<Item> l) {
          l.sort([](const Item& a, const Item& b){ return a.key < b.key; });
          auto it = l.begin();
          int last_key = -1, last_order_for_key = -1;
          for (; it != l.end(); ++it) {
            if (it->key == last_key) { assert(it->order > last_order_for_key); }
            last_order_for_key = it->order;
            last_key = it->key;
          }
        };
        xl::list<Item> src;
        for (int i = 0; i < 20; ++i) src.push_back({i % 5, i});
        xl::list<Item> l0 = src; check_stable(l0);
        xl::list<Item> l1 = src; { l1.sort<1>([](const Item& a, const Item& b){ return a.key < b.key; }); }
        xl::list<Item> l2 = src; { l2.sort<2>([](const Item& a, const Item& b){ return a.key < b.key; }); }
        xl::list<Item> l3 = src; { l3.sort<3>([](const Item& a, const Item& b){ return a.key < b.key; }); }
        xl::list<Item> l4 = src; { l4.sort<4>([](const Item& a, const Item& b){ return a.key < b.key; }); }
      }

      // xl::sort free-function wrapper
      {
        xl::list lst2 = {4, 2, 5, 1, 3};
        auto b2 = lst2.cbegin(), e2 = lst2.cend();
        xl::sort(lst2, b2, e2);
        assert(std::is_sorted(lst2.begin(), lst2.end()));
      }

      // std::list parity check
      {
        xl::list xl_lst = {9, 7, 5, 3, 1, 2, 4, 6, 8};
        std::list std_lst(xl_lst.begin(), xl_lst.end());
        xl_lst.sort(); std_lst.sort();
        assert(std_lst == xl_lst);
      }

      // sort already-sorted list is idempotent
      {
        xl::list asc = {1, 2, 3, 4, 5};
        asc.sort();
        assert((asc == xl::list<int>{1, 2, 3, 4, 5}));
      }

      // sort list of duplicates produces stable equal block
      {
        xl::list<int> dups(8, 3);
        dups.sort();
        for (auto v : dups) assert(v == 3);
        assert(dups.size() == 8);
      }

      // sort all-same list with all five variants; each still has N elements
      {
        for (int variant = 0; variant < 5; ++variant) {
          xl::list<int> s(7, 42);
          auto do_sort = [&](int v) {
            if (v == 0) s.sort<0>();
            else if (v == 1) s.sort<1>();
            else if (v == 2) s.sort<2>();
            else if (v == 3) s.sort<3>();
            else             s.sort<4>();
          };
          do_sort(variant);
          assert(s.size() == 7);
          for (auto val : s) assert(val == 42);
        }
      }

      // sort uses pseudorandom input of size > bsize0 (exercises run-merging path)
      {
        std::mt19937 rng(42);
        xl::list<int> rand_lst;
        for (int i = 0; i < 200; ++i)
          rand_lst.push_back(static_cast<int>(rng() % 1000));
        rand_lst.sort();
        assert(std::is_sorted(rand_lst.begin(), rand_lst.end()));
        assert(rand_lst.size() == 200);
      }

      // xl::sort partial range (begin to before_end): last element excluded from sort
      { xl::list z = {3, 2, 1}; xl::sort(z, z.begin(), z.before_end());   assert((std::array{2, 3, 1} == z)); }
      { xl::list z = {3, 2, 1}; xl::sort<1>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
      { xl::list z = {3, 2, 1}; xl::sort<2>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
      { xl::list z = {3, 2, 1}; xl::sort<3>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
      { xl::list z = {3, 2, 1}; xl::sort<4>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }

      // sort<0..4> on empty list: all variants must be no-ops
      { xl::list<int> z; z.sort<0>(); z.sort<1>(); z.sort<2>(); z.sort<3>(); z.sort<4>(); assert(z.empty()); }

      // stable sort: Person objects with equal id retain relative name order
      {
        struct Person {
          int id;
          std::string name;
          bool operator<(const Person& o) const { return id < o.id || (id == o.id && name < o.name); }
        };
        xl::list<Person> p = {{2, "B"}, {1, "A"}, {2, "C"}};
        p.sort();
        assert(p.front().name == "A" && p.back().name == "C");
      }
    }

    // --- merge ---
    {
      // non-interleaved; resize after merge
      {
        xl::list a = {1, 2, 3, 4, 5}, b = {6, 7, 8, 9, 10};
        a.merge(std::move(b));
        assert(a.size() == 10 && a.front() == 1 && a.back() == 10);
        a.resize(5);
        assert(a.size() == 5 && a.front() == 1 && a.back() == 5);
      }
      // both empty
      { xl::list<int> a, b; a.merge(b); assert(a.empty() && b.empty()); }

      // empty into non-empty (no-op for source)
      {
        xl::list<int> a = {1, 2, 3}, b;
        a.merge(b);
        assert((a == std::array{1, 2, 3}) && b.empty());
      }
      // non-empty into empty
      {
        xl::list<int> a, b = {4, 5, 6};
        a.merge(b);
        assert((a == std::array{4, 5, 6}) && b.empty());
      }
      // interleaved equal-sized
      {
        xl::list a = {1, 3, 5}, b = {2, 4, 6};
        a.merge(b);
        assert((a == std::array{1, 2, 3, 4, 5, 6}) && b.empty());
      }
      // different sizes
      {
        xl::list a = {1, 3, 5}, b = {2, 4};
        a.merge(b);
        assert((a == std::initializer_list{1, 2, 3, 4, 5}) && b.empty());
      }
      // duplicates interleaved
      {
        xl::list a = {1, 2, 3}, b = {2, 3, 4};
        a.merge(b);
        assert((a == std::vector{1, 2, 2, 3, 3, 4}) && b.empty());
      }
      // partially overlapping ranges
      {
        xl::list a = {1, 3, 5}, b = {2, 3, 4, 6};
        a.merge(b);
        assert((a == std::array{1, 2, 3, 3, 4, 5, 6}) && b.empty());
      }
      // shared values (stable ordering check via size/endpoints)
      {
        xl::list a = {1, 2, 2, 3}, b = {2, 4, 5};
        a.merge(b);
        assert(a.size() == 7 && a.front() == 1 && a.back() == 5);
      }
      // move-merge same-sized lists; check full element order
      {
        xl::list a = {1, 2}, b = {1, 2};
        a.merge(std::move(b));
        assert(a.size() == 4 && a.front() == 1 && a.back() == 2);
        assert(a.at(1) == 1 && a.at(2) == 2);
      }
      // descending comparator (std::greater)
      {
        xl::list a = {5,3,1}, b = {6,4,2};
        a.merge(b, std::greater<int>());
        assert((a == std::array{6,5,4,3,2,1}) && b.empty());
      }
      // custom descending lambda
      {
        xl::list a = {5, 3, 1}, b = {6, 4, 2};
        a.merge(b, [](int x, int y){ return x > y; });
        assert((a == std::array{6, 5, 4, 3, 2, 1}) && b.empty());
      }
      // merge empty into sorted list with custom comparator (no-op)
      {
        xl::list<int> a = {5, 3, 1}, b;
        a.merge(b, [](int x, int y){ return x > y; });
        assert((a == std::array{5, 3, 1}) && b.empty());
      }
      // custom-comparable struct
      {
        struct Value {
          int priority, id;
          bool operator<(const Value& o) const { return priority < o.priority; }
        };
        xl::list<Value> a = {{1,101},{3,103},{5,105}};
        xl::list<Value> b = {{2,202},{4,204},{6,206}};
        a.merge(b);
        int last = 0;
        for (const auto& v : a) { assert(v.priority > last); last = v.priority; }
        assert(b.empty());
      }
      // chained merge (three lists)
      {
        xl::list a = {1, 4, 7}, b = {2, 5, 8}, c = {3, 6, 9};
        a.merge(b); a.merge(c);
        assert(a.size() == 9 && std::is_sorted(a.begin(), a.end()));
        int expected = 1;
        for (const auto& v : a) assert(v == expected++);
      }

      // merge of two single-element lists (both orderings)
      {
        xl::list a1 = {1}, b1 = {2};
        a1.merge(b1);
        assert((a1 == xl::list<int>{1, 2}) && b1.empty());

        xl::list a2 = {2}, b2 = {1};
        a2.merge(b2);
        assert((a2 == xl::list<int>{1, 2}) && b2.empty());
      }

      // result of merge is fully sorted (std::is_sorted check)
      {
        xl::list a = {2, 5, 8, 11}, b = {1, 3, 7, 9, 13};
        a.merge(b);
        assert(std::is_sorted(a.begin(), a.end()));
        assert(a.size() == 9 && b.empty());
      }

      // merge preserves stability (equal elements from *this come before those from other)
      {
        struct Labeled { int val; char src; bool operator<(const Labeled& o) const { return val < o.val; } };
        xl::list<Labeled> a2 = {{1,'a'},{3,'a'},{5,'a'}}, b2 = {{2,'b'},{3,'b'},{4,'b'}};
        a2.merge(b2);
        assert(b2.empty());
        auto it = std::find_if(a2.begin(), a2.end(), [](const Labeled& l){ return l.val == 3; });
        assert(it != a2.end() && it->src == 'a');
      }
      // heavy interleaved duplicates: both lists contain multiple copies of 3
      {
        xl::list A = {1, 3, 3, 7};
        xl::list B = {2, 3, 3, 8};
        A.sort(); B.sort();
        A.merge(B);
        assert(B.empty());
        assert((A == std::array{1, 2, 3, 3, 3, 3, 7, 8}));
      }

      // merge with custom Person comparator by id
      {
        struct Person {
          int id;
          std::string name;
        };
        xl::list<Person> alpha = {{1, "A"}, {4, "D"}};
        xl::list<Person> beta  = {{2, "B"}, {3, "C"}};
        auto cmp = [](const Person& a, const Person& b){ return a.id < b.id; };
        alpha.sort(cmp);
        beta.sort(cmp);
        alpha.merge(beta, cmp);
        assert(beta.empty());
        assert(alpha.size() == 4);
        int idx = 1;
        for (const auto& p : alpha) assert(p.id == idx++);
      }
    }

    // --- splice ---
    {
      // whole-list splice at beginning
      {
        xl::list a = {1, 2, 3, 4, 5};
        xl::list b = {10, 20, 30};
        a.splice(a.begin(), std::move(b));
        assert((a == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}) && b.empty());
      }
      // empty-list splice is a no-op
      {
        xl::list<int> a = {1, 2, 3}, b;
        a.splice(a.begin(), b);
        assert(a.size() == 3 && b.empty());
      }
      // splice from empty source into non-empty (range overload)
      {
        xl::list dst = {1, 2, 3};
        xl::list<int> src;
        dst.splice(dst.begin(), src, src.begin(), src.end());
        assert(dst.size() == 3 && src.empty());
      }
      // single-element splice to front of a
      {
        xl::list a = {1, 2, 3, 4, 5}, b = {6, 7, 8, 9, 10};
        auto it = b.begin(); std::advance(it, 2);
        a.splice(a.begin(), b, it);
        assert(a.size() == 6 && b.size() == 4 && a.front() == 8);
      }
      // single-element splice to end of destination
      {
        xl::list a = {1, 2, 3};
        xl::list b = {4, 5, 6};
        a.splice(a.end(), b, b.begin());
        assert(a.size() == 4 && a.back() == 4);
        assert(b.size() == 2 && b.front() == 5);
      }
      // range splice
      {
        xl::list a = {1, 2, 3, 4, 5}, b = {6, 7, 8, 9, 10};
        auto it1 = b.begin(), it2 = it1;
        std::advance(it2, 3);
        a.splice(a.begin(), b, it1, it2);
        assert(a.size() == 8 && b.size() == 2 && a.front() == 6);
      }
      // splice empty range is a no-op
      {
        xl::list a = {1, 2, 3}, b = {4, 5, 6};
        a.splice(a.end(), b, b.end(), b.end());
        assert(a.size() == 3 && b.size() == 3);
      }
      // self-splice: move suffix to front
      {
        xl::list lst = {1, 2, 3, 4, 5};
        auto it = lst.begin(); std::advance(it, 2);
        lst.splice(lst.begin(), lst, it, lst.end());
        assert((lst == xl::list<int>{3, 4, 5, 1, 2}));
      }
      // self-splice: identity cases
      {
        xl::list lst = {1, 2, 3, 4, 5};
        lst.splice(lst.begin(), lst, lst.begin());
        assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
        lst.splice(std::next(lst.begin()), lst, lst.begin());
        assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
        lst.splice(lst.end(), lst, lst.begin(), lst.end());
        assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
      }
      // self-splice: move single element within list
      {
        xl::list lst = {1, 2, 4, 5, 3};
        auto it = lst.begin(); std::advance(it, 2);
        lst.splice(it, lst, std::prev(lst.end()));
        assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
        lst.splice(lst.begin(), lst, std::prev(lst.end()));
        assert((lst == xl::list<int>{5, 1, 2, 3, 4}));
      }
      // rotate via splice (forward and back)
      {
        xl::list lst = {1, 2, 3, 4, 5};
        auto it = std::next(lst.begin(), 2);
        lst.splice(lst.end(), lst, lst.begin(), it);
        assert((lst == xl::list<int>{3, 4, 5, 1, 2}));
        it = std::next(lst.begin(), 3);
        lst.splice(lst.begin(), lst, it, lst.end());
        assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
      }
      // splice then sort
      {
        xl::list a = {3, 1, 4}, b = {1, 5, 9, 2, 6};
        a.splice(a.end(), b);
        assert(b.empty() && a.size() == 8);
        a.sort();
        assert(std::is_sorted(a.begin(), a.end()));
      }

      // splice entire list into empty destination
      {
        xl::list<int> dst;
        xl::list src = {1, 2, 3, 4, 5};
        dst.splice(dst.end(), src);
        assert(src.empty());
        assert((dst == xl::list<int>{1, 2, 3, 4, 5}));
      }

      // splice single element from two-element list, verify source integrity
      {
        xl::list s2 = {10, 20};
        xl::list<int> d2;
        d2.splice(d2.end(), s2, s2.begin()); // move 10 out
        assert(s2.size() == 1 && s2.front() == 20 && s2.back() == 20);
        assert(d2.size() == 1 && d2.front() == 10);
      }

      // splice range then iterate both lists for consistency
      {
        xl::list src3 = {1, 2, 3, 4, 5};
        xl::list dst3 = {10, 20};
        auto s_mid = std::next(src3.begin(), 1); // 2
        auto s_end = std::next(s_mid, 3);        // past 4
        dst3.splice(dst3.end(), src3, s_mid, s_end); // move {2,3,4}
        assert(src3.size() == 2 && dst3.size() == 5);
        assert((src3 == xl::list<int>{1, 5}));
        assert((dst3 == xl::list<int>{10, 20, 2, 3, 4}));
      }
    }

    // --- unique ---
    {
      // empty list
      xl::list<int> ml; assert(ml.unique() == 0);

      // all-dup, no-dup, mixed
      ml = {1, 1, 1};          assert(ml.unique() == 2);
      ml = {1, 2, 3};          assert(!ml.unique());
      ml = {1, 1, 2, 2, 3};    assert(ml.unique() == 2);
      ml = {1, 1, 2, 2, 3, 4}; assert(ml.unique() == 2);
      ml = {1, 2, 1, 1, 3, 3, 3, 4, 5, 4}; assert(ml.unique() == 3);

      // single-element list
      xl::list one = {42}; assert(one.unique() == 0 && one.size() == 1);

      // two-element: same / different
      xl::list same = {7, 7}; assert(same.unique() == 1 && same.size() == 1);
      xl::list diff = {3, 4}; assert(diff.unique() == 0 && diff.size() == 2);

      // unique after sort removes non-consecutive duplicates
      xl::list su = {1, 2, 1, 3, 3, 2, 4};
      su.sort(); su.unique();
      assert((su == std::array{1, 2, 3, 4}));

      // default predicate: removes adjacent equal elements
      xl::list def = {1, 2, 2, 3, 4, 4, 4, 5};
      auto orig_sz = def.size();
      auto removed = def.unique();
      assert(removed > 0 && def.size() < orig_sz);
      assert((def == std::array{1, 2, 3, 4, 5}));

      // custom predicate: collapse elements within distance 1
      xl::list cp = {1, 2, 4, 5, 7, 8};
      cp.unique([](int a, int b){ return std::abs(a - b) <= 1; });
      assert(cp.size() < 6);

      // binary predicate on sorted list of strings (case-insensitive)
      xl::list<std::string> sl = {"a", "A", "b", "B", "c"};
      sl.sort([](const std::string& x, const std::string& y){
        return std::tolower(x[0]) < std::tolower(y[0]); });
      sl.unique([](const std::string& x, const std::string& y){
        return std::tolower(x[0]) == std::tolower(y[0]); });
      assert(sl.size() == 3);

      // unique on list of all identical elements → single survivor
      {
        xl::list all_same(10, 5);
        all_same.unique();
        assert(all_same.size() == 1 && all_same.front() == 5);
      }

      // unique does not reorder elements (first of each run survives)
      {
        xl::list ordered = {3, 3, 1, 1, 2, 2};
        ordered.unique();
        assert((ordered == xl::list<int>{3, 1, 2}));
      }

      // unique returns correct count on longer mixed sequence
      {
        xl::list mixed = {1, 1, 2, 3, 3, 3, 4, 4, 5};
        auto n = mixed.unique();
        assert(n == 4);
        assert((mixed == xl::list<int>{1, 2, 3, 4, 5}));
      }

      // unique on empty list with custom predicate (no crash)
      {
        xl::list<int> empty_u;
        assert(empty_u.unique([](int,int){ return true; }) == 0);
      }
    }

    // --- remove / remove_if ---
    {
      // remove non-existing value is a no-op
      xl::list lst = {1, 2, 3};
      lst.remove(42); assert(lst.size() == 3);

      // remove returns count of removed elements
      xl::list lr = {1, 2, 3, 2, 4, 2};
      auto n = lr.remove(2);
      assert(n == 3 && lr.size() == 3 && (lr == xl::list<int>{1, 3, 4}));
      auto m = lr.remove(99);
      assert(m == 0 && lr.size() == 3);

      // remove_if: keep only odd numbers
      xl::list lodd = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      lodd.remove_if([](int x){ return x % 2 == 0; });
      assert(lodd.size() == 5);
      for (const auto& v : lodd) assert(v % 2 == 1);

      // remove_if returns count of removed elements
      xl::list lrf = {1, 2, 3, 4, 5, 6};
      n = lrf.remove_if([](int x){ return x % 2 == 0; });
      assert(n == 3 && lrf.size() == 3);
      m = lrf.remove_if([](int){ return false; });
      assert(m == 0);

      // remove all elements matching predicate; list becomes empty
      xl::list lall = {2, 4, 6, 8, 10};
      lall.remove_if([](int x){ return x % 2 == 0; });
      assert(lall.empty());

      // remove from single-element list when value matches → empty
      {
        xl::list one = {42};
        auto cnt = one.remove(42);
        assert(cnt == 1 && one.empty());
      }

      // remove from single-element list when value doesn't match → unchanged
      {
        xl::list one2 = {42};
        auto cnt = one2.remove(99);
        assert(cnt == 0 && one2.size() == 1 && one2.front() == 42);
      }

      // remove_if leaving only one element
      {
        xl::list rif = {1, 2, 3, 4, 5};
        rif.remove_if([](int x){ return x != 3; });
        assert(rif.size() == 1 && rif.front() == 3 && rif.back() == 3);
      }

      // remove first and last simultaneously
      {
        xl::list<int> rfl = {9, 1, 2, 3, 9};
        rfl.remove(9);
        assert((rfl == xl::list{1, 2, 3}));
      }

      // remove_if all elements: list becomes empty
      {
        xl::list<int> z = {1, 1, 1};
        z.remove_if([](int){ return true; });
        assert(z.empty());
      }

      // remove from empty list (no crash)
      {
        xl::list<int> empty_r;
        assert(empty_r.remove(42) == 0);
        assert(empty_r.remove_if([](int){ return true; }) == 0);
      }
    }

    // --- iter_swap ---
    {
      // adjacent elements
      xl::list lst = {1, 2, 3, 4, 5};
      auto it1 = lst.begin();
      auto it2 = std::next(it1);
      lst.iter_swap(it1, it2);
      assert((lst == xl::list<int>{2, 1, 3, 4, 5}));

      // non-adjacent: first and last
      auto first = lst.begin();
      auto last  = std::prev(lst.end());
      lst.iter_swap(first, last);
      assert(lst.front() == 5 && lst.back() == 2);

      // same iterator (no-op)
      auto mid = std::next(lst.begin(), 2);
      int val_before = *mid;
      lst.iter_swap(mid, mid);
      assert(*mid == val_before);

      // iter_swap on two-element list swaps front and back
      {
        xl::list two = {10, 20};
        two.iter_swap(two.begin(), std::prev(two.end()));
        assert(two.front() == 20 && two.back() == 10);
      }

      // iter_swap: values are exchanged, not node positions (iterator still points to same node)
      {
        xl::list sw = {1, 2, 3};
        auto ia = sw.begin();
        auto ib = std::next(ia, 2);
        sw.iter_swap(ia, ib);
        assert(*ia == 3 && *ib == 1);
        assert(sw.front() == 3 && sw.back() == 1);
      }
    }
  }

  // TC-09  Range Operations (append_range, prepend_range, insert_range, assign_range)
  {
    // append_range / prepend_range / insert_range with C array
    xl::list lst{1, 2, 3};
    int const vec[]{4, 5, 6};
    lst.append_range(vec);
    assert((lst == std::array{1, 2, 3, 4, 5, 6}));
    lst.prepend_range(vec);
    assert((lst == std::array{4, 5, 6, 1, 2, 3, 4, 5, 6}));
    auto it = lst.begin(); std::advance(it, 3);
    lst.insert_range(it, vec);
    assert((lst == std::array{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));

    // same operations with xl::list sources
    xl::list ml{1, 2, 3}, al{4, 5, 6}, pl{7, 8, 9}, il{10, 11, 12};
    ml.append_range(al);   assert((ml == xl::list{1, 2, 3, 4, 5, 6}));
    ml.prepend_range(pl);  assert((ml == xl::list{7, 8, 9, 1, 2, 3, 4, 5, 6}));
    auto pos = std::find(ml.begin(), ml.end(), 1);
    ml.insert_range(pos, il);
    assert((ml == xl::list{7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6}));
    ml.assign_range(xl::list{13, 14, 15});
    assert((ml == xl::list{13, 14, 15}));

    // assign_range from C array
    int const ra[]{6, 7, 8, 9, 10};
    xl::list<int> lst2;
    lst2.assign_range(ra);
    assert(lst2.size() == std::size(ra));
    auto li = lst2.begin(); auto ai = std::begin(ra);
    while (li) assert(*li++ == *ai++);

    // operator=(range) replaces content
    lst2 = ra;
    assert(lst2.size() == std::size(ra));

    // assign_range from xl::list
    xl::list src = {13, 14, 15};
    lst2.assign_range(src);
    assert((lst2 == std::array{13, 14, 15}));

    // move-assign array into xl::list (elements are moved-from)
    std::string sa[]{"A", "B", "C"};
    xl::list<std::string> sb;
    assert(std::ranges::none_of(sa, [](auto const& s){ return s.empty(); }));
    sb = std::move(sa);
    assert(std::ranges::all_of(sa,  [](auto const& s){ return s.empty(); }));
    assert(std::ranges::none_of(sb, [](auto const& s){ return s.empty(); }));
    sb.assign_range({"a", {"b"}, {"c"}});
    assert(sb.size() == 3);

    // append_range with rvalue range (move semantics)
    xl::list<std::string> dest;
    xl::list<std::string> src2 = {"x", "y", "z"};
    dest.append_range(std::move(src2));
    assert(dest.size() == 3 && dest.front() == "x");

    // append_range to empty list
    {
      xl::list<int> ae;
      int src_arr[] = {1, 2, 3};
      ae.append_range(src_arr);
      assert((ae == xl::list{1, 2, 3}));
    }

    // prepend_range to empty list
    {
      xl::list<int> pe;
      int src_arr[] = {7, 8, 9};
      pe.prepend_range(src_arr);
      assert((pe == xl::list{7, 8, 9}));
    }

    // insert_range at begin
    {
      xl::list ir = {4, 5, 6};
      int pre[] = {1, 2, 3};
      ir.insert_range(ir.begin(), pre);
      assert((ir == xl::list{1, 2, 3, 4, 5, 6}));
    }
  }

  // TC-10  Comparison Operators and Free-Function Algorithms
  {
    // comparison operators
    xl::list a={1,2,3}, b={1,2,3}, c={1,2,4}, d={1,2};
    assert(a==b && a!=c && a!=d && d<a && c>a && d<=a && a>=b && c>=a);

    // heterogeneous (int vs long)
    xl::list li1 = {1, 2, 3};
    xl::list<long> li2 = {1, 2, 3};
    xl::list li3 = {1, 2, 4};
    assert(li1 == li2 && li1 != li3 && li1 < li3 && li3 > li1);

    // two empty lists are equal
    {
      xl::list<int> e1, e2;
      assert(e1 == e2 && !(e1 != e2) && !(e1 < e2) && !(e1 > e2));
    }

    // empty list is less than any non-empty list
    {
      xl::list<int> em;
      xl::list ne = {1};
      assert(em < ne && !(ne < em) && em != ne);
    }

    // three-way comparison / spaceship
    {
      xl::list x = {1, 2, 3};
      xl::list y = {1, 2, 3};
      xl::list z = {1, 2, 4};
      assert((x <=> y) == std::strong_ordering::equal);
      assert((x <=> z) == std::strong_ordering::less);
      assert((z <=> x) == std::strong_ordering::greater);
    }

    // xl::find / xl::find_if / xl::erase / xl::erase_if
    {
      // erase-remove idiom
      xl::list lst = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      lst.erase(std::remove(lst.begin(), lst.end(), 2), lst.end());
      assert(std::find(lst.begin(), lst.end(), 2) == lst.end());
      assert(!xl::find(lst, 2));
      lst.erase(std::remove_if(lst.begin(), lst.end(),
        [](int i){ return i % 2 == 0; }), lst.end());
      assert(!xl::find_if(lst, [](int i){ return i % 2 == 0; }));

      // xl::erase / xl::erase_if
      xl::list lst2 = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      xl::erase(lst2, 2);
      assert(!xl::find(lst2, 2));
      xl::erase_if(lst2, [](int i){ return i % 2 == 0; });
      assert(!xl::find_if(lst2, [](int i){ return i % 2 == 0; }));

      // chained erases
      xl::list lst3 = {1, 2, 3, 4, 5, 3, 6, 3, 7};
      xl::erase(lst3, 3);
      xl::erase_if(lst3, [](int v){ return v > 4; });
      assert(!xl::find(lst3, 3));
      assert(std::none_of(lst3.begin(), lst3.end(), [](int v){ return v > 4; }));

      // xl::erase_if returns correct removal count
      xl::list lst4 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      auto removed = xl::erase_if(lst4, [](int x){ return x % 2 == 0; });
      assert(removed == 5 && lst4.size() == 5);
      for (const auto& v : lst4) assert(v % 2 != 0);
      auto r2 = xl::erase_if(lst4, [](int){ return false; });
      assert(r2 == 0 && lst4.size() == 5);

      // xl::find and xl::erase (variadic)
      xl::list lstf{3, 1, 2};
      *xl::find(lstf, 3) = 1;
      assert(lstf.front() == 1);
      xl::erase(lstf, 3, 2, 1);
      assert(lstf.empty());

      // xl::find on const list
      const xl::list clst = {10, 20, 30, 40};
      auto cit = xl::find(clst, 30);
      assert(cit && *cit == 30);
      assert(!xl::find(clst, 99));

      // xl::find returns iterator that can be used to modify
      {
        xl::list mf = {1, 2, 3};
        auto fit = xl::find(mf, 2);
        assert(fit);
        *fit = 99;
        assert((mf == xl::list<int>{1, 99, 3}));
      }

      // xl::find_if on empty list returns falsy iterator
      {
        xl::list<int> ef;
        assert(!xl::find_if(ef, [](int){ return true; }));
      }

      // xl::erase on empty list is a no-op (no crash)
      {
        xl::list<int> ef2;
        xl::erase(ef2, 42);
        assert(ef2.empty());
      }

      // xl::find with initializer list argument (single value wrapped in braces)
      { xl::list z = {1, 2, 3}; assert(xl::find(z, {1})); }

      // xl::find with variadic multi-value arguments (all present)
      { xl::list z = {1, 2, 3}; assert(xl::find(z, 1, 2, 3)); }
    }
  }

  // TC-11  Standard Algorithms Interoperability
  {
    xl::list lst = {1, 2, 3, 4, 5};

    // std::accumulate
    assert(std::accumulate(lst.begin(), lst.end(), 0) == 15);

    // std::transform in-place
    std::transform(lst.begin(), lst.end(), lst.begin(), [](int x){ return x * 2; });
    assert(lst.front() == 2 && lst.back() == 10);

    // std::count_if
    assert(std::count_if(lst.begin(), lst.end(), [](int x){ return x % 2 == 0; }) == 5);

    // std::copy into list via back_inserter
    std::vector src = {1, 2, 3, 4, 5};
    xl::list<int> dst;
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    assert(dst.size() == 5 && std::equal(src.begin(), src.end(), dst.begin()));

    // std::fill modifies all elements in-place
    xl::list fill_lst = {1, 2, 3, 4, 5};
    std::fill(fill_lst.begin(), fill_lst.end(), 0);
    for (const auto& v : fill_lst) assert(v == 0);

    // std::is_sorted after sort
    xl::list ul = {5, 2, 8, 1, 9};
    ul.sort();
    assert(std::is_sorted(ul.begin(), ul.end()));

    // std::for_each accumulates into external variable
    {
      xl::list fe = {1, 2, 3, 4, 5};
      int total = 0;
      std::for_each(fe.begin(), fe.end(), [&](int v){ total += v; });
      assert(total == 15);
    }

    // std::copy into vector via std::back_inserter, then compare
    {
      xl::list src2 = {10, 20, 30};
      std::vector<int> dst2;
      std::copy(src2.begin(), src2.end(), std::back_inserter(dst2));
      assert((dst2 == std::vector{10, 20, 30}));
    }

    // std::find on an xl::list
    {
      xl::list sf = {5, 3, 8, 1, 4};
      auto fit = std::find(sf.begin(), sf.end(), 8);
      assert(fit != sf.end() && *fit == 8);
      assert(std::find(sf.begin(), sf.end(), 99) == sf.end());
    }

    // std::rotate via std::rotate algorithm (if supported by bidirectional)
    {
      xl::list rot = {1, 2, 3, 4, 5};
      auto mid2 = std::next(rot.begin(), 2);
      std::rotate(rot.begin(), mid2, rot.end());
      assert((rot == xl::list{3, 4, 5, 1, 2}));
    }

    // std::reverse_copy from xl::list into vector
    {
      xl::list rc = {1, 2, 3, 4, 5};
      std::vector<int> rv;
      std::reverse_copy(rc.begin(), rc.end(), std::back_inserter(rv));
      assert((rv == std::vector{5, 4, 3, 2, 1}));
    }

    // std::ranges::reverse
    {
      xl::list rng{1, 2, 3, 4, 5};
      std::ranges::reverse(rng);
      assert((rng == std::array{5, 4, 3, 2, 1}));
    }

    // std::ranges::equal
    {
      xl::list ra = {1, 2, 3};
      xl::list rb = {1, 2, 3};
      assert(std::ranges::equal(ra, rb));
      rb.back() = 99;
      assert(!std::ranges::equal(ra, rb));
    }

    // std::ranges::size / empty / begin / end
    {
      xl::list rl = {1, 2, 3, 4, 5};
      assert(std::ranges::size(rl) == 5);
      xl::list<int> re;
      assert(std::ranges::empty(re));
      re.push_back(1);
      assert(!std::ranges::empty(re));
      assert(std::ranges::begin(rl) == rl.begin());
      assert(std::ranges::end(rl)   == rl.end());
    }
  }

  // TC-12  Edge Cases: Empty, Single-Element, and Two-Element Lists
  {
    // --- single-element list ---
    {
      xl::list<int> lst;
      lst.push_back(42);
      assert(!lst.empty() && lst.size() == 1 && lst.front() == 42 && lst.back() == 42);
      lst.sort();    assert(lst.size() == 1);
      lst.reverse(); assert(lst.size() == 1 && lst.front() == 42);
      lst.unique();  assert(lst.size() == 1);
      lst.remove(99); assert(lst.size() == 1);
      lst.remove_if([](int x){ return x == 99; }); assert(lst.size() == 1);
      lst.pop_front(); assert(lst.empty());

      lst.push_front(100); lst.pop_back(); assert(lst.empty());

      lst.push_back(200); lst.erase(lst.begin()); assert(lst.empty());

      // splice a single-element list into itself is a no-op
      lst.push_back(7);
      lst.splice(lst.begin(), lst, lst.begin()); // identity splice
      assert(lst.size() == 1 && lst.front() == 7);

      // merge single-element into itself (via empty other) leaves it alone
      {
        xl::list one = {5};
        xl::list<int> empty_other;
        one.merge(empty_other);
        assert(one.size() == 1 && one.front() == 5);
      }

      // assign a single-element list to another single-element list
      {
        xl::list a = {1};
        xl::list b = {2};
        a = b;
        assert(a.size() == 1 && a.front() == 2);
        assert(b.size() == 1 && b.front() == 2);
        assert(a.front() == b.front());
      }

      // emplace_back on empty list front and back are the same element
      {
        xl::list<std::string> es;
        auto& ref = es.emplace_back("only");
        assert(ref == "only");
        assert(es.size() == 1 && es.front() == "only" && es.back() == "only");
      }

      // emplace_front on empty list same node is front and back
      {
        xl::list<int> ef;
        ef.emplace_front(77);
        assert(ef.size() == 1 && ef.front() == 77 && ef.back() == 77);
      }

      // emplace into single-element list at begin and at end
      {
        xl::list<int> sel = {5};
        sel.emplace(sel.begin(), 3);   // now {3, 5}
        sel.emplace(sel.end(),   7);   // now {3, 5, 7}
        assert((sel == xl::list{3, 5, 7}));
      }
    }

    // --- two-element list ---
    {
      // insert into two-element list at every position
      {
        xl::list t = {1, 3};
        t.insert(std::next(t.begin()), 2);
        assert((t == xl::list{1, 2, 3}));
      }

      // erase from two-element list: erase front
      {
        xl::list t = {1, 2};
        t.erase(t.begin());
        assert(t.size() == 1 && t.front() == 2 && t.back() == 2);
      }

      // erase from two-element list: erase back
      {
        xl::list t = {1, 2};
        t.erase(std::prev(t.end()));
        assert(t.size() == 1 && t.front() == 1 && t.back() == 1);
      }

      // sort two-element list (both orderings)
      {
        xl::list t1 = {2, 1}; t1.sort();
        assert(t1.front() == 1 && t1.back() == 2);
        xl::list t2 = {1, 2}; t2.sort();
        assert(t2.front() == 1 && t2.back() == 2);
      }

      // reverse two-element list
      {
        xl::list t = {1, 2}; t.reverse();
        assert(t.front() == 2 && t.back() == 1);
      }

      // bidirectional traversal of two-element list
      {
        xl::list t = {10, 20};
        auto it = t.begin();
        assert(*it == 10); ++it;
        assert(*it == 20); ++it;
        assert(it == t.end());
        --it;
        assert(*it == 20); --it;
        assert(*it == 10);
      }
    }

    // --- reducing to exactly one element ---
    {
      xl::list lst = {1, 2, 3};
      lst.pop_back();
      assert(lst.size() == 2 && lst.back() == 2);
      lst.pop_front();
      assert(lst.size() == 1 && lst.front() == 2 && lst.back() == 2);

      xl::list lst2 = {10, 20, 30};
      lst2.pop_front();
      assert(lst2.size() == 2 && lst2.front() == 20);
      lst2.pop_back();
      assert(lst2.size() == 1 && lst2.front() == 20 && lst2.back() == 20);

      // multiple alternating pops converging to one element
      {
        xl::list alt2 = {1, 2, 3, 4, 5, 6};
        while (alt2.size() > 1) {
          alt2.pop_front();
          if (alt2.size() > 1) alt2.pop_back();
        }
        assert(alt2.size() == 1);
        assert(alt2.front() == alt2.back());
      }
    }

    // --- empty list operations ---
    {
      xl::list<int> e;
      assert(e.empty() && e.size() == 0);
      // sort empty
      e.sort(); assert(e.empty());
      // reverse empty
      e.reverse(); assert(e.empty());
      // unique empty
      assert(e.unique() == 0);
      // remove from empty
      assert(e.remove(42) == 0);
      assert(e.remove_if([](int){ return true; }) == 0);
      // erase empty range
      assert(e.erase(e.begin(), e.end()) == e.end());
      // merge empty into empty
      xl::list<int> e2;
      e.merge(e2);
      assert(e.empty() && e2.empty());
    }
  }

  // TC-13  Complex and Nested Value Types
  {
    // nested list
    xl::list<xl::list<int>> nested;
    nested.push_back({1, 2, 3});
    nested.push_back({4, 5, 6});
    nested.push_back({7, 8, 9});
    assert(nested.size() == 3 && nested.front().size() == 3 && nested.back().back() == 9);
    xl::list<int> flat;
    for (const auto& inner : nested)
      for (const auto& val : inner)
        flat.push_back(val);
    assert(flat.size() == 9 && flat.back() == 9);

    // copy nested list; inner lists are deeply copied
    {
      xl::list<xl::list<int>> orig = {{1,2},{3,4},{5,6}};
      xl::list<xl::list<int>> copy2 = orig;
      assert(copy2.size() == 3);
      orig.front().front() = 99;
      assert(copy2.front().front() == 1);
    }

    // move nested list; source is empty
    {
      xl::list<xl::list<int>> m1 = {{1,2,3},{4,5,6}};
      xl::list<xl::list<int>> m2 = std::move(m1);
      assert(m1.empty());
      assert(m2.size() == 2 && m2.front().size() == 3);
    }
  }

  // TC-14  Exception Safety
  {
    // push_back where constructor throws
    {
      struct TestException : std::exception {};
      struct Thrower {
        Thrower() = default;
        Thrower(int) { throw TestException(); }
      };
      xl::list<Thrower> lst;
      lst.push_back(Thrower{});
      try {
        lst.push_back(42);
        assert(false && "exception not thrown");
      } catch (const TestException&) {
        assert(lst.size() == 1);
      }
    }

    // insert where copy constructor throws
    {
      struct ThrowOnCopy {
        int value;
        ThrowOnCopy(int v) : value(v) {}
        ThrowOnCopy(const ThrowOnCopy& o) : value(o.value) {
          if (value == 42) throw std::runtime_error("copy failed");
        }
      };
      xl::list<ThrowOnCopy> lst;
      lst.emplace_back(1); lst.emplace_back(2);
      try {
        lst.insert(lst.begin(), ThrowOnCopy(42));
        assert(false && "exception not thrown");
      } catch (const std::runtime_error&) {
        assert(lst.size() == 2 && lst.front().value == 1);
      }
    }

    // exception in emplace_back leaves list unchanged
    {
      struct ThrowOnN {
        int v;
        ThrowOnN(int n) : v(n) { if (n == 3) throw std::runtime_error("boom"); }
      };
      xl::list<ThrowOnN> lst;
      lst.emplace_back(1);
      lst.emplace_back(2);
      try {
        lst.emplace_back(3);
        assert(false);
      } catch (const std::runtime_error&) {
        assert(lst.size() == 2);
        assert(lst.front().v == 1 && lst.back().v == 2);
      }
    }

    // copy-throwing type leaves list unchanged on push_back
    {
      static int throw_cnt;
      throw_cnt = 0;
      struct ThrowOnCopy {
        int val{};
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int v) : val(v) {}
        ThrowOnCopy(const ThrowOnCopy&) {
          if (++throw_cnt == 2) throw std::bad_alloc();
        }
      };
      xl::list<ThrowOnCopy> z;
      z.emplace_back(1);
      z.emplace_back(2);
      bool caught = false;
      try {
        z.push_back(ThrowOnCopy(3));
      } catch (const std::bad_alloc&) {
        caught = true;
      }
      assert(z.size() == 2);
      assert(caught);
    }
  }

  // TC-15  Resource Management and Destructor Balance
  {
    static int counter = 0;
    struct Counted {
      Counted()               { ++counter; }
      Counted(const Counted&) { ++counter; }
      ~Counted()              { --counter; }
    };
    {
      xl::list<Counted> lst;
      lst.push_back(Counted()); lst.push_back(Counted());
      assert(counter == 2);
    }
    assert(counter == 0);
    {
      xl::list<Counted> lst;
      lst.push_back(Counted()); lst.push_back(Counted());
      xl::list<Counted> lst2 = std::move(lst);
      assert(counter == 2);
    }
    assert(counter == 0);

    // destructor balance: clear() decrements counter to 0
    {
      counter = 0;
      xl::list<Counted> lst;
      for (int i = 0; i < 5; ++i) lst.emplace_back();
      assert(counter == 5);
      lst.clear();
      assert(counter == 0);
      assert(lst.empty());
    }

    // destructor balance: pop_back / pop_front each decrement counter
    {
      counter = 0;
      xl::list<Counted> lst;
      lst.emplace_back(); lst.emplace_back(); lst.emplace_back();
      assert(counter == 3);
      lst.pop_back();  assert(counter == 2);
      lst.pop_front(); assert(counter == 1);
      lst.pop_back();  assert(counter == 0);
      assert(lst.empty());
    }

    // destructor balance: erase(range) decrements correctly
    {
      counter = 0;
      xl::list<Counted> lst;
      for (int i = 0; i < 6; ++i) lst.emplace_back();
      assert(counter == 6);
      auto b2 = std::next(lst.begin());
      auto e2 = std::prev(lst.end());
      lst.erase(b2, e2); // removes 4 elements
      assert(counter == 2);
    }
    assert(counter == 0); // outer scope destructor cleans last 2
  }

  // TC-16  Large-List Stress Tests
  {
    // 1M push_back / clear
    {
      constexpr int N = 1'000'000;
      xl::list lst(std::views::iota(0, N));
      assert(lst.size() == N && lst.front() == 0 && lst.back() == N - 1);
      lst.clear(); assert(lst.empty());
    }

    // push_back + front/back + sum
    {
      const std::size_t N = 100000;
      xl::list<std::size_t> large;
      for (std::size_t i = 0; i < N; ++i) large.push_back(i);
      assert(large.size() == N && large.front() == 0 && large.back() == N - 1);
      std::size_t sum = 0;
      for (auto it = large.begin(); it != large.end(); ++it) sum += *it;
      assert(sum == (N - 1) * N / 2);
    }

    // swap between two large lists
    {
      xl::list<int> la, lb;
      for (int i = 0; i < 100000; ++i) { la.push_back(i); lb.push_back(i * 2); }
      la.swap(lb);
      assert(la.front() == 0 && lb.front() == 0);
    }

    // xl::find, reverse, sort on large list
    {
      const std::size_t N = 50000;
      xl::list<std::size_t> lst;
      for (std::size_t i = 0; i < N; ++i) lst.push_back(i);
      assert(lst.size() == N);
      auto found = xl::find(lst, N / 2);
      assert(found && *found == N / 2);
      lst.reverse();
      assert(lst.front() == N - 1 && lst.back() == 0);
      lst.sort();
      assert(lst.front() == 0 && lst.back() == N - 1);
    }

    // fragmented push/pop pattern preserves size and sortability
    {
      xl::list<int> lst;
      for (int i = 0; i < 1000; ++i) {
        lst.push_back(i);
        if (i % 3 == 0) lst.pop_front();
      }
      assert(!lst.empty());
      auto sz = lst.size();
      lst.sort();
      assert(lst.size() == sz);
    }

    // build reversed copy via rbegin, then sort original
    {
      constexpr std::size_t N = 100'000;
      xl::list<std::size_t> large;
      for (std::size_t i = 0; i < N; ++i) large.push_back(i);
      xl::list<std::size_t> reversed;
      for (auto it = large.rbegin(); it != large.rend(); ++it)
        reversed.push_back(*it);
      assert(reversed.front() == N - 1);
      large.sort();
      assert(large.front() == 0 && large.back() == N - 1);
    }

    // sort + unique on many duplicates
    {
      xl::list<int> lst;
      for (int i = 0; i < 1000; ++i) lst.push_back(i % 10);
      lst.sort(); lst.unique();
      assert(lst.size() == 10);
      int expected = 0;
      for (const auto v : lst) assert(v == expected++);
    }

    // repeated push/pop cycles (allocator stress)
    {
      xl::list<int> lst;
      for (int cycle = 0; cycle < 100; ++cycle) {
        for (int i = 0; i < 10; ++i) lst.push_back(i);
        for (int i = 0; i <  5; ++i) lst.pop_back();
      }
      assert(lst.size() == 500);
      lst.clear(); assert(lst.empty());
    }

    // large list: sort variants 0–4 agree on size 500 pseudorandom input
    {
      std::mt19937 rng2(1234);
      std::vector<int> data;
      data.reserve(500);
      for (int i = 0; i < 500; ++i)
        data.push_back(static_cast<int>(rng2() % 10000));

      xl::list<int> ref_l(data.begin(), data.end()); ref_l.sort<0>();
      xl::list<int> l1(data.begin(), data.end());    l1.sort<1>();
      xl::list<int> l2(data.begin(), data.end());    l2.sort<2>();
      xl::list<int> l3(data.begin(), data.end());    l3.sort<3>();
      xl::list<int> l4(data.begin(), data.end());    l4.sort<4>();
      assert(l1 == ref_l && l2 == ref_l && l3 == ref_l && l4 == ref_l);
    }

    // assign(1M, value): verifies allocator and size tracking at scale
    {
      xl::list<int> z;
      z.assign(1'000'000, 42);
      assert(z.size() == 1'000'000);
      assert(std::all_of(z.begin(), z.end(), [](int v){ return v == 42; }));
      z.assign(0, 0);
      assert(z.empty());
    }
  }

  // TC-17  Comprehensive Composition Smoke Test
  {
    // comprehensive single-block exercise of most member functions
    xl::list<int> lst;
    lst.push_back(10, 20, 30); lst.push_front(5);
    lst.pop_back();  lst.pop_front();
    lst.insert(lst.begin(), 15); lst.erase(lst.begin());
    lst.clear(); lst.resize(5, 100);
    assert(lst.size() == 5 && lst.back() == 100);
    lst.assign(3, 200); assert(lst.size() == 3 && lst.front() == 200);
    xl::list lst2 = {1, 2, 3};
    lst.splice(lst.begin(), lst2);
    assert(lst.size() == 6 && lst.front() == 1 && lst2.empty());
    lst.remove(1);
    lst.remove_if([](int i){ return i > 2; });
    lst.push_back(2); lst.unique();
    assert(lst.size() == 1 && lst.back() == 2);
    xl::list lst3 = {1, 3};
    lst.merge(lst3);
    lst.sort(); lst.reverse();
    assert(lst.front() == 3 && lst.back() == 1);

    // self-merge via splice then merge
    xl::list smrg = {1, 3, 5, 2, 4, 6};
    xl::list<int> back2;
    auto mid = std::next(smrg.begin(), 3);
    back2.splice(back2.begin(), smrg, mid, smrg.end());
    smrg.merge(back2);
    assert(back2.empty() && smrg.size() == 6);
    assert(std::is_sorted(smrg.begin(), smrg.end()));

    // push/sort/splice/sort round-trip
    {
      xl::list a2 = {5, 1, 4, 2, 3};
      xl::list b2 = {8, 6, 7};
      a2.sort();
      b2.sort();
      a2.splice(a2.end(), b2);
      a2.sort();
      assert(std::is_sorted(a2.begin(), a2.end()) && a2.size() == 8);
    }

    // interleaved operations: push_back, erase, sort, reverse, check
    {
      xl::list<int> c2;
      for (int i = 10; i >= 1; --i) c2.push_back(i);
      for (auto it = c2.begin(); it != c2.end();) {
        if (*it % 2 == 0) it = c2.erase(it);
        else ++it;
      }
      assert(c2.size() == 5); // 1,3,5,7,9
      c2.sort();
      c2.reverse();
      assert(c2.front() == 9 && c2.back() == 1);
    }
  }
}

int main()
{
  test();
  std::cout << "All tests passed!\n";
  return 0;
}

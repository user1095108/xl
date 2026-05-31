#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

#include "list.hpp"

void test()
{
  // ── TC-01  Basic push/pop/insert/erase/clear (all overloads) ─────────────
  {
    xl::list<int> l;
    assert(l.empty() && l.size() == 0);

    // variadic push_back, operator[], size
    l.push_back(1, 2, 3);
    assert(l.size() == 3 && l[0] == 1 && l[1] == 2 && l[2] == 3);
    l[1] = 42;
    assert(l[1] == 42);

    // front / back
    assert(l.front() == 1 && l.back() == 3);

    // pop_back / pop_front
    l.pop_back();  assert(l.size() == 2 && l.back()  == 42);
    l.pop_front(); assert(l.size() == 1 && l.front() == 42);

    // single-element list: front == back
    assert(l.front() == l.back());
    l.pop_back(); assert(l.empty());

    // push_front + push_back in alternation
    for (int i = 0; i < 5; ++i) l.push_back(i);
    l.push_front(100);
    assert(l.front() == 100 && l.back() == 4);
    l.pop_front(); assert(l.front() == 0);
    l.pop_back();  assert(l.back()  == 3);

    // sequential forward walk
    {
      int expected = 0;
      for (auto v : l) assert(v == expected++);
    }

    // insert returning iterator and erase returning iterator
    l.clear(); assert(l.empty() && l.size() == 0);
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

    // variadic push_front
    l.clear();
    l.push_back(1, 2, 3);
    l.push_front(0);
    assert(l.size() == 4 && (l == std::array{0, 1, 2, 3}));

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

    // swap; copy construction; range-based-for sum
    decltype(l) other; other.push_back(6, 7);
    l.swap(other);
    assert(l.size() == 2 && other.size() == 3);
    decltype(l) copied = l;
    assert(copied.size() == 2 && copied[0] == 6);
    int sum = 0;
    for (const auto& x : l) sum += x;
    assert(sum == 13);
  }

  // ── TC-02  Constructors ───────────────────────────────────────────────────
  {
    // default
    xl::list<int> l1; assert(l1.empty());

    // fill: (count) and (count, value)
    xl::list<int> l2(5);    assert(l2.size() == 5);
    xl::list<int> l3(5, 42); assert(l3.size() == 5 && l3.front() == 42);
    for (const auto& e : l3) assert(e == 42);

    // initialiser-list
    xl::list l4 = {1, 2, 3, 4, 5}; assert(l4.size() == 5 && l4.back() == 5);

    // iterator-range
    int arr[] = {1, 2, 3, 4, 5};
    xl::list<int> l5(std::begin(arr), std::end(arr));
    assert(l5.size() == 5 && l5.front() == 1 && l5.back() == 5);

    // from_range tag
    xl::list l6(xl::from_range, arr);
    assert(l6.size() == 5 && l6.front() == 1 && l6.back() == 5);

    // from_range + string_view
    xl::list pal(xl::from_range, std::string_view("racecar"));
    while (pal.size() > 1) {
      assert(pal.front() == pal.back());
      pal.pop_front(); pal.pop_back();
    }
    assert(pal.size() == 1);

    // multi_t constructor
    xl::list lm{xl::multi, 1, 2, 3, 4, 5};
    assert(lm.size() == 5 && lm.front() == 1 && lm.back() == 5);

    // multi_t round-trips
    lm.push_front(0); assert(lm.front() == 0);
    lm.push_back(6);  assert(lm.back()  == 6);
    lm.pop_front();   assert(lm.front() == 1);
    lm.pop_back();    assert(lm.back()  == 5);
    lm.clear(); assert(lm.empty());
    lm.push_front(1); assert(lm.front() == 1); lm.pop_front(); assert(lm.empty());
    lm.push_back(6);  assert(lm.back()  == 6); lm.pop_back();  assert(lm.empty());
    lm.insert(lm.begin(), 2); assert(*lm.begin() == 2);
    lm.erase(lm.begin());     assert(lm.empty());

    // views::iota range constructor + range assignment
    xl::list l7(std::views::iota(0, 100));
    assert(l7.size() == 100);
    l7 = std::views::iota(0, 10);
    assert(l7.size() == 10);

    // empty range, single-pointer range, istream_iterator
    std::vector<int> ev;
    xl::list<int> el(ev.begin(), ev.end()); assert(el.empty());
    int single = 42;
    xl::list<int> sl(&single, &single + 1);
    assert(sl.size() == 1 && sl.front() == 42);
    std::istringstream iss("1 2 3 4");
    xl::list<int> il(std::istream_iterator<int>(iss), std::istream_iterator<int>{});
    assert(il.size() == 4 && il.back() == 4);

    // range-constructed equals C array
    int const ca[10]{};
    assert(std::ranges::equal(ca, xl::list(ca)));
  }

  // ── TC-03  Copy / move construction and assignment; swap ─────────────────
  {
    xl::list l1{1, 2, 3, 4, 5};

    // copy constructor
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

    // swap
    xl::list a = {1, 2, 3}, b = {4, 5};
    a.swap(b);
    assert(a.size() == 2 && a.front() == 4 && a.back() == 5);
    assert(b.size() == 3 && b.front() == 1 && b.back() == 3);
    std::swap(a, b);
    assert(a.size() == 3 && a.front() == 1 && a.back() == 3);
    assert(b.size() == 2 && b.front() == 4 && b.back() == 5);

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
  }

  // ── TC-04  Assign variants ────────────────────────────────────────────────
  {
    xl::list<int> lst;

    // assign from initialiser list to empty list
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

    // assign_range from C array
    int const ra[]{6, 7, 8, 9, 10};
    lst.assign_range(ra);
    assert(lst.size() == std::size(ra));
    auto li = lst.begin(); auto ai = std::begin(ra);
    while (li) assert(*li++ == *ai++);

    // operator=(range)
    lst = ra;
    assert(lst.size() == std::size(ra));

    // assign_range from xl::list
    xl::list src = {13, 14, 15};
    lst.assign_range(src);
    assert((lst == std::array{13, 14, 15}));
  }

  // ── TC-05  Emplace variants ───────────────────────────────────────────────
  {
    // emplace_back / emplace_front return reference
    xl::list<int> li;
    auto& rb = li.emplace_back(10);  assert(rb == 10 && li.back()  == 10);
    auto& rf = li.emplace_front(5);  assert(rf == 5  && li.front() == 5);

    // emplace returns iterator to new element
    auto it = li.emplace(std::next(li.begin()), 7);
    assert(*it == 7 && li.size() == 3);
    assert((li == std::array{5, 7, 10}));

    // emplace at begin / end / middle with std::string
    xl::list<std::string> sl = {"world"};
    sl.emplace(sl.begin(), "hello");
    sl.emplace(sl.end(),   "!");
    assert(sl.size() == 3 && sl.front() == "hello" && sl.back() == "!");
    auto sit = sl.begin(); ++sit;
    sit = sl.emplace(sit, "there");
    assert(*sit == "there");
    assert((sl == xl::list<std::string>{"hello", "there", "world", "!"}));

    // emplace_back / emplace_front with pair (multi-arg constructor)
    xl::list<std::pair<int, int>> pl;
    pl.emplace_back(1, 2); pl.emplace_front(3, 4);
    auto pit = pl.begin(); ++pit;
    pit = pl.emplace(pit, 5, 6);
    assert(pl.size()  == 3);
    assert(pl.front() == std::pair(3,4));
    assert(pl.back()  == std::pair(1,2));
    assert(*pit       == std::pair(5,6));

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
  }

  // ── TC-06  insert overloads ───────────────────────────────────────────────
  {
    // single at begin / end / middle
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
  }

  // ── TC-07  erase overloads ────────────────────────────────────────────────
  {
    // erase two elements in sequence
    xl::list l{1, 2, 3, 4, 5};
    auto it = l.begin();
    l.erase(std::next(it, 2));
    l.erase(it);
    assert(l.size() == 3 && l.front() == 2 && l.back() == 5);

    // erase(begin, begin) is a no-op (empty range)
    xl::list lst = {1, 2, 3};
    auto ret = lst.erase(lst.begin(), lst.begin());
    assert(lst.size() == 3 && ret == lst.begin());

    // erase range in the middle; return iterator and endpoints correct
    xl::list mid = {1, 2, 3, 4, 5, 6, 7};
    auto first = std::next(mid.begin(), 2); // points to 3
    auto last  = std::next(first, 3);        // points to 6
    ret = mid.erase(first, last);
    assert(mid.size() == 4 && mid.front() == 1 && mid.back() == 7 && *ret == 6);

    // erase range spanning entire list
    xl::list all = {1, 2, 3, 4, 5};
    ret = all.erase(all.begin(), all.end());
    assert(all.empty() && ret == all.end());
  }

  // ── TC-08  Iterator correctness ───────────────────────────────────────────
  {
    // insert at end does not invalidate existing iterators
    xl::list l{1, 2, 3};
    auto itm = l.begin(); ++itm;       // points to 2
    l.insert(l.end(), 4);
    assert(l.size() == 4 && *itm == 2);

    // erase does not invalidate other iterators
    xl::list lst = {1, 2, 3, 4, 5};
    auto it1 = lst.begin(); ++it1;
    auto it2 = std::next(it1);
    lst.erase(it1);
    assert(*it2 == 3);
    auto it3 = lst.begin();
    lst.insert(std::next(it3), 10);
    assert(*it3 == 1 && *it2 == 3);

    // iterator validity around erase, insert, splice
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

    // iterator validity after erase of middle element (saved iterators)
    xl::list lstv = {1, 2, 3, 4, 5};
    std::vector<decltype(lstv.begin())> iters;
    for (auto it = lstv.begin(); it != lstv.end(); ++it) iters.push_back(it);
    lstv.erase(std::next(lstv.begin(), 2));
    assert(*iters[0] == 1 && *iters[1] == 2);
    assert(*iters[3] == 4 && *iters[4] == 5);

    // bidirectional traversal: forward and backward yield mirror
    xl::list lbidi = {10, 20, 30, 40, 50};
    std::vector<int> fwd, bwd;
    for (auto it = lbidi.begin();  it != lbidi.end();  ++it) fwd.push_back(*it);
    for (auto it = lbidi.rbegin(); it != lbidi.rend(); ++it) bwd.push_back(*it);
    for (std::size_t i = 0; i < fwd.size(); ++i)
      assert(fwd[i] == bwd[fwd.size() - 1 - i]);

    // std::distance between const_iterators equals size
    const xl::list clst = {1, 2, 3, 4, 5};
    assert(std::distance(clst.cbegin(), clst.cend()) == (std::ptrdiff_t)clst.size());

    // cbegin / cend on non-const list
    xl::list ncl = {10, 20, 30};
    int sum = 0;
    for (auto it = ncl.cbegin(); it != ncl.cend(); ++it) sum += *it;
    assert(sum == 60);
    assert(ncl.cbegin() == ncl.begin() && ncl.cend() == ncl.end());

    // const list: size, front, back, empty, cbegin/cend
    const xl::list cl = {10, 20, 30};
    assert(cl.size() == 3 && cl.front() == 10 && cl.back() == 30 && !cl.empty());
    sum = 0;
    for (auto it = cl.cbegin(); it; ++it) sum += *it;
    assert(sum == 60);

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
  }

  // ── TC-09  after_begin / before_end / rafter_begin / rbefore_end ─────────
  {
    xl::list lst = {1, 2, 3, 4, 5};

    // after_begin points to second element; before_end to last
    assert(*lst.after_begin() == 2);
    assert(*lst.before_end()  == 5);

    // cafter_begin / cbefore_end on const
    const xl::list clst = {10, 20, 30};
    assert(*clst.cafter_begin() == 20);
    assert(*clst.cbefore_end()  == 30);

    // rafter_begin / rbefore_end: reverse sentinels
    auto rab = lst.rafter_begin(); // wraps before_end; derefs to second-from-last (4)
    assert(*rab == 4);
    auto rbe = lst.rbefore_end();  // wraps after_begin; derefs to first element (1)
    assert(*rbe == 1);
  }

  // ── TC-10  resize ─────────────────────────────────────────────────────────
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
  }

  // ── TC-11  merge ─────────────────────────────────────────────────────────
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
  }

  // ── TC-12  splice ─────────────────────────────────────────────────────────
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
  }

  // ── TC-13  Range operations: append_range, prepend_range, insert_range ────
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

    // operator=(range) replaces content
    xl::list lr = {1, 2, 3, 4, 5};
    int arr2[] = {10, 20, 30};
    lr = arr2;
    assert(lr == arr2);

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
  }

  // ── TC-14  unique ─────────────────────────────────────────────────────────
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
  }

  // ── TC-15  remove / remove_if ─────────────────────────────────────────────
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
  }

  // ── TC-16  sort ───────────────────────────────────────────────────────────
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
  }

  // ── TC-17  reverse ────────────────────────────────────────────────────────
  {
    // basic reverse
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
  }

  // ── TC-18  Comparison operators ───────────────────────────────────────────
  {
    xl::list a={1,2,3}, b={1,2,3}, c={1,2,4}, d={1,2};
    assert(a==b && a!=c && a!=d && d<a && c>a && d<=a && a>=b && c>=a);

    // heterogeneous (int vs long)
    xl::list li1 = {1, 2, 3};
    xl::list<long> li2 = {1, 2, 3};
    xl::list li3 = {1, 2, 4};
    assert(li1 == li2 && li1 != li3 && li1 < li3 && li3 > li1);
  }

  // ── TC-19  xl::erase / xl::erase_if / xl::find / xl::find_if free functions
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
  }

  // ── TC-20  Container concept, type aliases, static_assert ─────────────────
  {
    static_assert(std::is_same_v<xl::list<int>::value_type,      int>);
    static_assert(std::is_same_v<xl::list<int>::reference,       int&>);
    static_assert(std::is_same_v<xl::list<int>::const_reference, const int&>);
    static_assert(std::bidirectional_iterator<xl::list<int>::iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::const_iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::reverse_iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::const_reverse_iterator>);
  }

  // ── TC-21  at(), operator[], front, back, max_size ───────────────────────
  {
    xl::list lst = {10, 20, 30, 40, 50};
    assert(lst.at(0) == 10 && lst.at(2) == 30 && lst.at(4) == 50);
    lst.at(2) = 99;
    assert(lst.at(2) == 99);
    assert(lst[0] == 10 && lst[4] == 50);
    const xl::list clst = {1, 2, 3};
    assert(clst.at(0) == 1 && clst.at(2) == 3);

    xl::list<int> empty;
    assert(empty.max_size() > 0);
  }

  // ── TC-22  Exception safety ────────────────────────────────────────────────
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
  }

  // ── TC-23  Resource management (destructor balance) ───────────────────────
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
  }

  // ── TC-24  Single-element list edge cases ─────────────────────────────────
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
  }

  // ── TC-25  iter_swap ──────────────────────────────────────────────────────
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
  }

  // ── TC-26  insert(multi_t, ...) variadic overload ─────────────────────────
  {
    xl::list lst = {1, 5};
    // insert multiple values before position
    auto pos = std::next(lst.begin()); // points to 5
    lst.insert(xl::multi, pos, 2, 3, 4);
    assert(lst.size() == 5 && std::is_sorted(lst.begin(), lst.end()));
    assert((lst == xl::list{1, 2, 3, 4, 5}));
  }

  // ── TC-27  Algorithms interop: accumulate, transform, count_if, copy, fill
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
  }

  // ── TC-28  Nested list ────────────────────────────────────────────────────
  {
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
  }

  // ── TC-29  Large-list stress ───────────────────────────────────────────────
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
  }

  // ── TC-30  Emplace-only (non-default-constructible) type ──────────────────
  {
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

  // ── TC-31  push/pop reducing list to exactly one element ─────────────────
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
  }

  // ── TC-32  Miscellaneous compositions ─────────────────────────────────────
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
  }
}

int main()
{
  test();
  std::cout << "All tests passed!\n";
  return 0;
}

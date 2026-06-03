// chatbottests.cpp  –  xl::list comprehensive test suite
//
// Organisation
// ─────────────────────────────────────────────────────────────────────────────
//  TC-01  Type Traits, Concepts, and Aliases
//  TC-02  Construction and Destruction (including move-only / non-trivial types)
//  TC-03  Assignment and Swap
//  TC-04  Element Access and Capacity
//  TC-05  Push, Pop, and Emplace
//  TC-06  Insert and Erase
//  TC-07  Iterators, Sentinels, and Traversal
//  TC-08  resize and reverse
//  TC-09  sort (all variants, comparators, sub-range, free function)
//  TC-10  merge
//  TC-11  splice
//  TC-12  unique, remove / remove_if, and iter_swap
//  TC-13  Range Operations (append_range, prepend_range, insert_range, assign_range)
//  TC-14  Comparison Operators and Free-Function Algorithms (find, erase, erase_if)
//  TC-15  Standard Algorithms Interoperability
//  TC-16  Edge Cases: Empty, Single-Element, and Two-Element Lists
//  TC-17  Complex and Nested Value Types
//  TC-18  Exception Safety
//  TC-19  Resource Management and Destructor Balance
//  TC-20  Large-List Stress Tests
//  TC-21  Comprehensive Composition Smoke Test

#include <array>
#include <cassert>
#include <cctype>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "list.hpp"

void test()
{
  // ─── TC-01  Type Traits, Concepts, and Aliases ──────────────────────────────
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

    // value_type of a non-int instantiation
    static_assert(std::is_same_v<xl::list<std::string>::value_type, std::string>);
    // size_type and difference_type are present
    static_assert(std::is_unsigned_v<xl::list<int>::size_type>);
    static_assert(std::is_signed_v<xl::list<int>::difference_type>);
  }

  // ─── TC-02  Construction and Destruction ────────────────────────────────────
  {
    // default: empty
    xl::list<int> l1;
    assert(l1.empty());

    // fill: (count) and (count, value)
    xl::list<int> l2(5);
    assert(l2.size() == 5);
    xl::list l3(5, 42);
    assert(l3.size() == 5 && l3.front() == 42);
    for (const auto& e : l3) assert(e == 42);

    // fill edge cases: single element, zero elements
    xl::list<int> fill1(1, 77);
    assert(fill1.size() == 1 && fill1.front() == 77 && fill1.back() == 77);
    xl::list<int> fill0(0, 99);
    assert(fill0.empty());

    // initialiser-list constructor
    xl::list l4 = {1, 2, 3, 4, 5};
    assert(l4.size() == 5 && l4.back() == 5);

    // iterator-range constructor
    int arr[] = {1, 2, 3, 4, 5};
    xl::list l5(std::begin(arr), std::end(arr));
    assert(l5.size() == 5 && l5.front() == 1 && l5.back() == 5);

    // empty iterator range
    std::vector<int> ev;
    xl::list el(ev.begin(), ev.end());
    assert(el.empty());

    // single-pointer range
    int single = 42;
    xl::list sl(&single, &single + 1);
    assert(sl.size() == 1 && sl.front() == 42);

    // from_range tag + C array
    xl::list l6(xl::from_range, arr);
    assert(l6.size() == 5 && l6.front() == 1 && l6.back() == 5);

    // from_range + string_view (palindrome detection, odd length)
    xl::list pal(xl::from_range, std::string_view("racecar"));
    while (pal.size() > 1) {
      assert(pal.front() == pal.back());
      pal.pop_front(); pal.pop_back();
    }
    assert(pal.size() == 1);

    xl::list pal2(xl::from_range, std::string_view("abcba"));
    while (pal2.size() > 1) {
      assert(pal2.front() == pal2.back());
      pal2.pop_front(); pal2.pop_back();
    }
    assert(pal2.size() == 1 && pal2.front() == 'c');

    // multi_t constructor
    xl::list lm{xl::multi, 1, 2, 3, 4, 5};
    assert(lm.size() == 5 && lm.front() == 1 && lm.back() == 5);

    // views::iota range constructor and range assignment
    xl::list l7(std::views::iota(0, 100));
    assert(l7.size() == 100);
    l7 = std::views::iota(0, 10);
    assert(l7.size() == 10);

    // istream_iterator range
    std::istringstream iss("1 2 3 4");
    xl::list il(std::istream_iterator<int>(iss), std::istream_iterator<int>{});
    assert(il.size() == 4 && il.back() == 4);

    // range-constructed list equals C array
    int const ca[10]{};
    assert(std::ranges::equal(ca, xl::list(ca)));

    // from std::vector iterator range
    std::vector src_vec = {10, 20, 30, 40, 50};
    xl::list from_vec(src_vec.begin(), src_vec.end());
    assert(std::ranges::equal(from_vec, src_vec));

    // copy constructor
    xl::list orig_copy = {1, 2, 3, 4, 5};
    xl::list copied(orig_copy);
    assert(copied.size() == 5 && copied.front() == 1 && copied.back() == 5);
    // copy is independent
    orig_copy.front() = 99;
    assert(copied.front() == 1);

    // copy of empty list is empty
    xl::list<int> empty_src;
    xl::list empty_copy(empty_src);
    assert(empty_copy.empty());

    // move constructor: source is emptied, values transferred
    xl::list move_src = {7, 8, 9};
    xl::list move_dst(std::move(move_src));
    assert(move_src.empty()); // NOLINT
    assert(move_dst.size() == 3 && move_dst.front() == 7 && move_dst.back() == 9);

    // move from empty list
    {
      xl::list<int> src;
      xl::list dst(std::move(src));
      assert(src.empty() && dst.empty()); // NOLINT
    }

    // move-only type (unique_ptr): construction and chained moves
    xl::list<std::unique_ptr<int>> pl;
    pl.push_back(std::make_unique<int>(42));
    pl.emplace_back(new int(100));
    assert(*pl.front() == 42);
    xl::list ml2 = std::move(pl);
    assert(pl.empty() && ml2.size() == 2); // NOLINT
    xl::list nl(std::move(ml2));
    assert(ml2.empty() && nl.size() == 2); // NOLINT

    // large-element type: move constructs without copying
    struct LargeData {
      std::array<int, 1024> buffer;
      explicit LargeData(int val) { buffer.fill(val); }
    };
    xl::list<LargeData> lsrc;
    lsrc.emplace_back(1); lsrc.emplace_back(2);
    xl::list<LargeData> ldst = std::move(lsrc);
    assert(lsrc.empty() && ldst.size() == 2);
    assert(ldst.front().buffer[0] == 1 && ldst.back().buffer[0] == 2);

    // std::string elements: push_back, copy, and move
    xl::list<std::string> strl;
    strl.push_back("hello"); strl.push_back("world");
    xl::list<std::string> strl2 = std::move(strl);
    assert(strl.empty() && strl2.size() == 2); // NOLINT
    strl = std::move(strl2);
    assert(strl2.empty() && strl.size() == 2); // NOLINT
    xl::list<std::string> strl3 = strl; // deep copy
    assert(strl3.size() == 2 && strl.size() == 2);

    // move-construct list of strings: strings are actually moved (not copied)
    xl::list<std::string> str_src;
    str_src.emplace_back("hello"); str_src.emplace_back("world");
    xl::list<std::string> str_dst = std::move(str_src);
    assert(str_src.empty());
    assert(str_dst.size() == 2 && str_dst.front() == "hello" && str_dst.back() == "world");
  }

  // ─── TC-03  Assignment and Swap ─────────────────────────────────────────────
  {
    // copy assignment
    xl::list l1 = {1, 2, 3, 4, 5};
    xl::list<int> l2;
    l2 = l1;
    assert(l2.size() == 5 && l2.front() == 1 && l2.back() == 5);
    // source unchanged
    assert(l1.size() == 5);

    // copy-assign to non-empty destination (destination fully replaced)
    xl::list dst3 = {100, 200, 300, 400};
    xl::list src3 = {1, 2};
    dst3 = src3;
    assert(dst3.size() == 2 && dst3.front() == 1 && dst3.back() == 2);
    assert(src3.size() == 2 && src3.front() == 1);

    // copy assignment from empty list
    {
      xl::list<int> src;
      xl::list dst = {1, 2, 3};
      dst = src;
      assert(dst.empty());
    }

    // self copy-assignment guard
    {
      xl::list l = {1, 2, 3, 4, 5};
      l = l; // NOLINT
      assert(l.size() == 5 && l.front() == 1 && l.back() == 5);
    }

    // move assignment: source is emptied, destination receives values
    xl::list mv_src = {7, 8, 9, 10};
    xl::list<int> mv_dst = {0};
    mv_dst = std::move(mv_src);
    assert(mv_src.empty()); // NOLINT
    assert(mv_dst.size() == 4 && mv_dst.front() == 7 && mv_dst.back() == 10);

    // self move-assignment: must not crash or corrupt memory
    {
      xl::list l5{11, 22, 33};
      auto& ref = l5;
      l5 = std::move(ref); // implementation-defined state; no crash/leak
    }

    // assign(initialiser_list) replaces content of empty and non-empty list
    xl::list<int> lst;
    lst.assign({7, 8, 9});
    assert(lst.size() == 3 && lst.front() == 7 && lst.back() == 9);
    lst.assign({4, 5, 6, 7});
    assert(lst.size() == 4 && lst.front() == 4 && lst.back() == 7);

    // operator=(initialiser_list)
    lst = {1, 2, 3, 4, 5};
    assert(lst.size() == 5 && lst.back() == 5);

    // assign(count, value)
    lst.assign(3, 200);
    assert(lst.size() == 3 && lst.front() == 200);
    for (auto v : lst) assert(v == 200);

    // assign(count=0, value) produces empty list
    lst.assign(0, 999);
    assert(lst.empty());

    // assign(count=1, value) produces single-element list
    lst.assign(1, 42);
    assert(lst.size() == 1 && lst.front() == 42 && lst.back() == 42);

    // assign(iter, iter) from array
    int arr[] = {7, 8, 9, 10};
    lst.assign(std::begin(arr), std::end(arr));
    assert(lst.size() == 4 && lst.front() == 7 && lst.back() == 10);

    // assign shrinks larger destination
    xl::list big(10, 5);
    big.assign(3, 1);
    assert(big.size() == 3);
    for (auto v : big) assert(v == 1);

    // assign grows smaller destination
    xl::list small(2, 5);
    small.assign(6, 7);
    assert(small.size() == 6);
    for (auto v : small) assert(v == 7);

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

    // self-swap
    {
      xl::list l = {1, 2, 3};
      l.swap(l); // NOLINT
      assert((l == xl::list<int>{1, 2, 3}));
    }

    // iterator stability: copy produces independent clone; move transfers ownership
    xl::list orig = {10, 20, 30, 40, 50};
    auto it_stab = std::next(orig.begin(), 2);
    assert(*it_stab == 30);
    xl::list copy2 = orig;
    *it_stab = 99;
    assert((orig == xl::list{10, 20, 99, 40, 50}));
    assert((copy2 == xl::list{10, 20, 30, 40, 50}));
    xl::list moved = std::move(orig);
    assert(orig.empty()); // NOLINT
    assert((moved == xl::list{10, 20, 99, 40, 50}));
  }

  // ─── TC-04  Element Access and Capacity ─────────────────────────────────────
  {
    xl::list<int> l;
    assert(l.empty() && l.size() == 0);

    l.push_back(1, 2, 3);
    assert(l.size() == 3 && l[0] == 1 && l[1] == 2 && l[2] == 3);
    l[1] = 42;
    assert(l[1] == 42);

    // front / back on a multi-element list
    assert(l.front() == 1 && l.back() == 3);

    // single-element list: front == back
    l.clear();
    l.push_back(42);
    assert(l.front() == l.back() && l.front() == 42);

    // at() and operator[] agree on all indices
    xl::list lst = {10, 20, 30, 40, 50};
    assert(lst.at(0) == 10 && lst.at(2) == 30 && lst.at(4) == 50);
    lst.at(2) = 99;
    assert(lst.at(2) == 99);
    assert(lst[0] == 10 && lst[4] == 50);
    for (std::size_t i = 0; i < lst.size(); ++i)
      assert(lst.at(i) == lst[i]);

    // const list: at(), operator[], front(), back(), empty(), cbegin/cend
    const xl::list clst = {1, 2, 3};
    assert(clst.at(0) == 1 && clst.at(2) == 3);
    assert(clst[0] == 1 && clst[1] == 2 && clst[2] == 3);

    // const list: size, front, back, empty, iteration
    const xl::list cl2 = {10, 20, 30};
    assert(cl2.size() == 3 && cl2.front() == 10 && cl2.back() == 30 && !cl2.empty());
    int sum = 0;
    for (auto it = cl2.cbegin(); it != cl2.cend(); ++it) sum += *it;
    assert(sum == 60);

    // max_size is positive for all instantiations
    assert(xl::list<int>{}.max_size() > 0);
    assert(xl::list<std::string>{}.max_size() > 0);
  }

  // ─── TC-05  Push, Pop, and Emplace ──────────────────────────────────────────
  {
    // variadic push_back
    xl::list<int> l;
    l.push_back(1, 2, 3);
    assert(l.size() == 3 && l[0] == 1 && l[1] == 2 && l[2] == 3);

    // push_front
    l.push_front(100);
    assert(l.front() == 100 && l.back() == 3);

    // combined variadic push_front / push_back, check order
    l.clear();
    l.push_back(1, 2, 3);
    l.push_front(0);
    assert(l.size() == 4 && (l == std::array{0, 1, 2, 3}));

    // pop_back / pop_front
    l.clear();
    l.push_back(1, 2, 3);
    l.pop_back();  assert(l.size() == 2 && l.back()  == 2);
    l.pop_front(); assert(l.size() == 1 && l.front() == 2);
    l.pop_back();  assert(l.empty());

    // alternating push_front / push_back preserves structural integrity;
    // forward and reverse traversal yield mirrors
    {
      xl::list<int> alt;
      for (int i = 1; i <= 5; ++i) {
        alt.push_back(i);
        alt.push_front(-i);
      }
      assert(alt.size() == 10 && alt.front() == -5 && alt.back() == 5);
      std::vector<int> fwd, bwd;
      for (auto v : alt) fwd.push_back(v);
      for (auto it = alt.rbegin(); it != alt.rend(); ++it) bwd.push_back(*it);
      assert(fwd.size() == bwd.size());
      for (std::size_t i = 0; i < fwd.size(); ++i)
        assert(fwd[i] == bwd[fwd.size() - 1 - i]);
    }

    // emplace_back / emplace_front return reference to new element
    xl::list<int> li;
    auto& rb = li.emplace_back(10);  assert(rb == 10 && li.back()  == 10);
    auto& rf = li.emplace_front(5);  assert(rf == 5  && li.front() == 5);

    // emplace(pos) returns iterator to new element
    auto eit = li.emplace(std::next(li.begin()), 7);
    assert(*eit == 7 && li.size() == 3);
    assert((li == std::array{5, 7, 10}));

    // emplace with multi-argument constructor
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

    // emplace_back / emplace_front with pair (multi-arg in-place construction)
    xl::list<std::pair<int, int>> pairlist;
    pairlist.emplace_back(1, 2); pairlist.emplace_front(3, 4);
    auto pit = pairlist.begin(); ++pit;
    pit = pairlist.emplace(pit, 5, 6);
    assert(pairlist.size()  == 3);
    assert(pairlist.front() == std::pair(3,4));
    assert(pairlist.back()  == std::pair(1,2));
    assert(*pit             == std::pair(5,6));

    // emplace at begin / end / middle with std::string
    xl::list<std::string> sl = {"world"};
    sl.emplace(sl.begin(), "hello");
    sl.emplace(sl.end(),   "!");
    assert(sl.size() == 3 && sl.front() == "hello" && sl.back() == "!");
    auto sit = sl.begin(); ++sit;
    sit = sl.emplace(sit, "there");
    assert(*sit == "there");
    assert((sl == xl::list<std::string>{"hello", "there", "world", "!"}));

    // non-default-constructible, non-copyable type can be emplaced
    struct NoDef {
      int x;
      explicit NoDef(int v) : x(v) {}
      NoDef(const NoDef&) = delete;
      NoDef(NoDef&&) = default;
      NoDef& operator=(NoDef&&) = default;
    };
    xl::list<NoDef> ndl;
    ndl.emplace_back(1);
    ndl.emplace_back(3);
    ndl.emplace_front(0);
    ndl.emplace(std::next(ndl.begin(), 2), 2); // insert 2 before 3
    assert(ndl.size() == 4);
    int expected_nd = 0;
    for (const auto& v : ndl) assert(v.x == expected_nd++);
  }

  // ─── TC-06  Insert and Erase ─────────────────────────────────────────────────
  {
    // single insert: returns iterator to inserted element
    xl::list<int> l;
    l.push_back(4);
    auto it = l.insert(std::next(l.begin()), 5);
    assert(l.size() == 2 && *it == 5);

    // erase single: returns iterator to successor
    it = l.erase(l.begin());
    assert(l.size() == 1 && *it == 5);

    // insert at begin / end / middle (single value)
    xl::list myList = {1, 2, 3, 4, 5};
    myList.insert(myList.begin(), 0);
    assert(myList.front() == 0);
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), 6);
    assert(myList.back() == 6);
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), 10);
    assert(myList[2] == 10);

    // insert single element into empty list
    {
      xl::list<int> empty_l;
      auto ins = empty_l.insert(empty_l.end(), 42);
      assert(empty_l.size() == 1 && *ins == 42);
      assert(empty_l.front() == 42 && empty_l.back() == 42);
    }

    // returned iterator from single insert points to the inserted element
    {
      xl::list<int> ri = {10, 30};
      auto ret_it = ri.insert(std::next(ri.begin()), 20);
      assert(*ret_it == 20);
      assert((ri == xl::list<int>{10, 20, 30}));
    }

    // insert(pos, initialiser_list) at begin / end / middle
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.begin(), {0, 0, 0});
    assert(myList.front() == 0 && myList.size() == 8);
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), {6, 6, 6});
    assert(myList.back() == 6 && myList.size() == 8);
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), {10, 10, 10});
    assert(myList[2] == 10 && myList.size() == 8);

    // insert(pos, initialiser_list) into empty list
    {
      xl::list<int> empty_l2;
      empty_l2.insert(empty_l2.end(), {1, 2, 3});
      assert((empty_l2 == xl::list<int>{1, 2, 3}));
    }

    // insert(pos, count, value) at begin, end, and middle
    xl::list lst = {1, 5};
    lst.insert(lst.begin(), 2, 0);
    assert(lst.size() == 4 && lst.front() == 0);
    lst.insert(lst.end(), 2, 9);
    assert(lst.size() == 6 && lst.back() == 9);
    {
      auto mid = std::next(lst.begin(), 3);
      lst.insert(mid, 3, 4);
      assert(lst.size() == 9);
    }

    // insert(pos, count=0, value) is a no-op
    {
      xl::list lst2 = {1, 2, 3};
      auto it2 = std::next(lst2.begin());
      lst2.insert(it2, 0, 99);
      assert(lst2.size() == 3 && (lst2 == xl::list<int>{1, 2, 3}));
    }

    // insert(pos, count, value) with count > 1 in middle
    {
      xl::list lst3 = {1, 2, 3};
      lst3.insert(std::next(lst3.begin()), 3, 99);
      assert(lst3.size() == 6 && lst3.front() == 1);
      auto it3 = std::next(lst3.begin());
      assert(*it3 == 99); ++it3; assert(*it3 == 99); ++it3;
      assert(*it3 == 99); ++it3; assert(*it3 == 2);
    }

    // insert(pos, count, value) into empty list
    {
      xl::list<int> l_empty;
      l_empty.insert(l_empty.begin(), 5, 42);
      assert(l_empty.size() == 5);
    }

    // insert(pos, iter, iter): iterator-range overload
    {
      xl::list lst4 = {1, 5};
      int mid4[] = {2, 3, 4};
      lst4.insert(std::next(lst4.begin()), std::begin(mid4), std::end(mid4));
      assert(lst4.size() == 5 && std::is_sorted(lst4.begin(), lst4.end()));
    }

    // insert(pos, iter, iter) at end
    {
      xl::list lend = {1, 2};
      int arr2[] = {3, 4, 5};
      lend.insert(lend.end(), std::begin(arr2), std::end(arr2));
      assert((lend == xl::list{1, 2, 3, 4, 5}));
    }

    // insert(pos, iter, iter) empty range is a no-op
    {
      xl::list<int> ins_noop = {1, 2, 3};
      std::vector<int> empty_src2;
      ins_noop.insert(ins_noop.begin(), empty_src2.begin(), empty_src2.end());
      assert((ins_noop == xl::list<int>{1, 2, 3}));
    }

    // variadic insert via multi_t: at middle, begin, and end
    {
      xl::list lvi = {1, 5};
      auto pos = std::next(lvi.begin());
      lvi.insert(xl::multi, pos, 2, 3, 4);
      assert(lvi.size() == 5 && (lvi == xl::list{1, 2, 3, 4, 5}));

      xl::list vi_begin = {4, 5};
      vi_begin.insert(xl::multi, vi_begin.begin(), 1, 2, 3);
      assert((vi_begin == xl::list{1, 2, 3, 4, 5}));

      xl::list vi_end = {1, 2};
      vi_end.insert(xl::multi, vi_end.end(), 3, 4, 5);
      assert((vi_end == xl::list{1, 2, 3, 4, 5}));

      xl::list vi_single = {1, 3};
      vi_single.insert(xl::multi, std::next(vi_single.begin()), 2);
      assert((vi_single == xl::list{1, 2, 3}));
    }

    // erase single element from middle; returned iterator is successor
    {
      xl::list<int> em = {1, 2, 3};
      auto ret = em.erase(std::next(em.begin()));
      assert(*ret == 3 && (em == xl::list<int>{1, 3}));
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

    // erase last then push_back preserves sequence
    {
      xl::list el2 = {1, 2, 3};
      el2.erase(std::prev(el2.end()));
      el2.push_back(99);
      int vals[] = {1, 2, 99}; int vi = 0;
      for (auto v : el2) assert(v == vals[vi++]);
    }

    // erase two elements in sequence (non-adjacent)
    {
      xl::list l2{1, 2, 3, 4, 5};
      auto it3 = l2.begin();
      l2.erase(std::next(it3, 2));
      l2.erase(it3);
      assert(l2.size() == 3 && l2.front() == 2 && l2.back() == 5);
    }

    // erase(begin, begin) empty range is a no-op; returns begin
    {
      xl::list lst_era = {1, 2, 3};
      auto ret = lst_era.erase(lst_era.begin(), lst_era.begin());
      assert(lst_era.size() == 3 && ret == lst_era.begin());
    }

    // erase range: interior; returned iterator points to element after range
    {
      xl::list re = {1, 2, 3, 4, 5};
      auto b2 = std::next(re.begin());
      auto e2 = std::next(b2, 2);
      auto r2 = re.erase(b2, e2);
      assert(*r2 == 4 && re.size() == 3 && (re == xl::list<int>{1, 4, 5}));
    }

    // erase range that spans multiple elements, verify endpoints and return
    {
      xl::list mid = {1, 2, 3, 4, 5, 6, 7};
      auto first = std::next(mid.begin(), 2);
      auto last  = std::next(first, 3);
      auto ret = mid.erase(first, last);
      assert(mid.size() == 4 && mid.front() == 1 && mid.back() == 7 && *ret == 6);
    }

    // erase range leaving only first and last
    {
      xl::list er2 = {1, 2, 3, 4, 5};
      er2.erase(std::next(er2.begin()), std::prev(er2.end()));
      assert((er2 == xl::list<int>{1, 5}));
    }

    // erase range spanning entire list; result is empty, return is end
    {
      xl::list all = {1, 2, 3, 4, 5};
      auto ret = all.erase(all.begin(), all.end());
      assert(all.empty() && ret == all.end());
    }

    // clear
    l.clear();
    assert(l.empty() && l.size() == 0);
  }

  // ─── TC-07  Iterators, Sentinels, and Traversal ──────────────────────────────
  {
    // sequential forward walk
    {
      xl::list l = {0, 1, 2, 3, 4};
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

    // reverse iterators accumulate digits
    xl::list rdig = {1, 2, 3, 4, 5};
    int rsum = 0;
    for (auto ri = rdig.rbegin(); ri != rdig.rend(); ++ri)
      rsum = rsum * 10 + *ri;
    assert(rsum == 54321);

    // std::distance between const_iterators equals size()
    const xl::list clst = {1, 2, 3, 4, 5};
    assert(std::distance(clst.cbegin(), clst.cend()) == (std::ptrdiff_t)clst.size());

    // cbegin / cend on non-const list; compare with begin / end
    xl::list ncl = {10, 20, 30};
    int csum = 0;
    for (auto it = ncl.cbegin(); it != ncl.cend(); ++it) csum += *it;
    assert(csum == 60);
    assert(ncl.cbegin() == ncl.begin() && ncl.cend() == ncl.end());

    // decrement from end; increment back to end
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

    // after_begin / before_end sentinels
    xl::list sent = {1, 2, 3, 4, 5};
    assert(*sent.after_begin() == 2);
    assert(*sent.before_end()  == 5);

    // cafter_begin / cbefore_end on const list
    const xl::list csentl = {10, 20, 30};
    assert(*csentl.cafter_begin() == 20);
    assert(*csentl.cbefore_end()  == 30);

    // rafter_begin / rbefore_end: reverse sentinels
    auto rab = sent.rafter_begin(); assert(*rab == 4);
    auto rbe = sent.rbefore_end();  assert(*rbe == 1);

    // after_begin on two-element list equals before_end
    {
      xl::list two = {10, 20};
      assert(*two.after_begin() == 20);
      assert(*two.before_end()  == 20);
    }

    // insert at end does not invalidate existing iterators
    {
      xl::list lins{1, 2, 3};
      auto itm = lins.begin(); ++itm;
      lins.insert(lins.end(), 4);
      assert(lins.size() == 4 && *itm == 2);
    }

    // erase does not invalidate iterators not pointing to erased node
    {
      xl::list lera = {1, 2, 3, 4, 5};
      auto it1 = lera.begin(); ++it1;
      auto it2 = std::next(it1);
      lera.erase(it1);
      assert(*it2 == 3);
      auto it3 = lera.begin();
      lera.insert(std::next(it3), 10);
      assert(*it3 == 1 && *it2 == 3);
    }

    // iterator validity across erase, insert, and splice in sequence
    {
      xl::list lseq = {1, 2, 3, 4, 5};
      auto ia = lseq.begin();
      auto ib = std::next(ia);
      auto ic = std::next(ib);
      ic = lseq.erase(ib);
      assert(*ia == 1 && *ic == 3);
      auto id = lseq.insert(ic, 10);
      assert(*ic == 3 && *id == 10);
      xl::list<int> lst3 = {20, 30};
      lseq.splice(ic = std::next(id), lst3);
      assert(*ic == 3 && lseq.size() == 7);
    }

    // saved iterators remain valid after erase of a different middle element
    {
      xl::list lstv = {1, 2, 3, 4, 5};
      std::vector<decltype(lstv.begin())> iters;
      for (auto i = lstv.begin(); i != lstv.end(); ++i) iters.push_back(i);
      lstv.erase(std::next(lstv.begin(), 2));
      assert(*iters[0] == 1 && *iters[1] == 2);
      assert(*iters[3] == 4 && *iters[4] == 5);
    }

    // iterator dereference via arrow operator
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
      auto it_b = it_a++;
      assert(*it_b == 10 && *it_a == 20);
      auto it_c = it_a--;
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

    // iterator bool conversion: valid node is truthy, end sentinel is falsy
    {
      xl::list ib = {1, 2};
      auto it_ib = ib.begin();
      assert(it_ib);
      it_ib = ib.end();
      assert(!it_ib);
    }

    // clear then re-populate: fresh iterators work correctly
    {
      xl::list relst = {1, 2, 3};
      relst.clear();
      assert(relst.empty());
      relst.push_back(7, 8, 9);
      assert(relst.size() == 3 && relst.front() == 7 && relst.back() == 9);
      int chk = 7;
      for (auto v : relst) assert(v == chk++);
    }
  }

  // ─── TC-08  resize and reverse ────────────────────────────────────────────────
  {
    // --- resize ---
    xl::list l = {1, 2, 3, 4, 5};
    l.resize(3); assert(l.size() == 3);
    l.resize(3); assert(l.size() == 3);  // same size is a no-op
    l.resize(5); assert(l.size() == 5);
    l.resize(8, 10); assert(l.size() == 8);

    // grow with default value appends zeros
    {
      xl::list<int> l2;
      l2.push_back(1); l2.push_back(2);
      l2.resize(3);
      assert(l2.size() == 3 && l2.front() == 1 && l2.back() == 0);
      assert(l2.at(1) == 2);
    }

    // grow with explicit fill value
    {
      xl::list l3 = {1, 2, 3};
      l3.resize(5, 100);
      assert(l3.size() == 5 && l3.back() == 100);
    }

    // resize to 0 produces empty list
    {
      xl::list l4 = {1, 2, 3, 4, 5};
      l4.resize(0);
      assert(l4.empty());
    }

    // resize empty list to non-zero
    {
      xl::list<int> re;
      re.resize(4, 7);
      assert(re.size() == 4);
      for (auto v : re) assert(v == 7);
    }

    // resize(1) on three-element list: only front survives
    {
      xl::list r1 = {10, 20, 30};
      r1.resize(1);
      assert(r1.size() == 1 && r1.front() == 10 && r1.back() == 10);
    }

    // shrink–grow cycle: shrunken prefix intact, grown suffix filled correctly
    {
      xl::list rsg(10, 5);
      rsg.resize(3);
      rsg.resize(8, 9);
      assert(rsg.size() == 8);
      auto it_rsg = rsg.begin();
      for (int i = 0; i < 3; ++i, ++it_rsg) assert(*it_rsg == 5);
      for (int i = 0; i < 5; ++i, ++it_rsg) assert(*it_rsg == 9);
    }

    // resize with non-trivial type: destructor is called on removed elements
    {
      static int live = 0;
      struct Counted {
        Counted()              { ++live; }
        Counted(Counted&&)     { ++live; }
        Counted(const Counted&){ ++live; }
        ~Counted()             { --live; }
      };
      {
        xl::list<Counted> lc(6);
        assert(live == 6);
        lc.resize(3);
        assert(live == 3);
        lc.resize(5);
        assert(live == 5);
      }
      assert(live == 0);
    }

    // --- reverse ---
    {
      xl::list lst{1, 2, 3, 4, 5};
      lst.reverse();
      assert((lst == std::array{5, 4, 3, 2, 1}));

      // double reverse is identity
      lst.reverse(); lst.reverse();
      assert((lst == std::array{5, 4, 3, 2, 1}));

      // std::ranges::reverse also works
      xl::list rng{1, 2, 3, 4, 5};
      std::ranges::reverse(rng);
      assert((rng == std::array{5, 4, 3, 2, 1}));

      // reverse empty list: no-op
      xl::list<int> empty_rev; empty_rev.reverse(); assert(empty_rev.empty());

      // reverse single-element list: no-op
      xl::list one_rev = {42}; one_rev.reverse();
      assert(one_rev.size() == 1 && one_rev.front() == 42);

      // reverse two-element list
      xl::list two_rev = {1, 2}; two_rev.reverse();
      assert(two_rev.front() == 2 && two_rev.back() == 1);

      // reverse then iterate: verify all original elements in opposite order
      xl::list orig_rev = {10, 20, 30, 40, 50};
      std::vector before_rev(orig_rev.begin(), orig_rev.end());
      orig_rev.reverse();
      std::vector after_rev(orig_rev.begin(), orig_rev.end());
      for (std::size_t i = 0; i < before_rev.size(); ++i)
        assert(before_rev[i] == after_rev[before_rev.size() - 1 - i]);
    }
  }

  // ─── TC-09  sort ─────────────────────────────────────────────────────────────
  {
    // sort empty list is a no-op
    xl::list<int> empty_srt; empty_srt.sort();

    // sort single-element list
    xl::list<int> single_srt = {5}; single_srt.sort();
    assert(single_srt.size() == 1 && single_srt.front() == 5);

    // two-element list: already sorted and reverse sorted
    xl::list<int> two_asc = {1, 2}; two_asc.sort();
    assert((two_asc == xl::list<int>{1, 2}));
    xl::list<int> two_desc = {2, 1}; two_desc.sort();
    assert((two_desc == xl::list<int>{1, 2}));

    // two equal elements: all five variants leave list unchanged
    for (int v = 0; v < 5; ++v) {
      xl::list<int> teq = {3, 3};
      auto do_sort = [&](int i) {
        if      (i == 0) teq.sort<0>();
        else if (i == 1) teq.sort<1>();
        else if (i == 2) teq.sort<2>();
        else if (i == 3) teq.sort<3>();
        else             teq.sort<4>();
      };
      do_sort(v);
      assert(teq.size() == 2 && teq.front() == 3 && teq.back() == 3);
    }

    // basic ascending sort
    xl::list lst_srt = {5, 2, 8, 1, 9};
    assert(!std::is_sorted(lst_srt.begin(), lst_srt.end()));
    lst_srt.sort();
    assert(std::is_sorted(lst_srt.begin(), lst_srt.end()));

    // custom descending comparator (functor and lambda)
    xl::list desc_fn = {3, 1, 4, 2, 5};
    desc_fn.sort(std::greater<int>());
    assert((desc_fn == xl::list<int>{5, 4, 3, 2, 1}));
    xl::list desc_lam = {3, 1, 4, 2, 5};
    desc_lam.sort([](int a, int b){ return a > b; });
    assert((desc_lam == xl::list<int>{5, 4, 3, 2, 1}));

    // sort already-sorted list is idempotent
    xl::list asc_idem = {1, 2, 3, 4, 5};
    asc_idem.sort();
    assert((asc_idem == xl::list<int>{1, 2, 3, 4, 5}));

    // sort list of all identical elements preserves count
    {
      xl::list<int> dups(8, 3);
      dups.sort();
      for (auto v : dups) assert(v == 3);
      assert(dups.size() == 8);
    }

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
      struct StatefulCmp {
        int threshold;
        bool operator()(int a, int b) const {
          return (a > threshold) < (b > threshold);
        }
      };
      xl::list sfc = {10, 2, 8, 1, 6, 12};
      sfc.sort(StatefulCmp{5});
      auto it_sfc = sfc.begin();
      while (it_sfc != sfc.end() && *it_sfc <= 5) ++it_sfc;
      while (it_sfc != sfc.end()) { assert(*it_sfc > 5); ++it_sfc; }
    }

    // sort list of pairs by second field, then by first
    {
      xl::list<std::pair<int,int>> pairs = {{3,1},{1,3},{2,2},{1,1},{2,1}};
      auto pcmp = [](const auto& a, const auto& b){
        return a.second != b.second ? a.second < b.second : a.first < b.first;
      };
      pairs.sort(pcmp);
      assert(std::is_sorted(pairs.begin(), pairs.end(), pcmp));
    }

    // sort sub-range via iterator pair (middle three only; endpoints untouched)
    {
      xl::list sr = {5, 3, 1, 4, 2};
      auto b_sr = std::next(sr.begin());
      auto e_sr = std::next(b_sr, 3);
      sr.sort(b_sr, e_sr);
      assert(sr.front() == 5 && sr.back() == 2);
      int prev_v = std::numeric_limits<int>::min();
      auto it_sr = std::next(sr.begin()); int cnt = 0;
      while (it_sr != std::prev(sr.end())) { assert(*it_sr >= prev_v); prev_v = *it_sr; ++it_sr; ++cnt; }
      assert(cnt == 3);
    }

    // all five sort variants produce identical result on the same input
    {
      auto make = []() -> xl::list<int> { return {5,3,1,4,2}; };
      auto ref = make(); ref.sort();
      auto s1 = make(); s1.sort<1>();
      auto s2 = make(); s2.sort<2>();
      auto s3 = make(); s3.sort<3>();
      auto s4 = make(); s4.sort<4>();
      assert(s1 == ref && s2 == ref && s3 == ref && s4 == ref);
    }

    // all five variants agree on already-sorted and reverse-sorted input
    {
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

    // all five variants on all-same input (size 7)
    {
      for (int variant = 0; variant < 5; ++variant) {
        xl::list<int> s(7, 42);
        auto do_sort = [&](int v) {
          if      (v == 0) s.sort<0>();
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

    // sort<0..4> on empty list: all variants are no-ops
    { xl::list<int> z; z.sort<0>(); z.sort<1>(); z.sort<2>(); z.sort<3>(); z.sort<4>(); assert(z.empty()); }

    // sort stability: equal-key items retain relative insertion order
    {
      struct Item { int key, order; };
      auto check_stable = [](xl::list<Item> l) {
        l.sort([](const Item& a, const Item& b){ return a.key < b.key; });
        int last_key = -1, last_order_for_key = -1;
        for (auto it = l.begin(); it != l.end(); ++it) {
          if (it->key == last_key) assert(it->order > last_order_for_key);
          last_order_for_key = it->order;
          last_key = it->key;
        }
      };
      xl::list<Item> src;
      for (int i = 0; i < 20; ++i) src.push_back({i % 5, i});
      xl::list<Item> l0 = src; check_stable(l0);
      xl::list<Item> l1s = src; { l1s.sort<1>([](const Item& a, const Item& b){ return a.key < b.key; }); }
      xl::list<Item> l2s = src; { l2s.sort<2>([](const Item& a, const Item& b){ return a.key < b.key; }); }
      xl::list<Item> l3s = src; { l3s.sort<3>([](const Item& a, const Item& b){ return a.key < b.key; }); }
      xl::list<Item> l4s = src; { l4s.sort<4>([](const Item& a, const Item& b){ return a.key < b.key; }); }
    }

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

    // pseudorandom input size > bsize0 (exercises run-merging path)
    {
      std::mt19937 rng(42);
      xl::list<int> rand_lst;
      for (int i = 0; i < 200; ++i)
        rand_lst.push_back(static_cast<int>(rng() % 1000));
      rand_lst.sort();
      assert(std::is_sorted(rand_lst.begin(), rand_lst.end()));
      assert(rand_lst.size() == 200);
    }

    // parity check with std::list on the same input
    {
      xl::list xl_lst = {9, 7, 5, 3, 1, 2, 4, 6, 8};
      std::list std_lst(xl_lst.begin(), xl_lst.end());
      xl_lst.sort(); std_lst.sort();
      assert(std_lst == xl_lst);
    }

    // xl::sort free-function wrapper (full range and partial range)
    {
      xl::list lst_ff = {4, 2, 5, 1, 3};
      auto b_ff = lst_ff.cbegin(), e_ff = lst_ff.cend();
      xl::sort(lst_ff, b_ff, e_ff);
      assert(std::is_sorted(lst_ff.begin(), lst_ff.end()));
    }

    // xl::sort partial range (begin to before_end): last element excluded, all five variants
    { xl::list z = {3, 2, 1}; xl::sort(z,    z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
    { xl::list z = {3, 2, 1}; xl::sort<1>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
    { xl::list z = {3, 2, 1}; xl::sort<2>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
    { xl::list z = {3, 2, 1}; xl::sort<3>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
    { xl::list z = {3, 2, 1}; xl::sort<4>(z, z.begin(), z.before_end()); assert((std::array{2, 3, 1} == z)); }
  }

  // ─── TC-10  merge ─────────────────────────────────────────────────────────────
  {
    // both empty: no-op
    { xl::list<int> a, b; a.merge(b); assert(a.empty() && b.empty()); }

    // empty source into non-empty: source stays empty, destination unchanged
    {
      xl::list<int> a = {1, 2, 3}, b;
      a.merge(b);
      assert((a == std::array{1, 2, 3}) && b.empty());
    }

    // non-empty source into empty destination
    {
      xl::list<int> a, b = {4, 5, 6};
      a.merge(b);
      assert((a == std::array{4, 5, 6}) && b.empty());
    }

    // interleaved equal-sized lists
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

    // non-interleaved (all of b follows all of a)
    {
      xl::list a = {1, 2, 3, 4, 5}, b = {6, 7, 8, 9, 10};
      a.merge(std::move(b));
      assert(a.size() == 10 && a.front() == 1 && a.back() == 10);
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

    // multiple copies of same value in both lists
    {
      xl::list A = {1, 3, 3, 7}, B = {2, 3, 3, 8};
      A.sort(); B.sort();
      A.merge(B);
      assert(B.empty() && (A == std::array{1, 2, 3, 3, 3, 3, 7, 8}));
    }

    // shared values: size and endpoints correct
    {
      xl::list a = {1, 2, 2, 3}, b = {2, 4, 5};
      a.merge(b);
      assert(a.size() == 7 && a.front() == 1 && a.back() == 5);
    }

    // move-merge same-sized; check full element order
    {
      xl::list a = {1, 2}, b = {1, 2};
      a.merge(std::move(b));
      assert(a.size() == 4 && a.front() == 1 && a.back() == 2);
      assert(a.at(1) == 1 && a.at(2) == 2);
    }

    // merge of two single-element lists, both orderings
    {
      xl::list a1 = {1}, b1 = {2};
      a1.merge(b1);
      assert((a1 == xl::list<int>{1, 2}) && b1.empty());
      xl::list a2 = {2}, b2 = {1};
      a2.merge(b2);
      assert((a2 == xl::list<int>{1, 2}) && b2.empty());
    }

    // result is fully sorted
    {
      xl::list a = {2, 5, 8, 11}, b = {1, 3, 7, 9, 13};
      a.merge(b);
      assert(std::is_sorted(a.begin(), a.end()) && a.size() == 9 && b.empty());
    }

    // descending comparator (functor and lambda)
    {
      xl::list a = {5, 3, 1}, b = {6, 4, 2};
      a.merge(b, std::greater<int>());
      assert((a == std::array{6, 5, 4, 3, 2, 1}) && b.empty());
    }
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
      int last_p = 0;
      for (const auto& v : a) { assert(v.priority > last_p); last_p = v.priority; }
      assert(b.empty());
    }

    // chained merge of three lists
    {
      xl::list a = {1, 4, 7}, b = {2, 5, 8}, c = {3, 6, 9};
      a.merge(b); a.merge(c);
      assert(a.size() == 9 && std::is_sorted(a.begin(), a.end()));
      int expected = 1;
      for (const auto& v : a) assert(v == expected++);
    }

    // merge preserves stability: equal elements from *this precede those from other
    {
      struct Labeled {
        int val; char src;
        bool operator<(const Labeled& o) const { return val < o.val; }
      };
      xl::list<Labeled> a2 = {{1,'a'},{3,'a'},{5,'a'}}, b2 = {{2,'b'},{3,'b'},{4,'b'}};
      a2.merge(b2);
      assert(b2.empty());
      auto it_lbl = std::find_if(a2.begin(), a2.end(), [](const Labeled& l){ return l.val == 3; });
      assert(it_lbl != a2.end() && it_lbl->src == 'a');
    }

    // merge with custom comparator by struct field
    {
      struct Person { int id; std::string name; };
      xl::list<Person> alpha = {{1,"A"},{4,"D"}};
      xl::list<Person> beta  = {{2,"B"},{3,"C"}};
      auto cmp = [](const Person& a, const Person& b){ return a.id < b.id; };
      alpha.sort(cmp); beta.sort(cmp);
      alpha.merge(beta, cmp);
      assert(beta.empty() && alpha.size() == 4);
      int idx = 1;
      for (const auto& p : alpha) assert(p.id == idx++);
    }
  }

  // ─── TC-11  splice ────────────────────────────────────────────────────────────
  {
    // whole-list splice at beginning
    {
      xl::list a = {1, 2, 3, 4, 5};
      xl::list b = {10, 20, 30};
      a.splice(a.begin(), std::move(b));
      assert((a == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}) && b.empty()); // NOLINT
    }

    // splice empty-list source is a no-op
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

    // splice entire list into empty destination
    {
      xl::list<int> dst;
      xl::list src = {1, 2, 3, 4, 5};
      dst.splice(dst.end(), src);
      assert(src.empty() && (dst == xl::list<int>{1, 2, 3, 4, 5}));
    }

    // single-element splice to front of destination
    {
      xl::list a = {1, 2, 3, 4, 5}, b = {6, 7, 8, 9, 10};
      auto it_b = b.begin(); std::advance(it_b, 2);
      a.splice(a.begin(), b, it_b);
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

    // single-element splice from two-element source; verify source integrity
    {
      xl::list s2 = {10, 20};
      xl::list<int> d2;
      d2.splice(d2.end(), s2, s2.begin());
      assert(s2.size() == 1 && s2.front() == 20 && s2.back() == 20);
      assert(d2.size() == 1 && d2.front() == 10);
    }

    // range splice: moves first three elements of b to front of a
    {
      xl::list a = {1, 2, 3, 4, 5}, b = {6, 7, 8, 9, 10};
      auto it1 = b.begin(), it2 = it1;
      std::advance(it2, 3);
      a.splice(a.begin(), b, it1, it2);
      assert(a.size() == 8 && b.size() == 2 && a.front() == 6);
    }

    // range splice then iterate both lists for consistency
    {
      xl::list src3 = {1, 2, 3, 4, 5};
      xl::list dst3 = {10, 20};
      auto s_mid = std::next(src3.begin(), 1);
      auto s_end = std::next(s_mid, 3);
      dst3.splice(dst3.end(), src3, s_mid, s_end);
      assert(src3.size() == 2 && dst3.size() == 5);
      assert((src3 == xl::list<int>{1, 5}));
      assert((dst3 == xl::list<int>{10, 20, 2, 3, 4}));
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
      auto it_ss = lst.begin(); std::advance(it_ss, 2);
      lst.splice(lst.begin(), lst, it_ss, lst.end());
      assert((lst == xl::list<int>{3, 4, 5, 1, 2}));
    }

    // self-splice: identity cases (inserting before current position leaves list unchanged)
    {
      xl::list lst = {1, 2, 3, 4, 5};
      lst.splice(lst.begin(), lst, lst.begin());
      assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
      lst.splice(std::next(lst.begin()), lst, lst.begin());
      assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
      lst.splice(lst.end(), lst, lst.begin(), lst.end());
      assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
    }

    // self-splice: move single element within list and to front
    {
      xl::list lst = {1, 2, 4, 5, 3};
      auto it_sm = lst.begin(); std::advance(it_sm, 2);
      lst.splice(it_sm, lst, std::prev(lst.end())); // move 3 before 4
      assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
      lst.splice(lst.begin(), lst, std::prev(lst.end())); // move 5 to front
      assert((lst == xl::list<int>{5, 1, 2, 3, 4}));
    }

    // rotate via splice (forward and back)
    {
      xl::list lst = {1, 2, 3, 4, 5};
      auto it_rot = std::next(lst.begin(), 2);
      lst.splice(lst.end(), lst, lst.begin(), it_rot);
      assert((lst == xl::list<int>{3, 4, 5, 1, 2}));
      it_rot = std::next(lst.begin(), 3);
      lst.splice(lst.begin(), lst, it_rot, lst.end());
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

  // ─── TC-12  unique, remove / remove_if, and iter_swap ─────────────────────────
  {
    // --- unique ---
    xl::list<int> ml;
    assert(ml.unique() == 0);                   // empty: returns 0

    ml = {1, 1, 1};          assert(ml.unique() == 2);
    ml = {1, 2, 3};          assert(!ml.unique());
    ml = {1, 1, 2, 2, 3};    assert(ml.unique() == 2);
    ml = {1, 1, 2, 2, 3, 4}; assert(ml.unique() == 2);
    ml = {1, 2, 1, 1, 3, 3, 3, 4, 5, 4}; assert(ml.unique() == 3);

    // single-element list: nothing removed
    xl::list one_u = {42}; assert(one_u.unique() == 0 && one_u.size() == 1);

    // two-element list: same and different
    xl::list same_u = {7, 7}; assert(same_u.unique() == 1 && same_u.size() == 1);
    xl::list diff_u = {3, 4}; assert(diff_u.unique() == 0 && diff_u.size() == 2);

    // default predicate: adjacent equal elements removed
    xl::list def_u = {1, 2, 2, 3, 4, 4, 4, 5};
    auto removed_u = def_u.unique();
    assert(removed_u > 0 && (def_u == std::array{1, 2, 3, 4, 5}));

    // sort + unique removes non-consecutive duplicates
    xl::list su = {1, 2, 1, 3, 3, 2, 4};
    su.sort(); su.unique();
    assert((su == std::array{1, 2, 3, 4}));

    // unique does not reorder elements (first of each run survives)
    {
      xl::list ordered = {3, 3, 1, 1, 2, 2};
      ordered.unique();
      assert((ordered == xl::list<int>{3, 1, 2}));
    }

    // unique returns correct count on longer mixed sequence
    {
      xl::list mixed = {1, 1, 2, 3, 3, 3, 4, 4, 5};
      assert(mixed.unique() == 4);
      assert((mixed == xl::list<int>{1, 2, 3, 4, 5}));
    }

    // all-identical elements: single survivor
    {
      xl::list all_same(10, 5);
      all_same.unique();
      assert(all_same.size() == 1 && all_same.front() == 5);
    }

    // custom predicate: collapse elements within distance 1
    {
      xl::list cp = {1, 2, 4, 5, 7, 8};
      cp.unique([](int a, int b){ return std::abs(a - b) <= 1; });
      assert(cp.size() < 6);
    }

    // binary predicate: case-insensitive deduplication on sorted strings
    {
      xl::list<std::string> sl_u = {"a", "A", "b", "B", "c"};
      sl_u.sort([](const std::string& x, const std::string& y){
        return std::tolower(x[0]) < std::tolower(y[0]); });
      sl_u.unique([](const std::string& x, const std::string& y){
        return std::tolower(x[0]) == std::tolower(y[0]); });
      assert(sl_u.size() == 3);
    }

    // empty list with custom predicate: no crash, returns 0
    {
      xl::list<int> empty_u;
      assert(empty_u.unique([](int,int){ return true; }) == 0);
    }

    // --- remove / remove_if ---
    // remove non-existing value is a no-op
    xl::list lst_r = {1, 2, 3};
    lst_r.remove(42); assert(lst_r.size() == 3);

    // remove returns count of removed elements; removes all matching
    xl::list lr = {1, 2, 3, 2, 4, 2};
    auto n_r = lr.remove(2);
    assert(n_r == 3 && lr.size() == 3 && (lr == xl::list<int>{1, 3, 4}));
    assert(lr.remove(99) == 0 && lr.size() == 3);

    // remove first and last element simultaneously
    {
      xl::list<int> rfl = {9, 1, 2, 3, 9};
      rfl.remove(9);
      assert((rfl == xl::list{1, 2, 3}));
    }

    // remove from single-element list when value matches → empty
    {
      xl::list one_r = {42};
      assert(one_r.remove(42) == 1 && one_r.empty());
    }

    // remove from single-element list when value doesn't match → unchanged
    {
      xl::list one_r2 = {42};
      assert(one_r2.remove(99) == 0 && one_r2.size() == 1 && one_r2.front() == 42);
    }

    // remove from empty list: no crash, returns 0
    {
      xl::list<int> empty_r;
      assert(empty_r.remove(42) == 0);
    }

    // remove_if: keep only odd numbers
    xl::list lodd = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    lodd.remove_if([](int x){ return x % 2 == 0; });
    assert(lodd.size() == 5);
    for (const auto& v : lodd) assert(v % 2 == 1);

    // remove_if returns count of removed elements; predicate returning false is a no-op
    xl::list lrf = {1, 2, 3, 4, 5, 6};
    assert(lrf.remove_if([](int x){ return x % 2 == 0; }) == 3 && lrf.size() == 3);
    assert(lrf.remove_if([](int){ return false; }) == 0);

    // remove_if all elements: list becomes empty
    {
      xl::list lall = {2, 4, 6, 8, 10};
      lall.remove_if([](int x){ return x % 2 == 0; });
      assert(lall.empty());
    }

    // remove_if leaving only one element
    {
      xl::list rif = {1, 2, 3, 4, 5};
      rif.remove_if([](int x){ return x != 3; });
      assert(rif.size() == 1 && rif.front() == 3 && rif.back() == 3);
    }

    // remove_if from empty list: no crash, returns 0
    {
      xl::list<int> empty_rif;
      assert(empty_rif.remove_if([](int){ return true; }) == 0);
    }

    // --- iter_swap ---
    // adjacent elements
    xl::list lst_sw = {1, 2, 3, 4, 5};
    auto sw1 = lst_sw.begin();
    auto sw2 = std::next(sw1);
    lst_sw.iter_swap(sw1, sw2);
    assert((lst_sw == xl::list<int>{2, 1, 3, 4, 5}));

    // non-adjacent: first and last
    lst_sw.iter_swap(lst_sw.begin(), std::prev(lst_sw.end()));
    assert(lst_sw.front() == 5 && lst_sw.back() == 2);

    // same iterator: no-op
    auto mid_sw = std::next(lst_sw.begin(), 2);
    int val_before = *mid_sw;
    lst_sw.iter_swap(mid_sw, mid_sw);
    assert(*mid_sw == val_before);

    // iter_swap on two-element list swaps front and back
    {
      xl::list two_sw = {10, 20};
      two_sw.iter_swap(two_sw.begin(), std::prev(two_sw.end()));
      assert(two_sw.front() == 20 && two_sw.back() == 10);
    }

    // iter_swap: values exchanged, iterators remain at same nodes
    {
      xl::list isw = {1, 2, 3};
      auto ia = isw.begin();
      auto ib = std::next(ia, 2);
      isw.iter_swap(ia, ib);
      assert(*ia == 3 && *ib == 1);
      assert(isw.front() == 3 && isw.back() == 1);
    }
  }

  // ─── TC-13  Range Operations ─────────────────────────────────────────────────
  {
    // append_range / prepend_range / insert_range with C array
    xl::list lst_rng{1, 2, 3};
    int const vec[]{4, 5, 6};
    lst_rng.append_range(vec);
    assert((lst_rng == std::array{1, 2, 3, 4, 5, 6}));
    lst_rng.prepend_range(vec);
    assert((lst_rng == std::array{4, 5, 6, 1, 2, 3, 4, 5, 6}));
    auto it_rng = lst_rng.begin(); std::advance(it_rng, 3);
    lst_rng.insert_range(it_rng, vec);
    assert((lst_rng == std::array{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));

    // same operations with xl::list sources
    xl::list ml{1,2,3}, al{4,5,6}, pl{7,8,9}, il{10,11,12};
    ml.append_range(al);   assert((ml == xl::list{1, 2, 3, 4, 5, 6}));
    ml.prepend_range(pl);  assert((ml == xl::list{7, 8, 9, 1, 2, 3, 4, 5, 6}));
    auto pos_rng = std::find(ml.begin(), ml.end(), 1);
    ml.insert_range(pos_rng, il);
    assert((ml == xl::list{7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6}));
    ml.assign_range(xl::list{13, 14, 15});
    assert((ml == xl::list{13, 14, 15}));

    // assign_range from C array
    int const ra[]{6, 7, 8, 9, 10};
    xl::list<int> lst2_rng;
    lst2_rng.assign_range(ra);
    assert(lst2_rng.size() == std::size(ra));
    auto li_rng = lst2_rng.begin(); auto ai_rng = std::begin(ra);
    while (li_rng) assert(*li_rng++ == *ai_rng++);

    // operator=(range) replaces content
    lst2_rng = ra;
    assert(lst2_rng.size() == std::size(ra));

    // assign_range from xl::list
    xl::list src_rng = {13, 14, 15};
    lst2_rng.assign_range(src_rng);
    assert((lst2_rng == std::array{13, 14, 15}));

    // move-assign array into xl::list (strings moved, originals emptied)
    std::string sa[]{"A", "B", "C"};
    xl::list<std::string> sb_rng;
    assert(std::ranges::none_of(sa, [](auto const& s){ return s.empty(); }));
    sb_rng = std::move(sa);
    assert(std::ranges::all_of(sa,  [](auto const& s){ return s.empty(); }));
    assert(std::ranges::none_of(sb_rng, [](auto const& s){ return s.empty(); }));
    sb_rng.assign_range({"a", {"b"}, {"c"}});
    assert(sb_rng.size() == 3);

    // append_range with rvalue range (move semantics)
    {
      xl::list<std::string> dest_mv;
      xl::list<std::string> src_mv = {"x", "y", "z"};
      dest_mv.append_range(std::move(src_mv));
      assert(dest_mv.size() == 3 && dest_mv.front() == "x");
    }

    // append_range / prepend_range to empty list
    {
      xl::list<int> ae;
      int src_arr[] = {1, 2, 3};
      ae.append_range(src_arr);
      assert((ae == xl::list{1, 2, 3}));
    }
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

    // self append_range / prepend_range / insert_range
    {
      xl::list l_self{1, 2, 3};
      l_self.append_range(l_self);
      assert((l_self == std::array{1, 2, 3, 1, 2, 3}));
    }
    {
      xl::list l_self{1, 2, 3};
      l_self.prepend_range(l_self);
      assert((l_self == std::array{1, 2, 3, 1, 2, 3}));
    }
    {
      xl::list l_self{1, 2};
      l_self.insert_range(l_self.cbegin(), l_self);
      assert((l_self == std::array{1, 2, 1, 2}));
      l_self = {1, 2};
      l_self.insert_range(l_self.cend(), l_self);
      assert((l_self == std::array{1, 2, 1, 2}));
      l_self = {1, 2};
      l_self.insert_range(l_self.cafter_begin(), l_self);
      assert((l_self == std::array{1, 1, 2, 2}));
    }
  }

  // ─── TC-14  Comparison Operators and Free-Function Algorithms ────────────────
  {
    // equality, inequality, ordering
    xl::list a={1,2,3}, b={1,2,3}, c={1,2,4}, d={1,2};
    assert(a==b && a!=c && a!=d && d<a && c>a && d<=a && a>=b && c>=a);

    // heterogeneous comparison (int vs long)
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

    // three-way comparison / spaceship operator
    {
      xl::list x = {1, 2, 3};
      xl::list y = {1, 2, 3};
      xl::list z = {1, 2, 4};
      assert((x <=> y) == std::strong_ordering::equal);
      assert((x <=> z) == std::strong_ordering::less);
      assert((z <=> x) == std::strong_ordering::greater);
    }

    // erase-remove idiom with std::remove / std::remove_if
    {
      xl::list lst_cmp = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      lst_cmp.erase(std::remove(lst_cmp.begin(), lst_cmp.end(), 2), lst_cmp.end());
      assert(std::find(lst_cmp.begin(), lst_cmp.end(), 2) == lst_cmp.end());
      assert(!xl::find(lst_cmp, 2));
      lst_cmp.erase(std::remove_if(lst_cmp.begin(), lst_cmp.end(),
        [](int i){ return i % 2 == 0; }), lst_cmp.end());
      assert(!xl::find_if(lst_cmp, [](int i){ return i % 2 == 0; }));
    }

    // xl::erase / xl::erase_if
    {
      xl::list lst2_cmp = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      xl::erase(lst2_cmp, 2);
      assert(!xl::find(lst2_cmp, 2));
      xl::erase_if(lst2_cmp, [](int i){ return i % 2 == 0; });
      assert(!xl::find_if(lst2_cmp, [](int i){ return i % 2 == 0; }));
    }

    // chained xl::erase and xl::erase_if
    {
      xl::list lst3_cmp = {1, 2, 3, 4, 5, 3, 6, 3, 7};
      xl::erase(lst3_cmp, 3);
      xl::erase_if(lst3_cmp, [](int v){ return v > 4; });
      assert(!xl::find(lst3_cmp, 3));
      assert(std::none_of(lst3_cmp.begin(), lst3_cmp.end(), [](int v){ return v > 4; }));
    }

    // xl::erase_if returns correct removal count and leaves correct remainder
    {
      xl::list lst4_cmp = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      auto removed_cmp = xl::erase_if(lst4_cmp, [](int x){ return x % 2 == 0; });
      assert(removed_cmp == 5 && lst4_cmp.size() == 5);
      for (const auto& v : lst4_cmp) assert(v % 2 != 0);
      assert(xl::erase_if(lst4_cmp, [](int){ return false; }) == 0);
    }

    // xl::find: returns mutable iterator allowing modification
    {
      xl::list lstf{3, 1, 2};
      *xl::find(lstf, 3) = 1;
      assert(lstf.front() == 1);
      xl::erase(lstf, 3, 2, 1);
      assert(lstf.empty());
    }

    // xl::find on const list
    {
      const xl::list clst_find = {10, 20, 30, 40};
      auto cit = xl::find(clst_find, 30);
      assert(cit && *cit == 30);
      assert(!xl::find(clst_find, 99));
    }

    // xl::find_if on empty list returns falsy iterator
    {
      xl::list<int> ef_find;
      assert(!xl::find_if(ef_find, [](int){ return true; }));
    }

    // xl::erase on empty list is a no-op
    {
      xl::list<int> ef2_find;
      xl::erase(ef2_find, 42);
      assert(ef2_find.empty());
    }

    // xl::find with initialiser list argument
    { xl::list z = {1, 2, 3}; assert(xl::find(z, {1})); }

    // xl::find with variadic multi-value arguments (all values present)
    { xl::list z = {1, 2, 3}; assert(xl::find(z, 1, 2, 3)); }
  }

  // ─── TC-15  Standard Algorithms Interoperability ─────────────────────────────
  {
    xl::list lst_alg = {1, 2, 3, 4, 5};

    // std::accumulate
    assert(std::accumulate(lst_alg.begin(), lst_alg.end(), 0) == 15);

    // std::transform in-place
    std::transform(lst_alg.begin(), lst_alg.end(), lst_alg.begin(), [](int x){ return x * 2; });
    assert(lst_alg.front() == 2 && lst_alg.back() == 10);

    // std::count_if
    assert(std::count_if(lst_alg.begin(), lst_alg.end(), [](int x){ return x % 2 == 0; }) == 5);

    // std::copy into list via std::back_inserter
    std::vector src_alg = {1, 2, 3, 4, 5};
    xl::list<int> dst_alg;
    std::copy(src_alg.begin(), src_alg.end(), std::back_inserter(dst_alg));
    assert(dst_alg.size() == 5 && std::equal(src_alg.begin(), src_alg.end(), dst_alg.begin()));

    // std::fill modifies all elements in-place
    xl::list fill_lst = {1, 2, 3, 4, 5};
    std::fill(fill_lst.begin(), fill_lst.end(), 0);
    for (const auto& v : fill_lst) assert(v == 0);

    // std::is_sorted after sort
    xl::list ul_alg = {5, 2, 8, 1, 9};
    ul_alg.sort();
    assert(std::is_sorted(ul_alg.begin(), ul_alg.end()));

    // std::for_each accumulates into external variable
    {
      xl::list fe = {1, 2, 3, 4, 5};
      int total = 0;
      std::for_each(fe.begin(), fe.end(), [&](int v){ total += v; });
      assert(total == 15);
    }

    // std::copy into vector via std::back_inserter, then compare
    {
      xl::list src2_alg = {10, 20, 30};
      std::vector<int> dst2_alg;
      std::copy(src2_alg.begin(), src2_alg.end(), std::back_inserter(dst2_alg));
      assert((dst2_alg == std::vector{10, 20, 30}));
    }

    // std::find on an xl::list
    {
      xl::list sf = {5, 3, 8, 1, 4};
      auto fit = std::find(sf.begin(), sf.end(), 8);
      assert(fit != sf.end() && *fit == 8);
      assert(std::find(sf.begin(), sf.end(), 99) == sf.end());
    }

    // std::rotate (bidirectional iterator required)
    {
      xl::list rot = {1, 2, 3, 4, 5};
      auto mid_rot = std::next(rot.begin(), 2);
      std::rotate(rot.begin(), mid_rot, rot.end());
      assert((rot == xl::list{3, 4, 5, 1, 2}));
    }

    // std::reverse_copy from xl::list into vector
    {
      xl::list rc = {1, 2, 3, 4, 5};
      std::vector<int> rv;
      std::reverse_copy(rc.begin(), rc.end(), std::back_inserter(rv));
      assert((rv == std::vector{5, 4, 3, 2, 1}));
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

  // ─── TC-16  Edge Cases: Empty, Single-Element, and Two-Element Lists ─────────
  {
    // --- empty list: all safe operations are no-ops or return zero ---
    {
      xl::list<int> e;
      assert(e.empty() && e.size() == 0);
      e.sort();    assert(e.empty());
      e.reverse(); assert(e.empty());
      assert(e.unique() == 0);
      assert(e.remove(42) == 0);
      assert(e.remove_if([](int){ return true; }) == 0);
      assert(e.erase(e.begin(), e.end()) == e.end());
      xl::list<int> e2;
      e.merge(e2);
      assert(e.empty() && e2.empty());
    }

    // --- single-element list ---
    {
      xl::list<int> lst;
      lst.push_back(42);
      assert(!lst.empty() && lst.size() == 1 && lst.front() == 42 && lst.back() == 42);
      lst.sort();    assert(lst.size() == 1);
      lst.reverse(); assert(lst.size() == 1 && lst.front() == 42);
      lst.unique();  assert(lst.size() == 1);
      lst.remove(99);              assert(lst.size() == 1);
      lst.remove_if([](int x){ return x == 99; }); assert(lst.size() == 1);

      lst.pop_front(); assert(lst.empty());
      lst.push_front(100); lst.pop_back(); assert(lst.empty());
      lst.push_back(200); lst.erase(lst.begin()); assert(lst.empty());

      // identity splice on single-element list is a no-op
      lst.push_back(7);
      lst.splice(lst.begin(), lst, lst.begin());
      assert(lst.size() == 1 && lst.front() == 7);

      // merge with empty other: unchanged
      {
        xl::list one_m = {5};
        xl::list<int> empty_other;
        one_m.merge(empty_other);
        assert(one_m.size() == 1 && one_m.front() == 5);
      }

      // copy-assign one single-element list to another
      {
        xl::list a = {1};
        xl::list b = {2};
        a = b;
        assert(a.size() == 1 && a.front() == 2 && b.front() == 2);
      }

      // emplace_back on empty list: front == back
      {
        xl::list<std::string> es;
        auto& ref = es.emplace_back("only");
        assert(ref == "only" && es.size() == 1 && es.front() == "only" && es.back() == "only");
      }

      // emplace_front on empty list: same node is front and back
      {
        xl::list<int> ef_edge;
        ef_edge.emplace_front(77);
        assert(ef_edge.size() == 1 && ef_edge.front() == 77 && ef_edge.back() == 77);
      }

      // emplace into single-element list at begin and at end
      {
        xl::list<int> sel = {5};
        sel.emplace(sel.begin(), 3);
        sel.emplace(sel.end(),   7);
        assert((sel == xl::list{3, 5, 7}));
      }
    }

    // --- two-element list ---
    {
      // insert at middle
      {
        xl::list t = {1, 3};
        t.insert(std::next(t.begin()), 2);
        assert((t == xl::list{1, 2, 3}));
      }

      // erase front / back
      {
        xl::list t = {1, 2};
        t.erase(t.begin());
        assert(t.size() == 1 && t.front() == 2 && t.back() == 2);
      }
      {
        xl::list t = {1, 2};
        t.erase(std::prev(t.end()));
        assert(t.size() == 1 && t.front() == 1 && t.back() == 1);
      }

      // sort both orderings
      {
        xl::list t1 = {2, 1}; t1.sort();
        assert(t1.front() == 1 && t1.back() == 2);
        xl::list t2 = {1, 2}; t2.sort();
        assert(t2.front() == 1 && t2.back() == 2);
      }

      // reverse
      {
        xl::list t = {1, 2}; t.reverse();
        assert(t.front() == 2 && t.back() == 1);
      }

      // bidirectional traversal
      {
        xl::list t = {10, 20};
        auto it_t = t.begin();
        assert(*it_t == 10); ++it_t;
        assert(*it_t == 20); ++it_t;
        assert(it_t == t.end());
        --it_t; assert(*it_t == 20);
        --it_t; assert(*it_t == 10);
      }
    }

    // --- reducing to exactly one element ---
    {
      xl::list lst2 = {1, 2, 3};
      lst2.pop_back();  assert(lst2.size() == 2 && lst2.back() == 2);
      lst2.pop_front(); assert(lst2.size() == 1 && lst2.front() == 2 && lst2.back() == 2);

      // multiple alternating pops converge to one element
      xl::list alt2 = {1, 2, 3, 4, 5, 6};
      while (alt2.size() > 1) {
        alt2.pop_front();
        if (alt2.size() > 1) alt2.pop_back();
      }
      assert(alt2.size() == 1 && alt2.front() == alt2.back());
    }
  }

  // ─── TC-17  Complex and Nested Value Types ───────────────────────────────────
  {
    // nested list: push, iterate, flatten
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

    // copy nested list: inner lists are deeply copied (independent)
    {
      xl::list<xl::list<int>> orig_n = {{1,2},{3,4},{5,6}};
      xl::list<xl::list<int>> copy_n = orig_n;
      assert(copy_n.size() == 3);
      orig_n.front().front() = 99;
      assert(copy_n.front().front() == 1);
    }

    // move nested list: source is emptied, inner lists transferred
    {
      xl::list<xl::list<int>> m1 = {{1,2,3},{4,5,6}};
      xl::list<xl::list<int>> m2 = std::move(m1);
      assert(m1.empty());
      assert(m2.size() == 2 && m2.front().size() == 3);
    }

    // sort nested list by inner list size
    {
      xl::list<xl::list<int>> sn = {{1,2,3},{4},{5,6}};
      sn.sort([](const xl::list<int>& a, const xl::list<int>& b){ return a.size() < b.size(); });
      assert(sn.front().size() == 1 && sn.back().size() == 3);
    }
  }

  // ─── TC-18  Exception Safety ─────────────────────────────────────────────────
  {
    // push_back where constructor throws: list remains unchanged
    {
      struct TestException : std::exception {};
      struct Thrower {
        Thrower() = default;
        Thrower(int) { throw TestException(); }
      };
      xl::list<Thrower> lst_ex;
      lst_ex.push_back(Thrower{});
      try {
        lst_ex.push_back(42);
        assert(false && "exception not thrown");
      } catch (const TestException&) {
        assert(lst_ex.size() == 1);
      }
    }

    // insert where copy constructor throws: list remains unchanged
    {
      struct ThrowOnCopy {
        int value;
        ThrowOnCopy(int v) : value(v) {}
        ThrowOnCopy(const ThrowOnCopy& o) : value(o.value) {
          if (value == 42) throw std::runtime_error("copy failed");
        }
      };
      xl::list<ThrowOnCopy> lst_ex2;
      lst_ex2.emplace_back(1); lst_ex2.emplace_back(2);
      try {
        lst_ex2.insert(lst_ex2.begin(), ThrowOnCopy(42));
        assert(false && "exception not thrown");
      } catch (const std::runtime_error&) {
        assert(lst_ex2.size() == 2 && lst_ex2.front().value == 1);
      }
    }

    // emplace_back where constructor throws: list unchanged, no partial node
    {
      struct ThrowOnN {
        int v;
        ThrowOnN(int n) : v(n) { if (n == 3) throw std::runtime_error("boom"); }
      };
      xl::list<ThrowOnN> lst_ex3;
      lst_ex3.emplace_back(1); lst_ex3.emplace_back(2);
      try {
        lst_ex3.emplace_back(3);
        assert(false);
      } catch (const std::runtime_error&) {
        assert(lst_ex3.size() == 2 && lst_ex3.front().v == 1 && lst_ex3.back().v == 2);
      }
    }

    // copy-throwing type leaves list unchanged on push_back
    {
      static int throw_cnt;
      throw_cnt = 0;
      struct ThrowOnCopy2 {
        int val{};
        ThrowOnCopy2() = default;
        explicit ThrowOnCopy2(int v) : val(v) {}
        ThrowOnCopy2(const ThrowOnCopy2&) {
          if (++throw_cnt == 2) throw std::bad_alloc();
        }
      };
      xl::list<ThrowOnCopy2> z_ex;
      z_ex.emplace_back(1); z_ex.emplace_back(2);
      bool caught_ex = false;
      try {
        z_ex.push_back(ThrowOnCopy2(3));
      } catch (const std::bad_alloc&) {
        caught_ex = true;
      }
      assert(z_ex.size() == 2 && caught_ex);
    }
  }

  // ─── TC-19  Resource Management and Destructor Balance ───────────────────────
  {
    static int counter = 0;
    struct Counted {
      Counted()               { ++counter; }
      Counted(const Counted&) { ++counter; }
      ~Counted()              { --counter; }
    };

    // destructor called on scope exit
    counter = 0;
    {
      xl::list<Counted> lst_cnt;
      lst_cnt.push_back(Counted()); lst_cnt.push_back(Counted());
      assert(counter == 2);
    }
    assert(counter == 0);

    // move: elements transferred, counter unchanged; both destroyed at scope end
    counter = 0;
    {
      xl::list<Counted> lst_cnt;
      lst_cnt.push_back(Counted()); lst_cnt.push_back(Counted());
      xl::list<Counted> lst_cnt2 = std::move(lst_cnt);
      assert(counter == 2);
    }
    assert(counter == 0);

    // clear() calls destructors and resets counter to 0
    counter = 0;
    {
      xl::list<Counted> lst_cnt;
      for (int i = 0; i < 5; ++i) lst_cnt.emplace_back();
      assert(counter == 5);
      lst_cnt.clear();
      assert(counter == 0 && lst_cnt.empty());
    }

    // pop_back / pop_front each call exactly one destructor
    counter = 0;
    {
      xl::list<Counted> lst_cnt;
      lst_cnt.emplace_back(); lst_cnt.emplace_back(); lst_cnt.emplace_back();
      assert(counter == 3);
      lst_cnt.pop_back();  assert(counter == 2);
      lst_cnt.pop_front(); assert(counter == 1);
      lst_cnt.pop_back();  assert(counter == 0 && lst_cnt.empty());
    }

    // erase(range) decrements counter by exactly (range size)
    counter = 0;
    {
      xl::list<Counted> lst_cnt;
      for (int i = 0; i < 6; ++i) lst_cnt.emplace_back();
      assert(counter == 6);
      auto b2 = std::next(lst_cnt.begin());
      auto e2 = std::prev(lst_cnt.end());
      lst_cnt.erase(b2, e2); // removes 4 elements
      assert(counter == 2);
    }
    assert(counter == 0); // outer scope destructor destroys final 2
  }

  // ─── TC-20  Large-List Stress Tests ──────────────────────────────────────────
  {
    // 1M push_back via iota, then clear
    {
      constexpr int N = 1'000'000;
      xl::list lst_big(std::views::iota(0, N));
      assert(lst_big.size() == N && lst_big.front() == 0 && lst_big.back() == N - 1);
      lst_big.clear(); assert(lst_big.empty());
    }

    // push_back, front/back, and sum over 100k elements
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

    // xl::find, reverse, and sort on 50k element list
    {
      const std::size_t N = 50000;
      xl::list<std::size_t> lst_50k;
      for (std::size_t i = 0; i < N; ++i) lst_50k.push_back(i);
      auto found = xl::find(lst_50k, N / 2);
      assert(found && *found == N / 2);
      lst_50k.reverse();
      assert(lst_50k.front() == N - 1 && lst_50k.back() == 0);
      lst_50k.sort();
      assert(lst_50k.front() == 0 && lst_50k.back() == N - 1);
    }

    // fragmented push/pop pattern preserves size and sortability
    {
      xl::list<int> lst_frag;
      for (int i = 0; i < 1000; ++i) {
        lst_frag.push_back(i);
        if (i % 3 == 0) lst_frag.pop_front();
      }
      assert(!lst_frag.empty());
      auto sz_frag = lst_frag.size();
      lst_frag.sort();
      assert(lst_frag.size() == sz_frag);
    }

    // build reversed copy via rbegin, then sort original
    {
      constexpr std::size_t N = 100'000;
      xl::list<std::size_t> large_rev;
      for (std::size_t i = 0; i < N; ++i) large_rev.push_back(i);
      xl::list<std::size_t> reversed_cpy;
      for (auto it = large_rev.rbegin(); it != large_rev.rend(); ++it)
        reversed_cpy.push_back(*it);
      assert(reversed_cpy.front() == N - 1);
      large_rev.sort();
      assert(large_rev.front() == 0 && large_rev.back() == N - 1);
    }

    // sort + unique on many duplicates
    {
      xl::list<int> lst_dup;
      for (int i = 0; i < 1000; ++i) lst_dup.push_back(i % 10);
      lst_dup.sort(); lst_dup.unique();
      assert(lst_dup.size() == 10);
      int expected = 0;
      for (const auto v : lst_dup) assert(v == expected++);
    }

    // repeated push/pop cycles (allocator stress)
    {
      xl::list<int> lst_cycle;
      for (int cycle = 0; cycle < 100; ++cycle) {
        for (int i = 0; i < 10; ++i) lst_cycle.push_back(i);
        for (int i = 0; i <  5; ++i) lst_cycle.pop_back();
      }
      assert(lst_cycle.size() == 500);
      lst_cycle.clear(); assert(lst_cycle.empty());
    }

    // all five sort variants agree on pseudorandom input of size 500
    {
      std::mt19937 rng2(1234);
      std::vector<int> data;
      data.reserve(500);
      for (int i = 0; i < 500; ++i) data.push_back(int(rng2() % 10000));

      xl::list ref_l(data.begin(), data.end()); ref_l.sort<0>();
      xl::list l1_big(data.begin(), data.end()); l1_big.sort<1>();
      xl::list l2_big(data.begin(), data.end()); l2_big.sort<2>();
      xl::list l3_big(data.begin(), data.end()); l3_big.sort<3>();
      xl::list l4_big(data.begin(), data.end()); l4_big.sort<4>();
      assert(l1_big == ref_l && l2_big == ref_l && l3_big == ref_l && l4_big == ref_l);
    }

    // assign(1M, value): allocator and size tracking at scale
    {
      xl::list<int> z_big;
      z_big.assign(1'000'000, 42);
      assert(z_big.size() == 1'000'000);
      assert(std::all_of(z_big.begin(), z_big.end(), [](int v){ return v == 42; }));
      z_big.assign(0, 0);
      assert(z_big.empty());
    }
  }

  // ─── TC-21  Comprehensive Composition Smoke Test ─────────────────────────────
  {
    // exercise the majority of member functions in a single chained sequence
    xl::list<int> lst_smoke;
    lst_smoke.push_back(10, 20, 30); lst_smoke.push_front(5);
    lst_smoke.pop_back();  lst_smoke.pop_front();
    lst_smoke.insert(lst_smoke.begin(), 15); lst_smoke.erase(lst_smoke.begin());
    lst_smoke.clear(); lst_smoke.resize(5, 100);
    assert(lst_smoke.size() == 5 && lst_smoke.back() == 100);
    lst_smoke.assign(3, 200);
    assert(lst_smoke.size() == 3 && lst_smoke.front() == 200);
    xl::list lst2_smoke = {1, 2, 3};
    lst_smoke.splice(lst_smoke.begin(), lst2_smoke);
    assert(lst_smoke.size() == 6 && lst_smoke.front() == 1 && lst2_smoke.empty());
    lst_smoke.remove(1);
    lst_smoke.remove_if([](int i){ return i > 2; });
    lst_smoke.push_back(2); lst_smoke.unique();
    assert(lst_smoke.size() == 1 && lst_smoke.back() == 2);
    xl::list lst3_smoke = {1, 3};
    lst_smoke.merge(lst3_smoke);
    lst_smoke.sort(); lst_smoke.reverse();
    assert(lst_smoke.front() == 3 && lst_smoke.back() == 1);

    // split + merge restores sorted order
    xl::list smrg = {1, 3, 5, 2, 4, 6};
    xl::list<int> back2_smoke;
    auto mid_smoke = std::next(smrg.begin(), 3);
    back2_smoke.splice(back2_smoke.begin(), smrg, mid_smoke, smrg.end());
    smrg.merge(back2_smoke);
    assert(back2_smoke.empty() && smrg.size() == 6);
    assert(std::is_sorted(smrg.begin(), smrg.end()));

    // push / sort / splice / sort round-trip
    {
      xl::list a2 = {5, 1, 4, 2, 3};
      xl::list b2 = {8, 6, 7};
      a2.sort(); b2.sort();
      a2.splice(a2.end(), b2);
      a2.sort();
      assert(std::is_sorted(a2.begin(), a2.end()) && a2.size() == 8);
    }

    // interleaved push_back, erase evens, sort, reverse
    {
      xl::list<int> c2;
      for (int i = 10; i >= 1; --i) c2.push_back(i);
      for (auto it = c2.begin(); it != c2.end();) {
        if (*it % 2 == 0) it = c2.erase(it);
        else ++it;
      }
      assert(c2.size() == 5); // 1,3,5,7,9
      c2.sort(); c2.reverse();
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

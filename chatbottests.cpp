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
  // TC-01  push_back (variadic), size, operator[], iterator, insert, erase,
  //        swap, copy construction, range-based for
  {
    xl::list<int> container;
    container.push_back(1, 2, 3);
    assert(container.size() == 3);
    assert(container[0] == 1);
    assert(container[1] == 2);
    assert(container[2] == 3);
    container[1] = 42;
    assert(container[1] == 42);
    container.pop_back();
    assert(container.size() == 2);
    auto it = container.begin();
    assert(*it == 1);  ++it;
    assert(*it == 42); ++it;
    assert(it == container.end());
    container.clear();
    assert(container.empty());
    assert(container.size() == 0);
    container.push_back(4);
    container.insert(std::next(container.begin()), 5);
    assert(container.size() == 2);
    assert(container[0] == 4);
    assert(container[1] == 5);
    container.erase(container.begin());
    assert(container.size() == 1);
    assert(container[0] == 5);
    decltype(container) other;
    other.push_back(6, 7);
    container.swap(other);
    assert(container.size() == 2);
    assert(other.size() == 1);
    decltype(container) copied = container;
    assert(copied.size() == 2);
    assert(copied[0] == 6);
    int sum = 0;
    for (const auto& item : container) sum += item;
    assert(sum == 13);
  }

  // TC-02  front / back, insert-before-second, erase returning iterator
  {
    xl::list<int> l;
    l.push_back(1, 2, 3);
    assert(l.size() == 3 && !l.empty());
    assert(l.front() == 1 && l.back() == 3);
    auto it = l.begin(); ++it;
    it = l.insert(it, 4);
    assert(l.size() == 4 && *it == 4);
    it = l.begin(); ++it;
    it = l.erase(it);
    assert(l.size() == 3 && *it == 2);
    l.clear();
    assert(l.size() == 0 && l.empty());
  }

  // TC-03  push_front, insert at second position, pop_back / pop_front
  {
    xl::list<int> l;
    assert(l.empty());
    l.push_back(1);
    l.push_front(2);
    l.insert(++l.begin(), 3);
    assert(l.size() == 3 && !l.empty());
    auto it = l.begin();
    assert(*it == 2); ++it;
    assert(*it == 3); ++it;
    assert(*it == 1);
    l.pop_back();  assert(l.size() == 2);
    l.pop_front(); assert(l.size() == 1);
    l.clear();     assert(l.empty());
  }

  // TC-04  initialiser-list construction, iterator write, insert/erase
  {
    xl::list numbers = {1, 2, 3, 4, 5};
    auto it = numbers.begin();
    assert(*it == 1);
    *it = 10;
    it = numbers.insert(std::next(it), 20);
    assert(*it == 20);
    it = numbers.erase(it);
    assert(*it == 2);
    int sum = 0;
    for (const auto& n : numbers) sum += n;
    assert(sum == 24); // 10+2+3+4+5
  }

  // TC-05  multi_t constructor, push_front/back, pop_front/back, round-trips
  {
    xl::list l{xl::multi, 1, 2, 3, 4, 5};
    assert(l.size() == 5);
    l.push_front(0); assert(l.size() == 6 && l.front() == 0);
    l.push_back(6);  assert(l.size() == 7 && l.back()  == 6);
    l.pop_front();   assert(l.size() == 6 && l.front() == 1);
    l.pop_back();    assert(l.size() == 5 && l.back()  == 5);
    l.clear(); assert(l.empty());
    l.push_front(1); assert(l.front() == 1);
    l.pop_front();   assert(l.empty());
    l.push_back(6);  assert(l.back() == 6);
    l.pop_back();    assert(l.empty());
    l.insert(l.begin(), 2);
    assert(*l.begin() == 2);
    l.erase(l.begin());
    assert(l.size() == 0);
  }

  // TC-06  loop push_back / push_front, pop_back / pop_front, sequential walk
  {
    xl::list<int> l;
    for (int i = 0; i < 10; ++i) l.push_back(i);
    assert(l.back() == 9);
    l.push_front(100); assert(l.front() == 100);
    l.pop_back();      assert(l.back()  == 8);
    l.pop_front();     assert(l.front() == 0);
    int expected = 0;
    for (auto val : l) assert(val == expected++);
  }

  // TC-07  fill constructor (count, value)
  {
    xl::list l(5, 10);
    assert(l.size() == 5);
    for (const auto& e : l) assert(e == 10);
  }

  // TC-08  from_range + string_view; palindrome peel; sort empty
  {
    xl::list palindrome(xl::from_range, std::string_view("racecar"));
    while (palindrome.size() > 1) {
      assert(palindrome.front() == palindrome.back());
      palindrome.pop_front();
      palindrome.pop_back();
    }
    assert(palindrome.size() == 1);
    palindrome.sort();
  }

  // TC-09  from_range + views::iota; range assignment
  {
    xl::list l(std::views::iota(0, 100));
    assert(l.size() == 100);
    l = std::views::iota(0, 10);
    assert(l.size() == 10);
  }

  // TC-10  insert overloads: single at begin/end/position; ilist at begin/
  //          end/position
  {
    xl::list myList = {1, 2, 3, 4, 5};
    myList.insert(myList.begin(), 0);
    assert(myList.front() == 0);
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), 6);
    assert(myList.back() == 6);
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), 10);
    assert(myList[2] == 10);
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.begin(), {0, 0, 0});
    assert(myList.front() == 0 && myList.size() == 8);
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), {6, 6, 6});
    assert(myList.back() == 6 && myList.size() == 8);
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), {10, 10, 10});
    assert(myList[2] == 10 && myList.size() == 8);
  }

  // TC-11  unique: empty list, all-dup, no-dup, mixed
  {
    xl::list<int> my_list;
    assert(my_list.unique() == 0);
    my_list = {1, 1, 1};       assert(my_list.unique() == 2);
    my_list = {1, 2, 3};       assert(!my_list.unique());
    my_list = {1, 1, 2, 2, 3}; assert(my_list.unique() == 2);
    my_list = {1, 1, 2, 2, 3, 4}; assert(my_list.unique() == 2);
    my_list = {1, 2, 1, 1, 3, 3, 3, 4, 5, 4}; assert(my_list.unique() == 3);
  }

  // TC-12  resize: shrink, same size, grow (default), grow with value
  {
    xl::list l = {1, 2, 3, 4, 5};
    l.resize(3); assert(l.size() == 3);
    l.resize(3); assert(l.size() == 3);
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

  // TC-13  merge: non-interleaved (1-5 + 6-10), interleaved (1-5 + 2-6),
  //          both-empty, empty + non-empty, non-empty + empty, duplicates,
  //          different sizes, custom descending comparator
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
    {
      xl::list<int> a, b;
      a.merge(b);
      assert(a.empty() && b.empty());
    }
    // empty into non-empty (no-op for source)
    {
      xl::list<int> a = {1, 2, 3}, b;
      a.merge(b);
      assert((a == xl::list<int>{1, 2, 3}) && b.empty());
    }
    // non-empty into empty
    {
      xl::list<int> a, b = {4, 5, 6};
      a.merge(b);
      assert((a == xl::list<int>{4, 5, 6}) && b.empty());
    }
    // interleaved equal-sized
    {
      xl::list a = {1, 3, 5}, b = {2, 4, 6};
      a.merge(b);
      assert((a == xl::list<int>{1, 2, 3, 4, 5, 6}) && b.empty());
    }
    // different sizes
    {
      xl::list a = {1, 3, 5}, b = {2, 4};
      a.merge(b);
      assert((a == std::initializer_list<int>{1, 2, 3, 4, 5}) && b.empty());
    }
    // duplicates interleaved
    {
      xl::list a = {1, 2, 3}, b = {2, 3, 4};
      a.merge(b);
      assert((a == xl::list<int>{1, 2, 2, 3, 3, 4}) && b.empty());
    }
    // custom descending comparator
    {
      xl::list a = {5, 3, 1}, b = {6, 4, 2};
      a.merge(b, [](int x, int y){ return x > y; });
      assert((a == xl::list<int>{6, 5, 4, 3, 2, 1}) && b.empty());
    }
    // merge empty into sorted list with custom comparator (no-op)
    {
      xl::list<int> a = {5, 3, 1}, b;
      a.merge(b, [](int x, int y){ return x > y; });
      assert((a == xl::list<int>{5, 3, 1}) && b.empty());
    }
    // merge with partially overlapping ranges
    {
      xl::list a = {1, 3, 5}, b = {2, 3, 4, 6};
      a.merge(b);
      assert((a == std::array{1, 2, 3, 3, 4, 5, 6}) && b.empty());
    }
    // merge with shared values (stable ordering)
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
  }

  // TC-14  splice: all three overloads (whole list, single element, range);
  //          empty-source splices; self-splice
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
    // single-element splice
    {
      xl::list a = {1, 2, 3, 4, 5}, b = {6, 7, 8, 9, 10};
      auto it = b.begin(); std::advance(it, 2); // points to 8
      a.splice(a.begin(), b, it);
      assert(a.size() == 6 && b.size() == 4 && a.front() == 8);
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
    // splice between two lists then merge
    {
      xl::list a = {3, 5, 7}, b = {2, 4, 6};
      a.merge(b);
      assert(b.empty() && (a == xl::list<int>{2, 3, 4, 5, 6, 7}));
    }
  }

  // TC-15  append_range, prepend_range, insert_range (array source)
  {
    xl::list lst{1, 2, 3};
    int const vec[]{4, 5, 6};
    lst.append_range(vec);
    assert((lst == xl::list{1, 2, 3, 4, 5, 6}));
    lst.prepend_range(vec);
    assert((lst == xl::list{4, 5, 6, 1, 2, 3, 4, 5, 6}));
    auto it = lst.begin(); std::advance(it, 3);
    lst.insert_range(it, vec);
    assert((lst == xl::list{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));
  }

  // TC-16  assign_range (array and list sources)
  {
    xl::list lst = {1, 2, 3, 4, 5};
    int const arr[]{6, 7, 8, 9, 10};
    lst.assign_range(arr);
    assert(lst.size() == std::size(arr));
    auto li = lst.begin(); auto ai = std::begin(arr);
    while (li) assert(*li++ == *ai++);
    lst = arr;
    assert(lst.size() == std::size(arr));
  }

  // TC-17  append/prepend/insert/assign_range with xl::list sources
  {
    xl::list myList     = {1, 2, 3};
    xl::list appendList = {4, 5, 6};
    xl::list prependList= {7, 8, 9};
    xl::list insertList = {10, 11, 12};
    xl::list assignList = {13, 14, 15};
    myList.append_range(appendList);
    assert((myList == xl::list{1, 2, 3, 4, 5, 6}));
    myList.prepend_range(prependList);
    assert((myList == xl::list{7, 8, 9, 1, 2, 3, 4, 5, 6}));
    auto pos = std::find(myList.begin(), myList.end(), 1);
    myList.insert_range(pos, insertList);
    assert((myList == xl::list{7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6}));
    myList.assign_range(assignList);
    assert((myList == xl::list{13, 14, 15}));
  }

  // TC-18  emplace_back, emplace_front, pop_front/back, sort, copy
  {
    xl::list<int> l;
    assert(l.empty());
    l.sort(); // sort empty list is a no-op
    l.push_back(10, 20, 30);
    assert(l.size() == 3);
    l.pop_front();
    assert(l.size() == 2);
    auto it = l.begin();
    assert(*it == 20); ++it; assert(*it == 30);
    l.clear(); assert(l.empty());
    l.push_front(30, 20, 10);
    assert(l.size() == 3);
    l.pop_back();
    assert(l.size() == 2);
    it = l.begin();
    assert(*it == 30); ++it; assert(*it == 20);
    // emplace_back
    xl::list<int> l2;
    l2.emplace_back(10); l2.emplace_back(20); l2.emplace_back(30);
    assert(l2.size() == 3);
    it = l2.begin();
    assert(*it == 10); ++it; assert(*it == 20); ++it; assert(*it == 30);
    // copy construction
    xl::list<int> l3(l2);
    assert(l3.size() == 3);
    it = l3.begin();
    assert(*it == 10); ++it; assert(*it == 20); ++it; assert(*it == 30);
  }

  // TC-19  comprehensive single-block exercise of most member functions
  {
    xl::list<int> lst;
    lst.push_back(10, 20, 30);
    assert(lst.size() == 3 && lst.back() == 30);
    lst.push_front(5);
    assert(lst.size() == 4 && lst.front() == 5);
    lst.pop_back();  assert(lst.size() == 3 && lst.back()  == 20);
    lst.pop_front(); assert(lst.size() == 2 && lst.front() == 10);
    auto it = lst.begin();
    lst.insert(it, 15);
    assert(lst.size() == 3 && lst.front() == 15);
    it = lst.begin();
    lst.erase(it);
    assert(lst.size() == 2 && lst.front() == 10);
    lst.clear(); assert(lst.empty());
    lst.resize(5, 100);
    assert(lst.size() == 5 && lst.back() == 100);
    lst.assign(3, 200);
    assert(lst.size() == 3 && lst.front() == 200);
    xl::list<int> lst2 = {1, 2, 3};
    it = lst.begin();
    lst.splice(it, lst2);
    assert(lst.size() == 6 && lst.front() == 1 && lst2.empty());
    lst.remove(1);
    assert(lst.size() == 5 && lst.front() == 2);
    lst.remove_if([](int i){ return i > 2; });
    assert(lst.size() == 1 && lst.back() == 2);
    lst.push_back(2);
    lst.unique();
    assert(lst.size() == 1 && lst.back() == 2);
    xl::list<int> lst3 = {1, 3};
    lst.merge(lst3);
    assert(lst.size() == 3 && lst.front() == 1 && lst3.empty());
    lst.sort();
    assert(lst.front() == 1 && lst.back() == 3);
    lst.reverse();
    assert(lst.front() == 3 && lst.back() == 1);
  }

  // TC-20  erase-remove idiom
  {
    xl::list lst = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    lst.erase(std::remove(lst.begin(), lst.end(), 2), lst.end());
    assert(std::find(lst.begin(), lst.end(), 2) == lst.end());
    assert(!xl::find(lst, 2));
    lst.erase(std::remove_if(lst.begin(), lst.end(),
      [](int i){ return i % 2 == 0; }), lst.end());
    assert(std::find_if(lst.begin(), lst.end(),
      [](int i){ return i % 2 == 0; }) == lst.end());
    assert(!xl::find_if(lst, [](int i){ return i % 2 == 0; }));
  }

  // TC-21  xl::erase and xl::erase_if free functions
  {
    xl::list<int> lst = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    xl::erase(lst, 2);
    assert(std::ranges::find(lst, 2) == lst.end());
    assert(!xl::find(lst, 2));
    xl::erase_if(lst, [](int i){ return i % 2 == 0; });
    assert(std::ranges::find_if(lst, [](int i){ return i % 2 == 0; }) == lst.end());
    assert(!xl::find_if(lst, [](int i){ return i % 2 == 0; }));
    // verify no remaining matching values after chained erases
    xl::list<int> lst2 = {1, 2, 3, 4, 5, 3, 6, 3, 7};
    xl::erase(lst2, 3);
    xl::erase_if(lst2, [](int v){ return v > 4; });
    assert(std::find(lst2.begin(), lst2.end(), 3) == lst2.end());
    assert(!xl::find(lst2, 3));
    assert(std::none_of(lst2.begin(), lst2.end(), [](int v){ return v > 4; }));
  }

  // TC-22  push/pop/insert/erase in sequence; forward, range-based, reverse
  {
    xl::list<int> lst;
    assert(lst.empty());
    lst.push_back(1, 2, 3);
    assert(lst.size() == 3);
    assert(lst.front() == 1 && lst.back() == 3);
    lst.insert(lst.begin(), 0);
    lst.insert(lst.end(), 4);
    assert(lst.size() == 5);
    lst.pop_back();  assert(lst.size() == 4);
    lst.erase(lst.begin()); assert(lst.size() == 3);
    xl::list<int>::iterator it = lst.begin();
    assert(*it++ == 1); assert(*it++ == 2); assert(*it++ == 3);
    for (int i : lst) assert(i == 1 || i == 2 || i == 3);
    xl::list<int>::reverse_iterator rit = lst.rbegin();
    assert(*rit == 3); rit++; assert(*rit == 2); rit++; assert(*rit == 1);
    lst.clear(); assert(lst.empty());
  }

  // TC-23  constructors, copy/move assignment, swap, relational operators,
  //          sort, merge, splice, unique, reverse iterators
  {
    xl::list<int> l1; assert(l1.empty() && l1.size() == 0);
    xl::list<int> l2{1, 2, 3, 4, 5};
    assert(!l2.empty() && l2.size() == 5 && l2.front() == 1 && l2.back() == 5);
    // copy constructor
    xl::list<int> l3(l2);
    assert(!l3.empty() && l3.size() == 5 && l3.front() == 1 && l3.back() == 5);
    // move constructor
    xl::list<int> l4(std::move(l2));
    assert(l2.empty() && l2.size() == 0);
    assert(!l4.empty() && l4.size() == 5 && l4.front() == 1 && l4.back() == 5);
    // copy assignment
    l2 = l3;
    assert(!l2.empty() && l2.size() == 5 && l2.front() == 1 && l2.back() == 5);
    // move assignment
    l2 = std::move(l3);
    assert(l3.empty() && l3.size() == 0);
    assert(!l2.empty() && l2.size() == 5 && l2.front() == 1 && l2.back() == 5);
    l2.push_front(0); assert(l2.size() == 6 && l2.front() == 0 && l2.back() == 5);
    l2.push_back(6);  assert(l2.size() == 7 && l2.front() == 0 && l2.back() == 6);
    l2.pop_front();   assert(l2.size() == 6 && l2.front() == 1 && l2.back() == 6);
    l2.pop_back();    assert(l2.size() == 5 && l2.front() == 1 && l2.back() == 5);
    l2.insert(l2.begin(), 0);
    assert(l2.size() == 6 && l2.front() == 0 && l2.back() == 5);
    l2.erase(l2.begin());
    assert(l2.size() == 5 && l2.front() == 1 && l2.back() == 5);
    l2.clear(); assert(l2.empty() && l2.size() == 0);
    xl::list l5{1, 2, 3};
    l2.swap(l5);
    assert(l2.size() == 3 && l2.front() == 1 && l2.back() == 3);
    assert(l5.empty() && l5.size() == 0);
    assert(l2 > l3 && l2 >= l3 && l3 < l2 && l3 <= l2);
    assert(l2 == l2 && l2 != l3);
    xl::list l9 = {13, 14, 15, 16, 17};
    l9.reverse();
    assert(l9.size() == 5 && l9.front() == 17 && l9.back() == 13);
    xl::list l10 = {18, 15, 20, 12, 19};
    l10.sort();
    assert(l10.size() == 5 && l10.front() == 12 && l10.back() == 20);
    xl::list l11 = {21, 23, 25}, l12 = {22, 24, 26};
    l11.merge(l12);
    assert(l11.size() == 6 && l11.front() == 21 && l11.back() == 26);
    xl::list l13 = {27, 28, 29}, l14 = {30, 31, 32};
    l14.splice(l14.begin(), l13);
    assert(l14.size() == 6 && l14.front() == 27 && l14.back() == 32);
    xl::list l15 = {33, 34, 34, 35, 35, 36};
    l15.unique();
    assert(l15.size() == 4 && l15.front() == 33 && l15.back() == 36);
    xl::list l16 = {37, 38, 39, 40, 41};
    for (auto rit = l16.rbegin(); rit != l16.rend(); ++rit) assert(*rit > 0);
  }

  // TC-24  erase two elements in sequence
  {
    xl::list l{1, 2, 3, 4, 5};
    auto it = l.begin();
    l.erase(std::next(it, 2));
    l.erase(it);
    assert(l.size() == 3 && l.front() == 2 && l.back() == 5);
  }

  // TC-25  insert at end does not invalidate existing iterators
  {
    xl::list l{1, 2, 3};
    auto it = l.begin(); ++it;        // points to 2
    l.insert(l.end(), 4);
    assert(l.size() == 4 && *it == 2);
  }

  // TC-26  std::ranges::reverse
  {
    xl::list lst{1, 2, 3, 4, 5};
    std::ranges::reverse(lst);
    assert((lst == xl::list{5, 4, 3, 2, 1}));
  }

  // TC-27  iterator-range constructor and from_range constructor
  {
    int arr[] = {1, 2, 3, 4, 5};
    xl::list<int> l(std::begin(arr), std::end(arr));
    assert(l.size() == 5 && l.front() == 1 && l.back() == 5);
    xl::list l2(xl::from_range, arr);
    assert(l2.size() == 5 && l2.front() == 1 && l2.back() == 5);
  }

  // TC-28  max_size; std::is_sorted after sort
  {
    xl::list<int> lst;
    assert(lst.max_size() > 0);
    lst = {5, 2, 8, 1, 9};
    assert(!std::is_sorted(lst.begin(), lst.end()));
    lst.sort();
    assert(std::is_sorted(lst.begin(), lst.end()));
  }

  // TC-29  rotate via splice (forward and back)
  {
    xl::list lst = {1, 2, 3, 4, 5};
    auto it = std::next(lst.begin(), 2);
    lst.splice(lst.end(), lst, lst.begin(), it);
    assert((lst == xl::list<int>{3, 4, 5, 1, 2}));
    it = std::next(lst.begin(), 3);
    lst.splice(lst.begin(), lst, it, lst.end());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
  }

  // TC-30  nested list: construct, access, flatten
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

  // TC-31  independent lists: push then splice together
  {
    xl::list lst1 = {1, 2, 3}, lst2 = {4, 5, 6};
    lst1.push_back(7);  assert(lst1.back()  == 7);
    lst2.push_front(0); assert(lst2.front() == 0);
    lst1.splice(lst1.end(), std::move(lst2));
    assert(lst2.empty() && lst1.size() == 8);
  }

  // TC-32  std::accumulate, std::transform, std::count_if
  {
    xl::list lst = {1, 2, 3, 4, 5};
    auto sum = std::accumulate(lst.begin(), lst.end(), 0);
    assert(sum == 15);
    std::transform(lst.begin(), lst.end(), lst.begin(), [](int x){ return x * 2; });
    assert(lst.front() == 2 && lst.back() == 10);
    auto ec = std::count_if(lst.begin(), lst.end(), [](int x){ return x % 2 == 0; });
    assert(ec == 5);
  }

  // TC-33  replace contents using an iterator range (operator=)
  {
    xl::list lst = {1, 2, 3, 4, 5};
    int arr[] = {10, 20, 30};
    lst = arr;
    assert(lst == arr);
  }

  // TC-34  insert(pos, count, value): multi-element fill insert
  {
    xl::list lst = {1, 2, 3};
    lst.insert(std::next(lst.begin()), 3, 99);
    assert(lst.size() == 6 && lst.front() == 1);
    auto it = std::next(lst.begin());
    assert(*it == 99); ++it; assert(*it == 99); ++it;
    assert(*it == 99); ++it; assert(*it == 2);
  }

  // TC-35  bidirectional traversal: forward and backward yield mirror
  {
    xl::list lst = {10, 20, 30, 40, 50};
    std::vector<int> fwd, bwd;
    for (auto it = lst.begin();  it != lst.end();  ++it) fwd.push_back(*it);
    for (auto it = lst.rbegin(); it != lst.rend(); ++it) bwd.push_back(*it);
    assert(fwd.size() == bwd.size());
    for (std::size_t i = 0; i < fwd.size(); ++i)
      assert(fwd[i] == bwd[fwd.size() - 1 - i]);
  }

  // TC-36  chained operations: sort + unique + reverse
  {
    xl::list lst = {5, 1, 3, 1, 4, 2, 5, 3};
    lst.sort(); lst.unique(); lst.reverse();
    assert((lst == std::array{5, 4, 3, 2, 1}));
  }

  // TC-37  erase(begin, begin) is a no-op (empty range)
  {
    xl::list lst = {1, 2, 3};
    auto it = lst.erase(lst.begin(), lst.begin());
    assert(lst.size() == 3 && it == lst.begin());
  }

  // TC-38  std::distance between const_iterators equals size
  {
    const xl::list lst = {1, 2, 3, 4, 5};
    assert(std::distance(lst.cbegin(), lst.cend()) == (std::ptrdiff_t)lst.size());
  }

  // TC-39  push_back / push_front with variadic arguments
  {
    xl::list<int> lst;
    lst.push_back(1, 2, 3); lst.push_front(0);
    assert(lst.size() == 4 && lst.front() == 0 && lst.back() == 3);
    assert((lst == std::array{0, 1, 2, 3}));
  }

  // TC-40  splice from empty source into non-empty destination (range overload)
  {
    xl::list dst = {1, 2, 3};
    xl::list<int> src;
    dst.splice(dst.begin(), src, src.begin(), src.end());
    assert(dst.size() == 3 && src.empty());
  }

  // TC-41  large sort + std::find_if
  {
    xl::list<int> lst;
    for (int i = 99; i >= 0; --i) lst.push_back(i);
    lst.sort();
    auto it = std::find_if(lst.begin(), lst.end(), [](int x){ return x > 50; });
    assert(it && *it == 51);
    assert(std::is_sorted(lst.begin(), lst.end()));
  }

  // TC-42  range moving (move-assign array into xl::list)
  {
    std::string a[]{"A", "B", "C"};
    xl::list<std::string> b;
    assert(std::ranges::none_of(a, [](auto const& s){ return s.empty(); }));
    b = std::move(a);
    assert(std::ranges::all_of(a,  [](auto const& s){ return s.empty(); }));
    assert(std::ranges::none_of(b, [](auto const& s){ return s.empty(); }));
    b.assign_range({"a", {"b"}, {"c"}});
    assert(b.size() == 3);
  }

  // TC-43  Container concept: static_assert on type aliases / iterator
  {
    static_assert(std::is_same_v<xl::list<int>::value_type,      int>);
    static_assert(std::is_same_v<xl::list<int>::reference,       int&>);
    static_assert(std::is_same_v<xl::list<int>::const_reference, const int&>);
    static_assert(std::bidirectional_iterator<xl::list<int>::iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::const_iterator>);
  }

  // TC-44  SequenceContainer construction forms
  {
    xl::list<int> l1, l2(5), l3(5, 42), l4{1, 2, 3, 4, 5};
    assert(l1.empty());
    assert(l2.size() == 5);
    assert(l3.size() == 5 && l3.front() == 42);
    assert(l4.size() == 5 && l4.back()  == 5);
  }

  // TC-45  emplace_back / emplace_front / emplace at position (pair)
  {
    xl::list<std::pair<int, int>> lst;
    lst.emplace_back(1, 2); lst.emplace_front(3, 4);
    auto it = lst.begin(); ++it;
    it = lst.emplace(it, 5, 6);
    assert(lst.size() == 3);
    assert(lst.front() == std::pair(3,4));
    assert(lst.back()  == std::pair(1,2));
    assert(*it         == std::pair(5,6));
  }

  // TC-46  assign with initialiser list
  {
    xl::list lst = {1, 2, 3};
    lst.assign({4, 5, 6, 7});
    assert(lst.size() == 4 && lst.front() == 4 && lst.back() == 7);
  }

  // TC-47  sort with custom (descending) comparator
  {
    xl::list lst = {3, 1, 4, 2, 5};
    lst.sort(std::greater<int>());
    assert((lst == xl::list<int>{5, 4, 3, 2, 1}));
  }

  // TC-48  double reverse is identity
  {
    xl::list lst = {1, 2, 3, 4, 5};
    lst.reverse(); lst.reverse();
    assert((lst == std::array{1, 2, 3, 4, 5}));
  }

  // TC-49  reverse empty list; sort single-element list
  {
    xl::list<int> l;
    l.reverse();
    assert(l.empty());

    xl::list<int> single = {5};
    single.sort();
    assert(single.size() == 1 && single.front() == 5);
  }

  // TC-50  remove non-existing value is a no-op
  {
    xl::list lst = {1, 2, 3};
    lst.remove(42);
    assert(lst.size() == 3);
  }

  // TC-51  unique after sort removes non-consecutive duplicates
  {
    xl::list lst = {1, 2, 1, 3, 3, 2, 4};
    lst.sort(); lst.unique();
    assert((lst == std::array{1, 2, 3, 4}));
  }

  // TC-52  exception safety: push_back where constructor throws
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

  // TC-53  const iterator traversal
  {
    const xl::list lst = {1, 2, 3};
    int sum = 0;
    for (auto it = lst.cbegin(); it; ++it) sum += *it;
    assert(sum == 6);
  }

  // TC-54  large list: push_back, front/back, sum via iterator
  {
    xl::list<std::size_t> large;
    const std::size_t N = 100000;
    for (std::size_t i = 0; i < N; ++i) large.push_back(i);
    assert(large.size() == N && large.front() == 0 && large.back() == N - 1);
    std::size_t sum = 0;
    for (auto it = large.begin(); it != large.end(); ++it) sum += *it;
    assert(sum == (N - 1) * N / 2);
  }

  // TC-55  move semantics with large element type
  {
    struct LargeData {
      std::array<int, 1024> buffer;
      LargeData(int val) { buffer.fill(val); }
    };
    xl::list<LargeData> src;
    src.emplace_back(1); src.emplace_back(2);
    xl::list<LargeData> dst = std::move(src);
    assert(src.empty() && dst.size() == 2);
    assert(dst.front().buffer[0] == 1 && dst.back().buffer[0] == 2);
  }

  // TC-56  exception safety: insert where copy constructor throws
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

  // TC-57  stateful comparator for sort (partition by threshold)
  {
    struct StatefulComparator {
      int threshold;
      bool operator()(int a, int b) const {
        return (a > threshold) < (b > threshold);
      }
    };
    xl::list lst = {10, 2, 8, 1, 6, 12};
    lst.sort(StatefulComparator{5});
    auto it = lst.begin();
    while (it != lst.end() && *it <= 5) ++it;
    while (it != lst.end()) { assert(*it > 5); ++it; }
  }

  // TC-58  sort correctness: reverse-sorted 10 000 elements
  {
    xl::list<int> lst;
    const int N = 10000;
    for (int i = N; i > 0; --i) lst.push_back(i);
    lst.sort();
    assert(lst.front() == 1 && lst.back() == N);
    int expected = 1;
    for (const auto& v : lst) assert(v == expected++);
  }

  // TC-59  heterogeneous comparison (int vs long)
  {
    xl::list l1 = {1, 2, 3};
    xl::list<long> l2 = {1, 2, 3};
    xl::list l3 = {1, 2, 4};
    assert(l1 == l2 && l1 != l3 && l1 < l3 && l3 > l1);
  }

  // TC-60  emplace at begin, end, and middle with std::string
  {
    xl::list<std::string> lst = {"world"};
    lst.emplace(lst.begin(), "hello");
    lst.emplace(lst.end(),   "!");
    assert(lst.size() == 3 && lst.front() == "hello" && lst.back() == "!");
    auto it = lst.begin(); ++it;
    it = lst.emplace(it, "there");
    assert(*it == "there");
    assert((lst == xl::list<std::string>{"hello", "there", "world", "!"}));
  }

  // TC-61  initialiser-list assignment and assign()
  {
    xl::list<int> lst;
    lst = {1, 2, 3, 4, 5};
    assert(lst.size() == 5 && lst.back() == 5);
    lst.assign({6, 7, 8});
    assert(lst.size() == 3 && lst.front() == 6);
  }

  // TC-62  reverse iterators accumulate digits (54321)
  {
    xl::list lst = {1, 2, 3, 4, 5};
    int sum = 0;
    for (auto rit = lst.rbegin(); rit != lst.rend(); ++rit)
      sum = sum * 10 + *rit;
    assert(sum == 54321);
  }

  // TC-63  swap between two large lists
  {
    xl::list<int> la, lb;
    const int N = 100000;
    for (int i = 0; i < N; ++i) { la.push_back(i); lb.push_back(i * 2); }
    la.swap(lb);
    assert(la.front() == 0 && lb.front() == 0);
  }

  // TC-64  std::string list: push_back, move and copy semantics
  {
    xl::list<std::string> lst;
    lst.push_back("hello"); lst.push_back("world");
    assert(lst.size() == 2 && lst.front() == "hello" && lst.back() == "world");
    xl::list<std::string> lst2 = std::move(lst);
    assert(lst.empty() && lst2.size() == 2);
    lst = std::move(lst2);
    assert(lst2.empty() && lst.size() == 2);
    xl::list<std::string> lst3 = lst;
    assert(lst3.size() == 2 && lst.size() == 2); // original unchanged
    xl::list<std::string> lst4; lst4 = lst;
    assert(lst4.size() == 2 && lst4.front() == "hello");
    lst3.swap(lst4);
    assert(lst3.size() == 2 && lst4.size() == 2);
  }

  // TC-65  iterator validity: erase does not invalidate other iterators
  {
    xl::list lst = {1, 2, 3, 4, 5};
    auto it  = lst.begin(); ++it;
    auto it2 = std::next(it);
    lst.erase(it);
    assert(*it2 == 3);
    auto it3 = lst.begin();
    lst.insert(std::next(it3), 10);
    assert(*it3 == 1 && *it2 == 3);
  }

  // TC-66  resource management: destructor balances constructor calls
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

  // TC-67  emplace with multi-argument constructor (Person)
  {
    struct Person {
      std::string name; int age;
      Person(std::string n, int a) : name(std::move(n)), age(a) {}
    };
    xl::list<Person> lst;
    lst.emplace_back("Alice", 30); lst.emplace_front("Bob", 25);
    auto it = lst.begin(); ++it;
    it = lst.emplace(it, "Charlie", 40);
    assert(lst.size() == 3);
    assert(lst.front().name == "Bob" && lst.back().name == "Alice");
    assert(it->name == "Charlie" && it->age == 40);
  }

  // TC-68  single-element list: all operations
  {
    xl::list<int> lst;
    lst.push_back(42);
    assert(!lst.empty() && lst.size() == 1 && lst.front() == 42 && lst.back() == 42);
    lst.pop_front(); assert(lst.empty());
    lst.push_front(100); lst.pop_back(); assert(lst.empty());
    lst.push_back(200); lst.erase(lst.begin()); assert(lst.empty());
  }

  // TC-69  self-assignment guard
  {
    xl::list lst = {1, 2, 3};
    lst = lst;
    assert(lst.size() == 3 && lst.front() == 1 && lst.back() == 3);
    xl::list lst2 = {11, 22, 33};
    lst2 = lst2;
    assert((lst2 == std::array{11, 22, 33}));
  }

  // TC-70  iterator arithmetic: decrement from end, std::distance
  {
    xl::list lst = {1, 2, 3, 4, 5};
    auto end_it = lst.end(); --end_it;
    assert(*end_it == 5);
    ++end_it; assert(end_it == lst.end());
    assert(std::distance(lst.begin(), lst.end()) == 5);
  }

  // TC-71  const list: size, front, back, empty, cbegin/cend
  {
    const xl::list cl = {10, 20, 30};
    assert(cl.size() == 3 && cl.front() == 10 && cl.back() == 30 && !cl.empty());
    auto cit = cl.cbegin(); assert(*cit == 10);
    int sum = 0;
    for (auto it = cl.cbegin(); it != cl.cend(); ++it) sum += *it;
    assert(sum == 60);
  }

  // TC-72  repeated push/pop cycles (allocator stress)
  {
    xl::list<int> lst;
    for (int cycle = 0; cycle < 100; ++cycle) {
      for (int i = 0; i < 10; ++i) lst.push_back(i);
      for (int i = 0; i <  5; ++i) lst.pop_back();
    }
    assert(lst.size() == 500);
    lst.clear(); assert(lst.empty());
  }

  // TC-73  iterator validity after erase of middle element
  {
    xl::list lst = {1, 2, 3, 4, 5};
    std::vector<decltype(lst.begin())> iters;
    for (auto it = lst.begin(); it != lst.end(); ++it) iters.push_back(it);
    lst.erase(std::next(lst.begin(), 2));
    assert(*iters[0] == 1 && *iters[1] == 2);
    assert(*iters[3] == 4 && *iters[4] == 5);
  }

  // TC-74  merge with descending comparator
  {
    xl::list a = {5,3,1}, b = {6,4,2};
    a.merge(b, std::greater<int>());
    assert((a == xl::list<int>{6,5,4,3,2,1}) && b.empty());
  }

  // TC-75  unique with default and custom predicate
  {
    xl::list lst = {1, 2, 2, 3, 4, 4, 4, 5};
    auto orig_sz = lst.size();
    auto removed = lst.unique();
    assert(removed > 0 && lst.size() < orig_sz);
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
    lst = {1, 2, 4, 5, 7, 8};
    lst.unique([](int a, int b){ return std::abs(a - b) <= 1; });
    assert(lst.size() < 6);
  }

  // TC-76  remove_if: keep only odd numbers
  {
    xl::list<int> lst = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    lst.remove_if([](int x){ return x % 2 == 0; });
    assert(lst.size() == 5);
    for (const auto& v : lst) assert(v % 2 == 1);
  }

  // TC-77  large list: xl::find, reverse, sort
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

  // TC-78  fragmented push/pop pattern preserves size and sortability
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

  // TC-79  sort with case-insensitive string comparator
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

  // TC-80  large list: build reversed copy via rbegin, then sort original
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

  // TC-81  std::advance forward and backward; make_reverse_iterator
  {
    xl::list lst = {1, 2, 3, 4, 5};
    auto it = lst.begin();
    std::advance(it,  3); assert(*it == 4);
    std::advance(it, -2); assert(*it == 2);
    auto rit = std::make_reverse_iterator(it);
    assert(*rit == 1);
    std::advance(rit, -2); assert(*rit == 3);
  }

  // TC-82  move-only type (unique_ptr)
  {
    xl::list<std::unique_ptr<int>> pl;
    pl.push_back(std::make_unique<int>(42));
    pl.emplace_back(new int(100));
    assert(*pl.front() == 42);
    xl::list ml = std::move(pl);
    assert(pl.empty() && ml.size() == 2);
    xl::list nl(std::move(ml));
    assert(ml.empty() && nl.size() == 2);
  }

  // TC-83  range constructors: empty range, single element, input iterator
  {
    std::vector<int> ev;
    xl::list<int> empty_l(ev.begin(), ev.end());
    assert(empty_l.empty());
    int single = 42;
    xl::list<int> single_l(&single, &single + 1);
    assert(single_l.size() == 1 && single_l.front() == 42);
    std::istringstream iss("1 2 3 4");
    std::istream_iterator<int> iit(iss), eos;
    xl::list<int> input_l(iit, eos);
    assert(input_l.size() == 4 && input_l.back() == 4);
  }

  // TC-84  spaceship / relational operator completeness
  {
    xl::list a={1,2,3}, b={1,2,3}, c={1,2,4}, d={1,2};
    assert(a==b && a!=c && a!=d && d<a && c>a && d<=a && a>=b && c>=a);
  }

  // TC-85  range-constructed list equals C array
  {
    int const a[10]{};
    assert(std::ranges::equal(a, xl::list(a)));
  }

  // TC-86  iterator stability after copy and move
  {
    xl::list orig = {10, 20, 30, 40, 50};
    auto it = std::next(orig.begin(), 2); assert(*it == 30);
    xl::list copy = orig;
    *it = 99;
    assert((orig == xl::list<int>{10, 20, 99, 40, 50}));
    assert((copy == xl::list<int>{10, 20, 30, 40, 50}));
    xl::list moved = std::move(orig);
    assert(orig.empty());                                    // NOLINT
    assert((moved == xl::list<int>{10, 20, 99, 40, 50}));
  }

  // TC-87  iterator validity around erase, insert, and splice
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it1 = lst.begin();
    auto it2 = std::next(it1);
    auto it3 = std::next(it2);
    it3 = lst.erase(it2);
    assert(*it1 == 1 && *it3 == 3);
    auto it4 = lst.insert(it3, 10);
    assert(*it3 == 3 && *it4 == 10);
    xl::list<int> lst2 = {20, 30};
    lst.splice(it3 = std::next(it4), lst2);
    assert(*it3 == 3 && lst.size() == 7);
  }

  // TC-88  xl::find and xl::erase (variadic)
  {
    xl::list lst{3, 1, 2};
    *xl::find(lst, 3) = 1;
    assert(lst.front() == 1);
    xl::erase(lst, 3, 2, 1);
    assert(lst.empty());
  }

  // TC-89  sort variants I=1..4 all produce the same ordered result
  {
    auto make = []() -> xl::list<int> { return {5,3,1,4,2}; };
    auto ref = make(); ref.sort();
    auto l1 = make(); l1.sort<1>();
    auto l2 = make(); l2.sort<2>();
    auto l3 = make(); l3.sort<3>();
    auto l4 = make(); l4.sort<4>();
    assert(l1 == ref && l2 == ref && l3 == ref && l4 == ref);
  }

  // TC-90  sort sub-range via iterator pair
  {
    xl::list lst = {5, 3, 1, 4, 2};
    // sort only the middle three elements [3,1,4]
    auto b = std::next(lst.begin());
    auto e = std::next(b, 3);
    lst.sort(b, e);
    // first element (5) and last element (2) must stay; middle must be sorted
    assert(lst.front() == 5 && lst.back() == 2);
    int prev = std::numeric_limits<int>::min();
    auto it = std::next(lst.begin()); int cnt = 0;
    while (it != std::prev(lst.end())) { assert(*it >= prev); prev = *it; ++it; ++cnt; }
    assert(cnt == 3);
  }

  // TC-91  sort stability across all five variants
  {
    struct Item { int key, order; };
    auto check_stable = [](xl::list<Item> lst) {
      lst.sort([](const Item& a, const Item& b){ return a.key < b.key; });
      auto it = lst.begin();
      int last_key = -1, last_order_for_key = -1;
      for (; it != lst.end(); ++it) {
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

  // TC-92  at() read and write access
  {
    xl::list lst = {10, 20, 30, 40, 50};
    assert(lst.at(0) == 10 && lst.at(2) == 30 && lst.at(4) == 50);
    lst.at(2) = 99;
    assert(lst.at(2) == 99);
    const xl::list clst = {1, 2, 3};
    assert(clst.at(0) == 1 && clst.at(2) == 3);
  }

  // TC-93  assign(iter, iter) from an array
  {
    xl::list lst = {1, 2, 3};
    int arr[] = {7, 8, 9, 10};
    lst.assign(std::begin(arr), std::end(arr));
    assert(lst.size() == 4 && lst.front() == 7 && lst.back() == 10);
  }

  // TC-94  merge two already-merged lists (chained merge)
  {
    xl::list a = {1, 4, 7}, b = {2, 5, 8}, c = {3, 6, 9};
    a.merge(b); a.merge(c);
    assert(a.size() == 9);
    assert(std::is_sorted(a.begin(), a.end()));
    int expected = 1;
    for (const auto& v : a) assert(v == expected++);
  }

  // TC-95  erase range spanning entire list
  {
    xl::list lst = {1, 2, 3, 4, 5};
    auto it = lst.erase(lst.begin(), lst.end());
    assert(lst.empty() && it == lst.end());
  }

  // TC-96  insert(pos, iter, iter) iterator-range overload
  {
    xl::list lst = {1, 5};
    int mid[] = {2, 3, 4};
    lst.insert(std::next(lst.begin()), std::begin(mid), std::end(mid));
    assert(lst.size() == 5 && std::is_sorted(lst.begin(), lst.end()));
  }

  // TC-97  unique with binary predicate on sorted list of strings
  {
    xl::list<std::string> lst = {"a", "A", "b", "B", "c"};
    lst.sort([](const std::string& x, const std::string& y){
      return std::tolower(x[0]) < std::tolower(y[0]); });
    lst.unique([](const std::string& x, const std::string& y){
      return std::tolower(x[0]) == std::tolower(y[0]); });
    assert(lst.size() == 3);
  }

  // TC-98  splice then sort: whole second list, verify ordering
  {
    xl::list a = {3, 1, 4}, b = {1, 5, 9, 2, 6};
    a.splice(a.end(), b);
    assert(b.empty() && a.size() == 8);
    a.sort();
    assert(std::is_sorted(a.begin(), a.end()));
  }

  // TC-99  move-construct list of strings; strings are actually moved
  {
    xl::list<std::string> src;
    src.emplace_back("hello"); src.emplace_back("world");
    xl::list<std::string> dst = std::move(src);
    assert(src.empty());
    assert(dst.size() == 2 && dst.front() == "hello" && dst.back() == "world");
  }

  // TC-100  large list with many duplicates: sort + unique
  {
    xl::list<int> lst;
    for (int i = 0; i < 1000; ++i) lst.push_back(i % 10);
    lst.sort();
    lst.unique();
    assert(lst.size() == 10);
    int expected = 0;
    for (const auto& v : lst) assert(v == expected++);
  }

  // TC-101  remove all elements matching predicate; list becomes empty
  {
    xl::list lst = {2, 4, 6, 8, 10};
    lst.remove_if([](int x){ return x % 2 == 0; });
    assert(lst.empty());
  }

  // TC-102  self-merge (both halves via splice then merge)
  {
    xl::list lst = {1, 3, 5, 2, 4, 6};
    // split at middle
    xl::list<int> back;
    auto mid = std::next(lst.begin(), 3);
    back.splice(back.begin(), lst, mid, lst.end());
    lst.merge(back);
    assert(back.empty() && lst.size() == 6);
    assert(std::is_sorted(lst.begin(), lst.end()));
  }

  // TC-103  std::sort-compatible: std::list-style interface sanity check
  {
    // xl::list models a std::list-like interface; verify std::list interop pattern
    xl::list xl_lst = {9, 7, 5, 3, 1, 2, 4, 6, 8};
    std::list std_lst(xl_lst.begin(), xl_lst.end());
    xl_lst.sort(); std_lst.sort();
    assert(std_lst == xl_lst);
  }

  // TC-104  push_back / emplace_back performance: 1M elements
  {
    constexpr int N = 1'000'000;
    xl::list lst(std::views::iota(0, N));
    assert(lst.size() == N);
    assert(lst.front() == 0 && lst.back() == N - 1);
    lst.clear(); assert(lst.empty());
  }

  // TC-105  erase_if free function returns correct removal count
  {
    xl::list lst = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto removed = xl::erase_if(lst, [](int x){ return x % 2 == 0; });
    assert(removed == 5 && lst.size() == 5);
    for (const auto& v : lst) assert(v % 2 != 0);

    xl::list lst2 = {10, 20, 30};
    auto r2 = xl::erase_if(lst2, [](int){ return false; });
    assert(r2 == 0 && lst2.size() == 3);
  }

  // TC-106  reverse_iterator full traversal on const list
  {
    const xl::list lst = {1, 2, 3, 4, 5};
    std::vector<int> rev;
    for (auto it = lst.crbegin(); it != lst.crend(); ++it)
      rev.push_back(*it);
    assert((rev == std::vector<int>{5, 4, 3, 2, 1}));
  }

  // TC-107  sort all five variants on already-sorted and reverse-sorted input
  {
    auto sorted_asc  = []{ return xl::list{1,2,3,4,5,6,7,8}; };
    auto sorted_desc = []{ return xl::list{8,7,6,5,4,3,2,1}; };

    for (auto& make : std::initializer_list<std::function<xl::list<int>()>>{sorted_asc, sorted_desc}) {
      auto l0 = make(); l0.sort<0>();
      auto l1 = make(); l1.sort<1>();
      auto l2 = make(); l2.sort<2>();
      auto l3 = make(); l3.sort<3>();
      auto l4 = make(); l4.sort<4>();
      assert(std::is_sorted(l0.begin(), l0.end()));
      assert(l0 == l1 && l1 == l2 && l2 == l3 && l3 == l4);
    }
  }

  // TC-108  insert(pos, count, value) at begin, end, and middle
  {
    xl::list<int> lst = {1, 5};
    lst.insert(lst.begin(), 2, 0);
    assert(lst.size() == 4 && lst.front() == 0);

    lst.insert(lst.end(), 2, 9);
    assert(lst.size() == 6 && lst.back() == 9);

    auto mid = std::next(lst.begin(), 3);
    lst.insert(mid, 3, 4);
    assert(lst.size() == 9);
  }

  // TC-109  splice single element at end of destination
  {
    xl::list a = {1, 2, 3};
    xl::list b = {4, 5, 6};
    a.splice(a.end(), b, b.begin());
    assert(a.size() == 4 && a.back() == 4);
    assert(b.size() == 2 && b.front() == 5);
  }

  // TC-110  list of pairs: sort by second field, then by first
  {
    xl::list<std::pair<int,int>> lst = {{3,1},{1,3},{2,2},{1,1},{2,1}};
    lst.sort([](const auto& a, const auto& b){
      return a.second != b.second ? a.second < b.second : a.first < b.first;
    });
    auto it = lst.begin();
    assert(it->second <= std::next(it)->second);
    assert(std::is_sorted(lst.begin(), lst.end(),
      [](const auto& a, const auto& b){
        return a.second != b.second ? a.second < b.second : a.first < b.first;
      }));
  }

  // TC-111  erase range in the middle; size and endpoints correct
  {
    xl::list lst = {1, 2, 3, 4, 5, 6, 7};
    auto first = std::next(lst.begin(), 2); // points to 3
    auto last  = std::next(first, 3);       // points to 6
    auto ret = lst.erase(first, last);
    assert(lst.size() == 4);
    assert(lst.front() == 1 && lst.back() == 7);
    assert(*ret == 6);
  }

  // TC-112  unique on single-element and two-element lists
  {
    xl::list one = {42};
    assert(one.unique() == 0 && one.size() == 1);

    xl::list same = {7, 7};
    assert(same.unique() == 1 && same.size() == 1);

    xl::list diff = {3, 4};
    assert(diff.unique() == 0 && diff.size() == 2);
  }

  // TC-113  remove returns count of removed elements
  {
    xl::list lst = {1, 2, 3, 2, 4, 2};
    auto n = lst.remove(2);
    assert(n == 3 && lst.size() == 3);
    assert((lst == xl::list<int>{1, 3, 4}));

    auto m = lst.remove(99); // value not present
    assert(m == 0 && lst.size() == 3);
  }

  // TC-114  remove_if returns count of removed elements
  {
    xl::list lst = {1, 2, 3, 4, 5, 6};
    auto n = lst.remove_if([](int x){ return x % 2 == 0; });
    assert(n == 3 && lst.size() == 3);
    for (const auto& v : lst) assert(v % 2 != 0);

    auto m = lst.remove_if([](int){ return false; });
    assert(m == 0);
  }

  // TC-115  std::swap exchanges two lists
  {
    xl::list a = {1, 2, 3}, b = {4, 5};
    std::swap(a, b);
    assert(a.size() == 2 && a.front() == 4 && a.back() == 5);
    assert(b.size() == 3 && b.front() == 1 && b.back() == 3);
  }

  // TC-116  insert(pos, count=0, value) is a no-op
  {
    xl::list lst = {1, 2, 3};
    auto it = std::next(lst.begin());
    lst.insert(it, 0, 99);
    assert(lst.size() == 3 && (lst == xl::list<int>{1, 2, 3}));
  }

  // TC-117  cbegin / cend on a non-const list
  {
    xl::list lst = {10, 20, 30};
    int sum = 0;
    for (auto it = lst.cbegin(); it != lst.cend(); ++it) sum += *it;
    assert(sum == 60);
    assert(lst.cbegin() == lst.begin());
    assert(lst.cend()   == lst.end());
  }

  // TC-118  assign from initialiser list to an empty list
  {
    xl::list<int> lst;
    lst.assign({7, 8, 9});
    assert(lst.size() == 3 && lst.front() == 7 && lst.back() == 9);
  }

  // TC-119  pop_back / pop_front reducing list to exactly one element
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

  // TC-120  std::copy into list via back_inserter
  {
    std::vector src = {1, 2, 3, 4, 5};
    xl::list<int> dst;
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    assert(dst.size() == 5 && dst.front() == 1 && dst.back() == 5);
    assert(std::equal(src.begin(), src.end(), dst.begin()));
  }

  // TC-121  std::fill modifies all elements in-place
  {
    xl::list lst = {1, 2, 3, 4, 5};
    std::fill(lst.begin(), lst.end(), 0);
    assert(lst.size() == 5);
    for (const auto& v : lst) assert(v == 0);
  }

  // TC-122  emplace returns iterator pointing to new element
  {
    xl::list lst = {1, 3};
    auto it = lst.emplace(std::next(lst.begin()), 2);
    assert(*it == 2);
    assert(lst.size() == 3);
    assert((lst == std::array{1, 2, 3}));
  }
}

int main()
{
  test();
  std::cout << "All tests passed!\n";
  return 0;
}

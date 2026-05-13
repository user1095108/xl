#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

#include "list.hpp"

void test()
{
  // Basic push_back, size, index access, iterator, insert, erase, swap, copy
  {
    xl::list<int> container;

    // push three elements
    container.push_back(1, 2, 3);
    assert(container.size() == 3);

    // index access
    assert(container[0] == 1);
    assert(container[1] == 2);
    assert(container[2] == 3);

    // modify via index
    container[1] = 42;
    assert(container[1] == 42);

    // pop_back reduces size
    container.pop_back();
    assert(container.size() == 2);

    // forward iterator walk
    auto it = container.begin();
    assert(*it == 1);
    ++it;
    assert(*it == 42);
    ++it;
    assert(it == container.end());

    // clear empties the list
    container.clear();
    assert(container.empty());
    assert(container.size() == 0);

    // insert at specific position
    container.push_back(4);
    container.insert(std::next(container.begin()), 5);
    assert(container.size() == 2);
    assert(container[0] == 4);
    assert(container[1] == 5);

    // erase at beginning
    container.erase(container.begin());
    assert(container.size() == 1);
    assert(container[0] == 5);

    // swap two lists
    decltype(container) other;
    other.push_back(6, 7);
    container.swap(other);
    assert(container.size() == 2);
    assert(other.size() == 1);

    // copy construction
    decltype(container) copied = container;
    assert(copied.size() == 2);
    assert(copied[0] == 6);

    // range-based for loop
    int sum = 0;
    for (const auto& item : container)
      sum += item;
    assert(sum == 13);
  }

  // front, back, insert in middle, erase returning iterator
  {
    xl::list<int> l;

    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    assert(l.size() == 3);
    assert(!l.empty());
    assert(l.front() == 1);
    assert(l.back() == 3);

    // insert before the second element
    auto it = l.begin();
    ++it;
    it = l.insert(it, 4);
    assert(l.size() == 4);
    assert(*it == 4);

    // erase the second element; returned iterator points to the next one
    it = l.begin();
    ++it;
    it = l.erase(it);
    assert(l.size() == 3);
    assert(*it == 2);

    l.clear();
    assert(l.size() == 0);
    assert(l.empty());
  }

  // push_back, push_front, insert at second position, pop_back, pop_front
  {
    xl::list<int> l;
    assert(l.empty());

    l.push_back(1);
    l.push_front(2);
    l.insert(++l.begin(), 3);   // 3 at position 1
    assert(l.size() == 3);
    assert(!l.empty());

    auto it = l.begin();
    assert(*it == 2); ++it;
    assert(*it == 3); ++it;
    assert(*it == 1);

    l.pop_back();
    assert(l.size() == 2);
    l.pop_front();
    assert(l.size() == 1);
    l.clear();
    assert(l.empty());
  }

  // Variadic push_back, front/back accessors, pop_back, clear
  {
    xl::list<int> l;
    assert(l.empty());

    l.push_back(1, 2, 3);
    assert(!l.empty());
    assert(l.size() == 3);
    assert(l.front() == 1);
    assert(l.back() == 3);

    l.pop_back();
    assert(l.back() == 2);

    l.clear();
    assert(l.empty());
  }

  // Initialiser-list construction, modify first element, insert/erase, sum
  {
    xl::list numbers = {1, 2, 3, 4, 5};

    auto it = numbers.begin();
    assert(*it == 1);

    // modify first element through iterator
    *it = 10;
    assert(*it == 10);

    // insert after first element
    it = numbers.insert(std::next(it), 20);
    assert(*it == 20);

    // erase the just-inserted element
    it = numbers.erase(it);
    assert(*it == 2);

    // sum: 10+2+3+4+5 = 24
    int sum = 0;
    for (const auto& num : numbers)
      sum += num;
    assert(sum == 24);
  }

  // multi_t constructor, push_front/back, pop_front/back, insert/erase, clear
  {
    xl::list<int> l{xl::multi_t{}, 1, 2, 3, 4, 5};
    assert(l.size() == 5);

    l.push_front(0);
    assert(l.size() == 6);
    assert(l.front() == 0);

    l.push_back(6);
    assert(l.size() == 7);
    assert(l.back() == 6);

    l.pop_front();
    assert(l.size() == 6);
    assert(l.front() == 1);

    l.pop_back();
    assert(l.size() == 5);
    assert(l.back() == 5);

    l.clear();
    assert(l.empty());

    // push_front / pop_front round-trip
    l.push_front(1);
    assert(l.front() == 1);
    l.pop_front();
    assert(l.empty());

    // push_back / pop_back round-trip
    l.push_back(6);
    assert(l.back() == 6);
    l.pop_back();

    // insert / erase round-trip
    l.insert(l.begin(), 2);
    assert(*l.begin() == 2);
    l.erase(l.begin());
    assert(l.size() == 0);

    l.clear();
    assert(l.empty());
  }

  // Loop-based push_back/front, pop_back/front, sequential iteration
  {
    xl::list<int> l;

    for (int i = 0; i < 10; ++i)
      l.push_back(i);
    assert(l.back() == 9);

    l.push_front(100);
    assert(l.front() == 100);

    l.pop_back();
    assert(l.back() == 8);

    l.pop_front();
    assert(l.front() == 0);

    // values should now be 0..8
    int expected = 0;
    for (auto val : l)
      assert(val == expected++);
  }

  // Fill constructor (count, value)
  {
    xl::list<int> l(5, 10);
    assert(l.size() == 5);
    for (const auto& e : l)
      assert(e == 10);
  }

  // from_range constructor with string_view; palindrome peel test
  {
    xl::list palindrome(xl::from_range, std::string_view("racecar"));

    while (palindrome.size() > 1) {
      assert(palindrome.front() == palindrome.back());
      palindrome.pop_front();
      palindrome.pop_back();
    }
    assert(1 == palindrome.size());
    palindrome.sort();
  }

  // from_range constructor with views::iota; range assignment
  {
    xl::list l(xl::from_range, std::views::iota(0, 100));
    assert(100 == l.size());
    l = std::views::iota(0, 10);
    assert(10 == l.size());
  }

  // insert overloads: at begin, at end, at position; single and multi-element
  {
    xl::list myList = {1, 2, 3, 4, 5};

    // insert single element at beginning
    myList.insert(myList.begin(), 0);
    assert(myList.front() == 0);

    // insert single element at end
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), 6);
    assert(myList.back() == 6);

    // insert single element at specific position
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), 10);
    assert(myList[2] == 10);

    // insert initialiser list at beginning
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.begin(), {0, 0, 0});
    assert(myList.front() == 0);
    assert(myList.size() == 8);

    // insert initialiser list at end
    myList = {1, 2, 3, 4, 5};
    myList.insert(myList.end(), {6, 6, 6});
    assert(myList.back() == 6);
    assert(myList.size() == 8);

    // insert initialiser list at specific position
    myList = {1, 2, 3, 4, 5};
    myList.insert(std::next(myList.begin(), 2), {10, 10, 10});
    assert(myList[2] == 10);
    assert(myList.size() == 8);
  }

  // unique: empty list, all-duplicate, no-duplicate, mixed
  {
    xl::list<int> my_list;
    assert(my_list.unique() == 0);

    my_list = {1, 1, 1};
    assert(my_list.unique() == 2);

    my_list = {1, 2, 3};
    assert(!my_list.unique());

    my_list = {1, 1, 2, 2, 3};
    assert(my_list.unique() == 2);

    my_list = {1, 1, 2, 2, 3, 4};
    assert(my_list.unique() == 2);

    my_list = {1, 2, 1, 1, 3, 3, 3, 4, 5, 4};
    assert(my_list.unique() == 3);
  }

  // resize: shrink, same size, grow with default, grow with value
  {
    xl::list<int> mylist = {1, 2, 3, 4, 5};

    mylist.resize(3);
    assert(mylist.size() == 3);

    mylist.resize(3);
    assert(mylist.size() == 3);

    mylist.resize(5);
    assert(mylist.size() == 5);

    mylist.resize(8, 10);
    assert(mylist.size() == 8);
  }

  // merge two sorted lists
  {
    xl::list<int> list1 = {1, 2, 3, 4, 5};
    xl::list<int> list2 = {6, 7, 8, 9, 10};

    list1.merge(std::move(list2));
    assert(list1.size() == 10);
    assert(list1.front() == 1);
    assert(list1.back() == 10);

    // resize merged result
    list1.resize(5);
    assert(list1.size() == 5);
    assert(list1.front() == 1);
    assert(list1.back() == 5);
  }

  // merge: various size and content combinations
  {
    xl::list<int> list1, list2;

    // Test 1: both empty
    list1.merge(std::move(list2));
    assert(list1.empty());

    // Test 2: two non-empty
    list1.push_back(1); list1.push_back(2);
    list2.push_back(3); list2.push_back(4);
    list1.merge(std::move(list2));
    assert(list1.size() == 4);
    assert(list1.front() == 1);
    assert(list1.back() == 4);

    // Test 3: different sizes
    list1.clear(); list2.clear();
    list1.push_back(1); list1.push_back(2);
    list2.push_back(3); list2.push_back(4); list2.push_back(5);
    list1.merge(std::move(list2));
    assert(list1.size() == 5);
    assert(list1.front() == 1);
    assert(list1.back() == 5);

    // Test 4: same size
    list1.clear(); list2.clear();
    list1.push_back(1); list1.push_back(2);
    list2.push_back(3); list2.push_back(4);
    list1.merge(std::move(list2));
    assert(list1.size() == 4);
    assert(list1.front() == 1);
    assert(list1.back() == 4);

    // Test 5: non-empty + empty
    list1.clear(); list2.clear();
    list1.push_back(1, 2, 3, 4, 5);
    list1.merge(list2);
    assert(list1.front() == 1);
    assert(list1.back() == 5);
    assert(list1.size() == 5);

    // Test 6: duplicate elements
    list1.clear(); list2.clear();
    list1.push_back(1, 2);
    list2.push_back(1, 2);
    list1.merge(list2);
    assert(list1.front() == 1);
    assert(list1.back() == 2);
    assert(list1.size() == 4);

    // Test 7: same element set, different sizes
    list1.clear(); list2.clear();
    list1.push_back(1, 2, 3, 4, 5);
    list1.merge(list2);
    assert(list1.front() == 1);
    assert(list1.back() == 5);
    assert(list1.size() == 5);

    // Test 8: same size and elements
    list1.clear(); list2.clear();
    list1.push_back(1); list1.push_back(2);
    list2.push_back(1); list2.push_back(2);
    list1.merge(list2);
    assert(list1.front() == 1);
    assert(list1.back() == 2);
    assert(list1.size() == 4);

    // Test 9: check element order after merge
    list1.clear(); list2.clear();
    list1.push_back(1, 2, 3, 4, 5);
    list1.merge(list2);
    assert(list1.front() == 1);
    assert(list1.back() == 5);
    assert(list1.at(1) == 2);
    assert(list1.at(2) == 3);
    assert(list1.at(3) == 4);
    assert(list1.size() == 5);

    // Test 10: move-merge same-sized lists with order check
    list1.clear(); list2.clear();
    list1.push_back(1); list1.push_back(2);
    list2.push_back(1); list2.push_back(2);
    list1.merge(std::move(list2));
    assert(list1.size() == 4);
    assert(list1.front() == 1);
    assert(list1.back() == 2);
    assert(list1.at(1) == 1);
    assert(list1.at(2) == 2);
  }

  // resize: eight scenarios covering empty/non-empty, shrink, grow, order
  {
    // Test 1: resize empty to 0
    xl::list<int> list1;
    list1.resize(0);
    assert(list1.empty());

    // Test 2: resize non-empty to same size
    xl::list<int> list2;
    list2.push_back(1); list2.push_back(2);
    list2.resize(2);
    assert(list2.size() == 2);
    assert(list2.front() == 1);
    assert(list2.back() == 2);

    // Test 3: grow with default value
    xl::list<int> list3;
    list3.push_back(1); list3.push_back(2);
    list3.resize(3);
    assert(list3.size() == 3);
    assert(list3.front() == 1);
    assert(list3.back() == 0);

    // Test 4: same size (no-op)
    xl::list<int> list4;
    list4.push_back(1); list4.push_back(2);
    list4.resize(2);
    assert(list4.size() == 2);
    assert(list4.front() == 1);
    assert(list4.back() == 2);

    // Test 5: grow (same as Test 3)
    xl::list<int> list5;
    list5.push_back(1); list5.push_back(2);
    list5.resize(3);
    assert(list5.size() == 3);
    assert(list5.front() == 1);
    assert(list5.back() == 0);

    // Test 6: no-op resize
    xl::list<int> list6;
    list6.push_back(1); list6.push_back(2);
    list6.resize(2);
    assert(list6.size() == 2);
    assert(list6.front() == 1);
    assert(list6.back() == 2);

    // Test 7: grow and check middle element
    xl::list<int> list7;
    list7.push_back(1); list7.push_back(2);
    list7.resize(3);
    assert(list7.size() == 3);
    assert(list7.front() == 1);
    assert(list7.back() == 0);
    assert(list7.at(1) == 2);

    // Test 8: no-op resize with middle element check
    xl::list<int> list8;
    list8.push_back(1); list8.push_back(2);
    list8.resize(2);
    assert(list8.size() == 2);
    assert(list8.front() == 1);
    assert(list8.back() == 2);
    assert(list8.at(1) == 2);
  }

  // Initialiser-list construction, push_back, sum, erase
  {
    xl::list<int> emptyList;
    assert(emptyList.size() == 0);
    assert(emptyList.empty());

    xl::list<int> list1 = {42};
    assert(list1.size() == 1);
    assert(list1.front() == 42);

    xl::list<int> list2 = {1, 2, 3, 4, 5};
    assert(list2.size() == 5);
    assert(list2.front() == 1);
    assert(list2.back() == 5);

    list2.push_back(6);
    assert(list2.size() == 6);
    assert(list2.front() == 1);
    assert(list2.back() == 6);

    int sum = 0;
    for (const auto& element : list2)
      sum += element;
    assert(sum == 21);

    list2.erase(list2.begin());
    assert(list2.size() == 5);
    assert(list2.front() == 2);
    assert(list2.empty() == false);
  }

  // splice: all three overloads (whole list, single element, range)
  {
    xl::list<int> list1 = {1, 2, 3, 4, 5};
    xl::list<int> list2 = {10, 20, 30};

    // splice whole list at beginning
    auto it1 = list1.begin();
    list1.splice(it1, std::move(list2));
    assert((list1 == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}));
    assert(list2.empty());

    // splice empty list in middle (no-op)
    it1 = list1.begin();
    std::advance(it1, 3);
    list1.splice(it1, std::move(list2));
    assert((list1 == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}));
    assert(list2.empty());

    // splice empty list at end (no-op)
    list1.splice(list1.end(), std::move(list2));
    assert((list1 == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}));
    assert(list2.empty());

    // splice single element from another list
    xl::list<int> list3 = {100};
    it1 = list1.begin();
    auto it2 = list3.begin();
    list1.splice(it1, std::move(list3), it2);
    assert((list1 == xl::list<int>{100, 10, 20, 30, 1, 2, 3, 4, 5}));
    assert(list3.empty());

    // splice a range between two lists
    it1 = list1.begin();
    auto it2_begin = list1.begin();
    std::advance(it2_begin, 3);
    auto it2_end = list1.begin();
    std::advance(it2_end, 6);
    list3 = {200, 300};
    list3.splice(list3.begin(), std::move(list1), it2_begin, it2_end);
    assert(list1 == xl::list<int>({100, 10, 20, 3, 4, 5}));
    assert(list3 == xl::list<int>({30, 1, 2, 200, 300}));
  }

  // splice overload 1: whole list; overload 2: single element; overload 3: range
  {
    // overload 1: entire list
    {
      xl::list<int> list1{1, 2, 3, 4, 5};
      xl::list<int> list2{6, 7, 8, 9, 10};
      list1.splice(list1.begin(), list2);
      assert(list1.size() == 10);
      assert(list2.empty());
    }

    // overload 2: single element
    {
      xl::list<int> list1{1, 2, 3, 4, 5};
      xl::list<int> list2{6, 7, 8, 9, 10};
      auto it = list2.begin();
      std::advance(it, 2);            // pointing to 8
      list1.splice(list1.begin(), list2, it);
      assert(list1.size() == 6);
      assert(list2.size() == 4);
      assert(list1.front() == 8);
    }

    // overload 3: range [6, 7, 8]
    {
      xl::list<int> list1{1, 2, 3, 4, 5};
      xl::list<int> list2{6, 7, 8, 9, 10};
      auto it1 = list2.begin();
      auto it2 = it1;
      std::advance(it2, 3);
      list1.splice(list1.begin(), list2, it1, it2);
      assert(list1.size() == 8);
      assert(list2.size() == 2);
      assert(list1.front() == 6);
    }
  }

  // merge: edge cases including custom comparator and duplicates
  {
    // Test 1: both empty
    xl::list<int> list1, list2;
    list1.merge(list2);
    assert(list1.empty() && list2.empty());

    // Test 2: empty into non-empty
    xl::list<int> list3 = {1, 2, 3};
    xl::list<int> list4;
    list3.merge(list4);
    assert(list3 == xl::list<int>({1, 2, 3}) && list4.empty());

    // Test 3: non-empty into empty
    xl::list<int> list5;
    xl::list<int> list6 = {4, 5, 6};
    list5.merge(list6);
    assert(list5 == xl::list<int>({4, 5, 6}) && list6.empty());

    // Test 4: equal-sized sorted lists
    xl::list<int> list7 = {1, 3, 5};
    xl::list<int> list8 = {2, 4, 6};
    list7.merge(list8);
    assert(list7 == xl::list<int>({1, 2, 3, 4, 5, 6}) && list8.empty());

    // Test 5: different-sized sorted lists
    xl::list<int> list9  = {1, 3, 5};
    xl::list<int> list10 = {2, 4};
    list9.merge(list10);
    assert(list9 == std::initializer_list<int>({1, 2, 3, 4, 5}) && list10.empty());

    // Test 6: duplicates interleaved
    xl::list<int> list11 = {1, 2, 3};
    xl::list<int> list12 = {2, 3, 4};
    list11.merge(list12);
    assert(list11 == xl::list<int>({1, 2, 2, 3, 3, 4}) && list12.empty());

    // Test 7: custom descending comparator
    xl::list<int> list13 = {5, 3, 1};
    xl::list<int> list14 = {6, 4, 2};
    list13.merge(list14, [](int a, int b) { return a > b; });
    assert(list13 == xl::list<int>({6, 5, 4, 3, 2, 1}) && list14.empty());
  }

  // append_range, prepend_range, insert_range
  {
    xl::list<int> lst{1, 2, 3};
    int const vec[]{4, 5, 6};

    lst.append_range(vec);
    assert((lst == xl::list<int>{1, 2, 3, 4, 5, 6}));

    lst.prepend_range(vec);
    assert((lst == xl::list<int>{4, 5, 6, 1, 2, 3, 4, 5, 6}));

    auto it = lst.begin();
    std::advance(it, 3);
    lst.insert_range(it, vec);
    assert((lst == xl::list<int>{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));
  }

  // assign_range
  {
    xl::list<int> list1 = {1, 2, 3, 4, 5};
    int const vector1[]{6, 7, 8, 9, 10};

    list1.assign_range(vector1);
    assert(list1.size() == std::size(vector1));

    auto list_it   = list1.begin();
    auto vector_it = std::begin(vector1);
    while (list_it) assert(*list_it++ == *vector_it++);

    list1 = vector1;
    assert(list1.size() == std::size(vector1));
  }

  // append_range / prepend_range / insert_range / assign_range on list sources
  {
    xl::list<int> myList     = {1, 2, 3};
    xl::list<int> appendList  = {4, 5, 6};
    xl::list<int> prependList = {7, 8, 9};
    xl::list<int> insertList  = {10, 11, 12};
    xl::list<int> assignList  = {13, 14, 15};

    myList.append_range(appendList);
    assert(myList == xl::list<int>({1, 2, 3, 4, 5, 6}));

    myList.prepend_range(prependList);
    assert(myList == xl::list<int>({7, 8, 9, 1, 2, 3, 4, 5, 6}));

    auto insertPos = std::find(myList.begin(), myList.end(), 1);
    myList.insert_range(insertPos, insertList);
    assert(myList == xl::list<int>({7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6}));

    myList.assign_range(assignList);
    assert(myList == xl::list<int>({13, 14, 15}));
  }

  // emplace_back, emplace_front, pop_front, pop_back, sort, comprehensive ops
  {
    xl::list<int> list1;

    // sort empty list is a no-op
    assert(list1.empty());
    list1.sort();

    list1.push_back(10);
    list1.push_back(20);
    list1.push_back(30);
    assert(list1.size() == 3);

    list1.pop_front();
    assert(list1.size() == 2);

    auto it = list1.begin();
    assert(*it == 20); ++it;
    assert(*it == 30);

    list1.clear();
    assert(list1.empty());

    list1.push_front(10);
    list1.push_front(20);
    list1.push_front(30);
    assert(list1.size() == 3);

    list1.pop_back();
    assert(list1.size() == 2);

    it = list1.begin();
    assert(*it == 30); ++it;
    assert(*it == 20);

    xl::list<int> list2 = {10, 20, 30, 40, 50};
    assert(list2.size() == 5);

    list2.erase(list2.begin());
    assert(list2.size() == 4);

    it = list2.begin();
    assert(*it == 20);

    list2.insert(list2.begin(), 10);
    assert(list2.size() == 5);

    it = list2.begin();
    assert(*it == 10);

    // emplace_back
    xl::list<int> list3;
    list3.emplace_back(10);
    list3.emplace_back(20);
    list3.emplace_back(30);
    assert(list3.size() == 3);

    it = list3.begin();
    assert(*it == 10); ++it;
    assert(*it == 20); ++it;
    assert(*it == 30);

    xl::list<int> list4{10, 20, 30, 40, 50};
    assert(list4.size() == 5);
    it = list4.begin();
    assert(*it == 10); ++it;
    assert(*it == 20); ++it;
    assert(*it == 30); ++it;
    assert(*it == 40); ++it;
    assert(*it == 50);

    // copy construction
    xl::list<int> list5(list4);
    assert(list5.size() == 5);
    it = list5.begin();
    assert(*it == 10); ++it;
    assert(*it == 20); ++it;
    assert(*it == 30); ++it;
    assert(*it == 40); ++it;
    assert(*it == 50);
  }

  // Comprehensive single-block exercise of most member functions
  {
    xl::list<int> lst;

    lst.push_back(10); lst.push_back(20); lst.push_back(30);
    assert(lst.size() == 3);
    assert(lst.back() == 30);

    lst.push_front(5);
    assert(lst.size() == 4);
    assert(lst.front() == 5);

    lst.pop_back();
    assert(lst.size() == 3);
    assert(lst.back() == 20);

    lst.pop_front();
    assert(lst.size() == 2);
    assert(lst.front() == 10);

    auto it = lst.begin();
    lst.insert(it, 15);
    assert(lst.size() == 3);
    assert(lst.front() == 15);

    it = lst.begin();
    lst.erase(it);
    assert(lst.size() == 2);
    assert(lst.front() == 10);

    lst.clear();
    assert(lst.empty());

    lst.resize(5, 100);
    assert(lst.size() == 5);
    assert(lst.back() == 100);

    lst.assign(3, 200);
    assert(lst.size() == 3);
    assert(lst.front() == 200);

    xl::list<int> lst2 = {1, 2, 3};
    it = lst.begin();
    lst.splice(it, lst2);
    assert(lst.size() == 6);
    assert(lst.front() == 1);
    assert(lst2.empty());

    lst.remove(1);
    assert(lst.size() == 5);
    assert(lst.front() == 2);

    lst.remove_if([](int i){ return i > 2; });
    assert(lst.size() == 1);
    assert(lst.back() == 2);

    lst.push_back(2);
    lst.unique();
    assert(lst.size() == 1);
    assert(lst.back() == 2);

    xl::list<int> lst3 = {1, 3};
    lst.merge(lst3);
    assert(lst.size() == 3);
    assert(lst.front() == 1);
    assert(lst3.empty());

    lst.sort();
    assert(lst.front() == 1);
    assert(lst.back() == 3);

    lst.reverse();
    assert(lst.front() == 3);
    assert(lst.back() == 1);
  }

  // erase-remove idiom and xl::erase / xl::erase_if helpers
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

  // xl::erase and xl::erase_if free functions
  {
    xl::list<int> lst = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    xl::erase(lst, 2);
    assert(std::ranges::find(lst, 2) == lst.end());
    assert(!xl::find(lst, 2));

    xl::erase_if(lst, [](int i){ return i % 2 == 0; });
    assert(std::ranges::find_if(lst, [](int i){ return i % 2 == 0; }) == lst.end());
    assert(!xl::find_if(lst, [](int i){ return i % 2 == 0; }));
  }

  // xl::erase and xl::erase_if: verify no remaining matching values
  {
    xl::list<int> myList = {1, 2, 3, 4, 5, 3, 6, 3, 7};

    xl::erase(myList, 3);
    xl::erase_if(myList, [](int value) { return value > 4; });

    assert(std::find(myList.begin(), myList.end(), 3) == myList.end());
    assert(!xl::find(myList, 3));
    assert(std::none_of(myList.begin(), myList.end(),
      [](int value) { return value > 4; }));
  }

  // Push/pop/insert/erase in sequence; forward, range-based, and reverse iteration
  {
    xl::list<int> list1;

    // Test 1: empty on construction
    assert(list1.empty());

    // Test 2: push_back and size
    list1.push_back(1); list1.push_back(2); list1.push_back(3);
    assert(list1.size() == 3);

    // Test 3: front and back
    assert(list1.front() == 1);
    assert(list1.back() == 3);

    // Test 4: insert at begin and end
    list1.insert(list1.begin(), 0);
    list1.insert(list1.end(), 4);
    assert(list1.size() == 5);

    // Test 5: pop_back
    list1.pop_back();
    assert(list1.size() == 4);

    // Test 6: erase at begin
    list1.erase(list1.begin());
    assert(list1.size() == 3);

    // Test 7: forward iterator order
    xl::list<int>::iterator it = list1.begin();
    assert(*it++ == 1);
    assert(*it++ == 2);
    assert(*it++ == 3);

    // Test 8: range-based for
    for (int i : list1)
      assert(i == 1 || i == 2 || i == 3);

    // Test 9: reverse iterator order
    xl::list<int>::reverse_iterator rit = list1.rbegin();
    assert(*rit == 3); rit++;
    assert(*rit == 2); rit++;
    assert(*rit == 1);

    // Test 10: clear
    list1.clear();
    assert(list1.empty());
  }

  // Constructors, copy/move, assignment, swap, relational operators, sort, merge, splice, unique
  {
    // default constructor
    xl::list<int> list1;
    assert(list1.empty());
    assert(list1.size() == 0);

    // initialiser-list constructor
    xl::list<int> list2{1, 2, 3, 4, 5};
    assert(!list2.empty());
    assert(list2.size() == 5);
    assert(list2.front() == 1);
    assert(list2.back() == 5);

    // copy constructor
    xl::list<int> list3(list2);
    assert(!list3.empty());
    assert(list3.size() == 5);
    assert(list3.front() == 1);
    assert(list3.back() == 5);

    // move constructor
    xl::list<int> list4(std::move(list2));
    assert(list2.empty());
    assert(list2.size() == 0);
    assert(!list4.empty());
    assert(list4.size() == 5);
    assert(list4.front() == 1);
    assert(list4.back() == 5);

    // copy assignment
    list2 = list3;
    assert(!list2.empty());
    assert(list2.size() == 5);
    assert(list2.front() == 1);
    assert(list2.back() == 5);

    // move assignment
    list2 = std::move(list3);
    assert(list3.empty());
    assert(list3.size() == 0);
    assert(!list2.empty());
    assert(list2.size() == 5);
    assert(list2.front() == 1);
    assert(list2.back() == 5);

    list2.push_front(0);
    assert(list2.size() == 6);
    assert(list2.front() == 0);
    assert(list2.back() == 5);

    list2.push_back(6);
    assert(list2.size() == 7);
    assert(list2.front() == 0);
    assert(list2.back() == 6);

    list2.pop_front();
    assert(list2.size() == 6);
    assert(list2.front() == 1);
    assert(list2.back() == 6);

    list2.pop_back();
    assert(list2.size() == 5);
    assert(list2.front() == 1);
    assert(list2.back() == 5);

    list2.insert(list2.begin(), 0);
    assert(list2.size() == 6);
    assert(list2.front() == 0);
    assert(list2.back() == 5);

    list2.erase(list2.begin());
    assert(list2.size() == 5);
    assert(list2.front() == 1);
    assert(list2.back() == 5);

    list2.clear();
    assert(list2.empty());
    assert(list2.size() == 0);

    xl::list<int> list5{1, 2, 3};
    list2.swap(list5);
    assert(list2.size() == 3);
    assert(list2.front() == 1);
    assert(list2.back() == 3);
    assert(list5.empty());
    assert(list5.size() == 0);

    assert(list2 > list3);
    assert(list2 >= list3);
    assert(list3 < list2);
    assert(list3 <= list2);
    assert(list2 == list2);
    assert(list2 != list3);

    xl::list<int> list9 = {13, 14, 15, 16, 17};
    list9.reverse();
    assert(list9.size() == 5);
    assert(list9.front() == 17);
    assert(list9.back() == 13);

    xl::list<int> list10 = {18, 15, 20, 12, 19};
    list10.sort();
    assert(list10.size() == 5);
    assert(list10.front() == 12);
    assert(list10.back() == 20);

    xl::list<int> list11 = {21, 23, 25};
    xl::list<int> list12 = {22, 24, 26};
    list11.merge(list12);
    assert(list11.size() == 6);
    assert(list11.front() == 21);
    assert(list11.back() == 26);

    xl::list<int> list13 = {27, 28, 29};
    xl::list<int> list14 = {30, 31, 32};
    list14.splice(list14.begin(), list13);
    assert(list14.size() == 6);
    assert(list14.front() == 27);
    assert(list14.back() == 32);

    xl::list<int> list15 = {33, 34, 34, 35, 35, 36};
    list15.unique();
    assert(list15.size() == 4);
    assert(list15.front() == 33);
    assert(list15.back() == 36);

    // reverse iterator gives all positive values
    xl::list<int> list16 = {37, 38, 39, 40, 41};
    for (auto rit = list16.rbegin(); rit != list16.rend(); ++rit)
      assert(*rit > 0);
  }

  // operator[]
  {
    xl::list l1{1, 2, 3};
    assert(l1[0] == 1);
    assert(l1[1] == 2);
    assert(l1[2] == 3);
  }

  // erase two elements in sequence
  {
    xl::list l2{1, 2, 3, 4, 5};
    auto it = l2.begin();
    l2.erase(std::next(it, 2));
    l2.erase(it);
    assert(l2.size() == 3);
    assert(l2.front() == 2);
    assert(l2.back() == 5);
  }

  // clear non-empty list
  {
    xl::list<int> l3{1, 2, 3, 4, 5};
    l3.clear();
    assert(l3.empty());
    assert(l3.size() == 0);
  }

  // insert at end does not invalidate existing iterator
  {
    xl::list<int> l4{1, 2, 3};
    auto it = l4.begin();
    ++it;                          // points to 2
    l4.insert(l4.end(), 4);
    assert(l4.size() == 4);
    assert(*it == 2);
  }

  // sort
  {
    xl::list<int> l5{18, 15, 20, 12, 19};
    l5.sort();
    assert(!l5.empty());
    assert(l5.size() == 5);
    assert(l5.front() == 12);
    assert(l5.back() == 20);
  }

  // merge two non-empty sorted lists
  {
    xl::list<int> l6{21, 23, 25};
    xl::list<int> l7{22, 24, 26};
    l6.merge(l7);
    assert(!l6.empty());
    assert(l6.size() == 6);
    assert(l6.front() == 21);
    assert(l6.back() == 26);
  }

  // splice two non-empty lists
  {
    xl::list<int> l8{27, 28, 29};
    xl::list<int> l9{30, 31, 32};
    l9.splice(l9.begin(), l8);
    assert(!l9.empty());
    assert(l9.size() == 6);
    assert(l9.front() == 27);
    assert(l9.back() == 32);
  }

  // unique removes consecutive duplicates
  {
    xl::list<int> l10 = {33, 34, 34, 35, 35, 36};
    l10.unique();
    assert(!l10.empty());
    assert(l10.size() == 4);
  }

  // splice empty source into non-empty destination (no-op)
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2;
    lst1.splice(lst1.begin(), lst2);
    assert(lst1.size() == 3);
    assert(lst2.empty());
  }

  // merge with shared values
  {
    xl::list<int> lst1 = {1, 2, 2, 3};
    xl::list<int> lst2 = {2, 4, 5};
    lst1.merge(lst2);
    assert(lst1.size() == 7);
    assert(lst1.front() == 1);
    assert(lst1.back() == 5);
  }

  // unique on empty list
  {
    xl::list<int> lst;
    lst.unique();
    assert(lst.empty());
  }

  // sort single-element list
  {
    xl::list<int> lst = {5};
    lst.sort();
    assert(lst.size() == 1);
    assert(lst.front() == 5);
  }

  // reverse empty list
  {
    xl::list<int> lst;
    lst.reverse();
    assert(lst.empty());
  }

  // insert at end()
  {
    xl::list<int> lst = {1, 2, 3};
    auto it = lst.end();
    lst.insert(it, 4);
    assert(lst.size() == 4);
    assert(lst.back() == 4);
  }

  // assign(count, value)
  {
    xl::list<int> lst;
    lst.assign(5, 100);
    assert(lst.size() == 5);
    assert(lst.front() == 100);
  }

  // merge with duplicates from both sides
  {
    xl::list<int> list11 = {1, 2, 3};
    xl::list<int> list12 = {2, 3, 4};
    list11.merge(list12);
    assert(list11 == xl::list<int>({1, 2, 2, 3, 3, 4}) && list12.empty());
  }

  // merge empty into sorted list with custom comparator
  {
    xl::list<int> list13 = {5, 3, 1};
    xl::list<int> list14;
    list13.merge(list14, [](int a, int b) { return a > b; });
    assert(list13 == xl::list<int>({5, 3, 1}) && list14.empty());
  }

  // merge 1..5 with 6..10
  {
    xl::list<int> list1{1, 2, 3, 4, 5};
    xl::list<int> list2{6, 7, 8, 9, 10};
    list1.merge(list2);
    assert(list1.size() == 10);
    assert(list2.empty());
  }

  // splice range [6,7,8] from list2 to front of list1
  {
    xl::list<int> list1{1, 2, 3, 4, 5};
    xl::list<int> list2{6, 7, 8, 9, 10};
    auto it1 = list2.begin();
    auto it2 = it1;
    std::advance(it2, 3);
    list1.splice(list1.begin(), list2, it1, it2);
    assert(list1.size() == 8);
    assert(list2.size() == 2);
    assert(list1.front() == 6);
  }

  // remove a specific value
  {
    xl::list<int> list1{1, 2, 3, 4, 5};
    list1.remove(3);
    assert(list1.size() == 4);
    assert(list1.front() == 1);
    assert(list1.back() == 5);
  }

  // resize up with fill value
  {
    xl::list<int> list1{1, 2, 3};
    list1.resize(5, 100);
    assert(list1.size() == 5);
    assert(list1.back() == 100);
  }

  // assign(count, value) on initially empty list
  {
    xl::list<int> list1;
    list1.assign(3, 200);
    assert(list1.size() == 3);
    assert(list1.front() == 200);
  }

  // std::ranges::reverse
  {
    xl::list lst{1, 2, 3, 4, 5};
    std::ranges::reverse(lst);
    assert((lst == xl::list{5, 4, 3, 2, 1}));
  }

  // ── test2 content ───────────────────────────────────────────────────────────

  // Default constructor
  {
    xl::list<int> l1;
    assert(l1.empty());
  }

  // Fill constructor
  {
    xl::list<int> l2(5, 100);
    assert(l2.size() == 5);
    assert(l2.front() == 100);
    assert(l2.back() == 100);
  }

  // Iterator-range constructor
  {
    int arr[] = {1, 2, 3, 4, 5};
    xl::list<int> l3(std::begin(arr), std::end(arr));
    assert(l3.size() == 5);
    assert(l3.front() == 1);
    assert(l3.back() == 5);
  }

  // from_range constructor
  {
    int arr[] = {1, 2, 3, 4, 5};
    xl::list l3(xl::from_range, arr);
    assert(l3.size() == 5);
    assert(l3.front() == 1);
    assert(l3.back() == 5);
  }

  // Copy constructor
  {
    xl::list<int> l4{1, 2, 3};
    xl::list<int> l5(l4);
    assert(l5.size() == l4.size());
    assert(l5.front() == l4.front());
    assert(l5.back() == l4.back());
  }

  // Move constructor
  {
    xl::list<int> l6{1, 2, 3};
    xl::list<int> l7(std::move(l6));
    assert(l6.empty());
    assert(l7.size() == 3);
  }

  // Copy assignment
  {
    xl::list<int> l8{1, 2, 3};
    xl::list<int> l9;
    l9 = l8;
    assert(l9.size() == l8.size());
    assert(l9.front() == l8.front());
    assert(l9.back() == l8.back());
  }

  // Move assignment
  {
    xl::list<int> l10{1, 2, 3};
    xl::list<int> l11;
    l11 = std::move(l10);
    assert(l10.empty());
    assert(l11.size() == 3);
  }

  // empty()
  {
    xl::list<int> l12;
    assert(l12.empty());
    l12.push_back(1);
    assert(!l12.empty());
  }

  // size()
  {
    xl::list<int> l13;
    assert(l13.size() == 0);
    l13.push_back(1);
    assert(l13.size() == 1);
  }

  // max_size()
  {
    xl::list<int> l14;
    assert(l14.max_size() > 0);
  }

  // front()
  {
    xl::list<int> l15{1, 2, 3};
    assert(l15.front() == 1);
  }

  // back()
  {
    xl::list<int> l16{1, 2, 3};
    assert(l16.back() == 3);
  }

  // push_front()
  {
    xl::list<int> l17;
    l17.push_front(1);
    assert(l17.front() == 1);
    assert(l17.size() == 1);
  }

  // pop_front()
  {
    xl::list<int> l18{1, 2, 3};
    l18.pop_front();
    assert(l18.front() == 2);
    assert(l18.size() == 2);
  }

  // push_back()
  {
    xl::list<int> l19;
    l19.push_back(1);
    assert(l19.back() == 1);
    assert(l19.size() == 1);
  }

  // pop_back()
  {
    xl::list<int> l20{1, 2, 3};
    l20.pop_back();
    assert(l20.back() == 2);
    assert(l20.size() == 2);
  }

  // insert() single element: returns iterator to the new element
  {
    xl::list<int> l21{1, 2, 3};
    auto it = l21.insert(l21.begin(), 100);
    assert(*it == 100);
    assert(l21.front() == 100);
    assert(l21.size() == 4);
  }

  // insert() fill (count, value)
  {
    xl::list<int> l22{1, 2, 3};
    l22.insert(l22.begin(), 2, 100);
    assert(l22.front() == 100);
    assert(l22.size() == 5);
  }

  // insert() iterator range
  {
    xl::list<int> l23{1, 2, 3};
    int arr[] = {100, 200};
    l23.insert(l23.begin(), std::begin(arr), std::end(arr));
    assert(l23.front() == 100);
    assert(l23.size() == 5);
  }

  // erase() single element: returns iterator to element after erased one
  {
    xl::list<int> l24{1, 2, 3};
    auto it = l24.erase(l24.begin());
    assert(*it == 2);
    assert(l24.front() == 2);
    assert(l24.size() == 2);
  }

  // erase() range
  {
    xl::list<int> l25{1, 2, 3, 4, 5};
    auto it = l25.erase(l25.begin(), std::next(l25.begin(), 3));
    assert(*it == 4);
    assert(l25.size() == 2);
  }

  // swap()
  {
    xl::list<int> l26{1, 2, 3};
    xl::list<int> l27{4, 5, 6};
    l26.swap(l27);
    assert(l26.front() == 4);
    assert(l27.front() == 1);
  }

  // clear()
  {
    xl::list<int> l28{1, 2, 3};
    l28.clear();
    assert(l28.empty());
  }

  // splice() entire list
  {
    xl::list<int> l29{1, 2, 3};
    xl::list<int> l30{4, 5, 6};
    l29.splice(l29.begin(), std::move(l30));
    assert(l29.size() == 6);
    assert(l30.empty());
  }

  // splice() single element
  {
    xl::list<int> l31{1, 2, 3};
    xl::list<int> l32{4, 5, 6};
    l31.splice(l31.begin(), std::move(l32), l32.begin());
    assert(l31.front() == 4);
    assert(l31.size() == 4);
    assert(l32.size() == 2);
  }

  // splice() element range
  {
    xl::list<int> l33{1, 2, 3};
    xl::list<int> l34{4, 5, 6};
    l33.splice(l33.begin(), std::move(l34), std::next(l34.begin()), l34.end());
    assert(l33.size() == 5);
    assert(l34.size() == 1);
  }

  // remove()
  {
    xl::list<int> l35{1, 2, 1, 3, 1, 4};
    l35.remove(1);
    assert(l35.size() == 3);
    assert(l35.front() == 2);
  }

  // remove_if()
  {
    xl::list<int> l36{1, 2, 3, 4, 5};
    l36.remove_if([](int x) { return x % 2 == 0; });
    assert(l36.size() == 3);
    assert(l36.front() == 1);
  }

  // unique()
  {
    xl::list<int> l37{1, 2, 2, 3, 3, 3};
    l37.unique();
    assert(l37.size() == 3);
  }

  // merge()
  {
    xl::list<int> l38{1, 3, 5};
    xl::list<int> l39{2, 4, 6};
    l38.merge(std::move(l39));
    assert(l38.size() == 6);
    assert(l38.front() == 1);
    assert(l38.back() == 6);
  }

  // reverse()
  {
    xl::list<int> l40{1, 2, 3};
    l40.reverse();
    assert(l40.front() == 3);
    assert(l40.back() == 1);
  }

  // sort()
  {
    xl::list<int> l41{3, 1, 2};
    l41.sort();
    assert(l41.front() == 1);
    assert(l41.back() == 3);
  }

  // xl::find, xl::erase (variadic)
  {
    xl::list<int> l42{3, 1, 2};
    *xl::find(l42, 3) = 1;
    assert(l42.front() == 1);
    xl::erase(l42, 3, 2, 1);
    assert(l42.empty());
  }

  // ── test3 content ───────────────────────────────────────────────────────────

  // emplace_back / emplace_front / emplace at position
  {
    xl::list<std::pair<int, int>> lst;
    lst.emplace_back(1, 2);
    lst.emplace_front(3, 4);
    auto it = lst.begin();
    ++it;
    it = lst.emplace(it, 5, 6);

    assert(lst.size() == 3);
    assert(lst.front() == std::make_pair(3, 4));
    assert(lst.back()  == std::make_pair(1, 2));
    assert(*it         == std::make_pair(5, 6));
  }

  // assign with initialiser list
  {
    xl::list<int> lst = {1, 2, 3};
    lst.assign({4, 5, 6, 7});
    assert(lst.size() == 4);
    assert(lst.front() == 4);
    assert(lst.back() == 7);
  }

  // swap between empty and non-empty
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2;
    lst1.swap(lst2);
    assert(lst1.empty());
    assert(lst2.size() == 3);
  }

  // sort with custom (descending) comparator
  {
    xl::list<int> lst = {3, 1, 4, 2, 5};
    lst.sort(std::greater<int>());
    assert((lst == xl::list<int>{5, 4, 3, 2, 1}));
  }

  // double reverse is identity
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    lst.reverse();
    lst.reverse();
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
  }

  // remove non-existing value is a no-op
  {
    xl::list<int> lst = {1, 2, 3};
    lst.remove(42);
    assert(lst.size() == 3);
  }

  // self-splice: move a suffix to the front
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it = lst.begin();
    std::advance(it, 2);
    lst.splice(lst.begin(), lst, it, lst.end());
    assert((lst == xl::list<int>{3, 4, 5, 1, 2}));
  }

  // self-splice: identity cases
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    lst.splice(lst.begin(), lst, lst.begin());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
    lst.splice(std::next(lst.begin()), lst, lst.begin());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
    lst.splice(lst.end(), lst, lst.begin(), lst.end());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
  }

  // unique after sort removes non-consecutive duplicates
  {
    xl::list<int> lst = {1, 2, 1, 3, 3, 2, 4};
    lst.sort();
    lst.unique();
    assert((lst == xl::list<int>{1, 2, 3, 4}));
  }

  // merge with partially overlapping ranges
  {
    xl::list<int> lst1 = {1, 3, 5};
    xl::list<int> lst2 = {2, 3, 4, 6};
    lst1.merge(lst2);
    assert((lst1 == xl::list<int>{1, 2, 3, 3, 4, 5, 6}));
    assert(lst2.empty());
  }

  // exception safety: push_back where constructor throws
  {
    struct TestException : std::exception {};
    struct Thrower {
      Thrower() = default;
      Thrower(int) { throw TestException(); }
    };

    xl::list<Thrower> lst;
    lst.push_back(Thrower{});         // succeeds

    try {
      lst.push_back(42);             // throws
      assert(false && "exception not thrown");
    } catch (const TestException&) {
      assert(lst.size() == 1);
    }
  }

  // const iterator traversal
  {
    const xl::list<int> lst = {1, 2, 3};
    int sum = 0;
    for (auto it = lst.cbegin(); it != lst.cend(); ++it)
      sum += *it;
    assert(sum == 6);
  }

  // large list: push_back, front/back, sum via iterator
  {
    xl::list<size_t> large_list;
    const size_t count = 100000;
    for (size_t i = 0; i < count; ++i)
      large_list.push_back(i);
    assert(large_list.size() == count);
    assert(large_list.front() == 0);
    assert(large_list.back() == count - 1);

    size_t sum = 0;
    for (auto it = large_list.begin(); it != large_list.end(); ++it)
      sum += *it;
    assert(sum == (count - 1) * count / 2);
  }

  // move semantics with large element type
  {
    struct LargeData {
      std::array<int, 1024> buffer;
      LargeData(int val) { buffer.fill(val); }
    };

    xl::list<LargeData> source;
    source.emplace_back(1);
    source.emplace_back(2);

    xl::list<LargeData> dest = std::move(source);
    assert(source.empty());
    assert(dest.size() == 2);
    assert(dest.front().buffer[0] == 1);
    assert(dest.back().buffer[0]  == 2);
  }

  // exception safety: insert where copy constructor throws
  {
    struct ThrowOnCopy {
      int value;
      ThrowOnCopy(int v) : value(v) {}
      ThrowOnCopy(const ThrowOnCopy& other) : value(other.value) {
        if (value == 42) throw std::runtime_error("copy failed");
      }
    };

    xl::list<ThrowOnCopy> lst;
    lst.emplace_back(1);
    lst.emplace_back(2);

    try {
      lst.insert(lst.begin(), ThrowOnCopy(42));
      assert(false && "exception not thrown");
    } catch (const std::runtime_error&) {
      assert(lst.size() == 2);
      assert(lst.front().value == 1);
    }
  }

  // stateful comparator for sort (partition by threshold)
  {
    struct StatefulComparator {
      int threshold;
      bool operator()(int a, int b) const {
        return (a > threshold) < (b > threshold);
      }
    };

    StatefulComparator comp{5};
    xl::list<int> lst = {10, 2, 8, 1, 6, 12};
    lst.sort(comp);

    // all values > 5 are grouped at the back
    auto it = lst.begin();
    while (it != lst.end() && *it <= 5) ++it;
    while (it != lst.end()) { assert(*it > 5); ++it; }
  }

  // self-splice: move single element within the same list
  {
    xl::list<int> lst = {1, 2, 4, 5, 3};

    // move element 3 (last) before element 4 (index 2)
    auto it = lst.begin();
    std::advance(it, 2);
    lst.splice(it, lst, std::prev(lst.end()));
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));

    lst.splice(lst.begin(), lst, std::prev(lst.end()));
    assert((lst == xl::list<int>{5, 1, 2, 3, 4}));
  }

  // merge with custom-comparable struct
  {
    struct Value {
      int priority;
      int id;
      bool operator<(const Value& other) const {
        return priority < other.priority;
      }
    };

    xl::list<Value> list1 = {{1, 101}, {3, 103}, {5, 105}};
    xl::list<Value> list2 = {{2, 202}, {4, 204}, {6, 206}};

    list1.merge(list2);
    int last_priority = 0;
    for (const auto& val : list1) {
      assert(val.priority > last_priority);
      last_priority = val.priority;
    }
    assert(list2.empty());
  }

  // sort correctness: reverse-sorted input of 10 000 elements
  {
    xl::list<int> perf_list;
    const int count = 10000;
    for (int i = count; i > 0; --i)
      perf_list.push_back(i);

    perf_list.sort();
    assert(perf_list.front() == 1);
    assert(perf_list.back() == count);

    int expected = 1;
    for (const auto& val : perf_list)
      assert(val == expected++);
  }

  // heterogeneous comparison (int vs long)
  {
    xl::list<int>  lst1 = {1, 2, 3};
    xl::list<long> lst2 = {1, 2, 3};
    xl::list<int>  lst3 = {1, 2, 4};

    assert(lst1 == lst2);
    assert(lst1 != lst3);
    assert(lst1 <  lst3);
    assert(lst3 >  lst1);
  }

  // emplace at begin, end, and middle with std::string
  {
    xl::list<std::string> lst = {"world"};
    lst.emplace(lst.begin(), "hello");
    lst.emplace(lst.end(),   "!");

    assert(lst.size() == 3);
    assert(lst.front() == "hello");
    assert(lst.back()  == "!");

    auto it = lst.begin();
    ++it;
    it = lst.emplace(it, "there");
    assert(*it == "there");
    assert((lst == xl::list<std::string>{"hello", "there", "world", "!"}));
  }

  // initialiser-list assignment and assign()
  {
    xl::list<int> lst;
    lst = {1, 2, 3, 4, 5};
    assert(lst.size() == 5);
    assert(lst.back() == 5);

    lst.assign({6, 7, 8});
    assert(lst.size() == 3);
    assert(lst.front() == 6);
  }

  // reverse iterators accumulate digits (54321)
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    int sum = 0;
    for (auto rit = lst.rbegin(); rit != lst.rend(); ++rit)
      sum = sum * 10 + *rit;
    assert(sum == 54321);
  }

  // swap between two large lists
  {
    xl::list<int> listA, listB;
    const int count = 100000;
    for (int i = 0; i < count; ++i) {
      listA.push_back(i);
      listB.push_back(i * 2);
    }
    listA.swap(listB);
    assert(listA.front() == 0);
    assert(listB.front() == 0);
  }

  // std::string list: push_back, move and copy semantics
  {
    xl::list<std::string> lst;
    lst.push_back("hello");
    lst.push_back("world");
    assert(lst.size() == 2);
    assert(lst.front() == "hello");
    assert(lst.back()  == "world");

    xl::list<std::string> lst2 = std::move(lst);
    assert(lst.empty());
    assert(lst2.size() == 2);
    assert(lst2.front() == "hello");
    assert(lst2.back()  == "world");

    lst = std::move(lst2);
    assert(lst2.empty());
    assert(lst.size() == 2);
    assert(lst.front() == "hello");
    assert(lst.back()  == "world");

    xl::list<std::string> lst3 = lst;
    assert(lst3.size() == 2);
    assert(lst3.front() == "hello");
    assert(lst.size() == 2);            // original unchanged

    xl::list<std::string> lst4;
    lst4 = lst;
    assert(lst4.size() == 2);
    assert(lst4.front() == "hello");

    lst3.swap(lst4);
    assert(lst3.size() == 2);
    assert(lst4.size() == 2);
  }

  // iterator validity: erase does not invalidate other iterators
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it  = lst.begin();
    ++it;                              // points to 2
    auto it2 = std::next(it);         // points to 3

    lst.erase(it);                     // it is now invalid
    assert(*it2 == 3);                 // it2 survives

    // insert does not invalidate existing iterators
    auto it3 = lst.begin();            // points to 1
    lst.insert(std::next(it3), 10);
    assert(*it3 == 1);
    assert(*it2 == 3);
  }

  // resource management: destructor balances constructor calls
  {
    static int counter = 0;
    struct Counted {
      Counted()                { ++counter; }
      Counted(const Counted&)  { ++counter; }
      ~Counted()               { --counter; }
    };

    {
      xl::list<Counted> lst;
      lst.push_back(Counted());
      lst.push_back(Counted());
      assert(counter == 2);
    }
    assert(counter == 0);

    {
      xl::list<Counted> lst;
      lst.push_back(Counted());
      lst.push_back(Counted());
      xl::list<Counted> lst2 = std::move(lst);
      assert(counter == 2);
    }
    assert(counter == 0);
  }

  // emplace with multi-argument constructor (Person)
  {
    struct Person {
      std::string name;
      int age;
      Person(std::string n, int a) : name(std::move(n)), age(a) {}
    };

    xl::list<Person> lst;
    lst.emplace_back("Alice", 30);
    lst.emplace_front("Bob", 25);
    auto it = lst.begin();
    ++it;
    it = lst.emplace(it, "Charlie", 40);

    assert(lst.size() == 3);
    assert(lst.front().name == "Bob");
    assert(lst.back().name  == "Alice");
    assert(it->name == "Charlie");
    assert(it->age  == 40);
  }

  // reverse iterators (second independent check)
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    int sum = 0;
    for (auto rit = lst.rbegin(); rit != lst.rend(); ++rit)
      sum = sum * 10 + *rit;
    assert(sum == 54321);
  }

  // single-element list: all operations
  {
    xl::list<int> lst;
    lst.push_back(42);

    assert(!lst.empty());
    assert(lst.size() == 1);
    assert(lst.front() == 42);
    assert(lst.back()  == 42);

    lst.pop_front();
    assert(lst.empty());

    lst.push_front(100);
    lst.pop_back();
    assert(lst.empty());

    lst.push_back(200);
    lst.erase(lst.begin());
    assert(lst.empty());
  }

  // self-assignment
  {
    xl::list<int> lst = {1, 2, 3};
    lst = lst;
    assert(lst.size() == 3);
    assert(lst.front() == 1);
    assert(lst.back()  == 3);
  }

  // comparison operators: equal, not-equal, less, greater, <=, >=
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2 = {1, 2, 3};
    xl::list<int> lst3 = {4, 5, 6};
    xl::list<int> lst4 = {1, 2};

    assert(lst1 == lst2);
    assert(lst1 != lst3);
    assert(lst1 != lst4);
    assert(lst4 <  lst1);
    assert(lst3 >  lst1);
    assert(lst1 <= lst2);
    assert(lst1 >= lst2);
  }

  // initialiser-list assignment (second check)
  {
    xl::list<int> lst;
    lst = {1, 2, 3, 4, 5};
    assert(lst.size() == 5);
    assert(lst.back() == 5);

    lst.assign({6, 7, 8});
    assert(lst.size() == 3);
    assert(lst.front() == 6);
  }

  // iterator validity around erase and insert
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it1 = lst.begin();
    auto it2 = std::next(it1);
    auto it3 = std::next(it2);

    it3 = lst.erase(it2);
    assert(*it1 == 1);
    assert(*it3 == 3);              // returned by erase()

    auto it4 = lst.insert(it3, 10);
    assert(*it3 == 3);
    assert(*it4 == 10);

    xl::list<int> lst2 = {20, 30};
    lst.splice(it3 = std::next(it4), lst2);
    assert(*it3 == 3);
    assert(lst.size() == 7);
  }

  // sort with case-insensitive string comparator
  {
    auto case_insensitive = [](const std::string& a, const std::string& b) {
      return std::lexicographical_compare(
        a.begin(), a.end(), b.begin(), b.end(),
        [](char c1, char c2) {
          return std::tolower(c1) < std::tolower(c2);
        });
    };

    xl::list<std::string> words = {"Banana", "apple", "Carrot", "date"};
    words.sort(case_insensitive);
    assert((words == xl::list<std::string>{"apple", "Banana", "Carrot", "date"}));
  }

  // splice between two lists then merge
  {
    xl::list<int> a = {1, 2, 3};
    xl::list<int> b = {4, 5, 6};
    auto it = a.begin();
    std::advance(it, 2);

    b.splice(b.begin(), a, a.begin(), it);
    assert(a.size() == 1);
    assert(a.front() == 3);
    assert(b.size() == 5);
    assert(b.front() == 1);

    a = {3, 5, 7};
    b = {2, 4, 6};
    a.merge(b);
    assert(b.empty());
    assert((a == xl::list<int>{2, 3, 4, 5, 6, 7}));
  }

  // large list: reverse via rbegin iterator, then sort
  {
    constexpr size_t N = 100'000;
    xl::list<size_t> large;
    for (size_t i = 0; i < N; ++i)
      large.push_back(i);
    assert(large.size() == N);

    xl::list<size_t> reversed;
    for (auto it = large.rbegin(); it != large.rend(); ++it)
      reversed.push_back(*it);
    assert(reversed.front() == N - 1);

    large.sort();
    assert(large.front() == 0);
    assert(large.back()  == N - 1);
  }

  // std::advance forward and backward; make_reverse_iterator
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it = lst.begin();
    std::advance(it, 3);
    assert(*it == 4);

    std::advance(it, -2);
    assert(*it == 2);

    auto rit = std::make_reverse_iterator(it);
    assert(*rit == 1);

    std::advance(rit, -2);
    assert(*rit == 3);
  }

  // move-only type (unique_ptr)
  {
    xl::list<std::unique_ptr<int>> ptr_list;
    ptr_list.push_back(std::make_unique<int>(42));
    ptr_list.emplace_back(new int(100));

    assert(**ptr_list.begin() == 42);

    xl::list<std::unique_ptr<int>> moved_list = std::move(ptr_list);
    assert(ptr_list.empty());
    assert(moved_list.size() == 2);

    xl::list<std::unique_ptr<int>> new_list(std::move(moved_list));
    assert(moved_list.empty());
    assert(new_list.size() == 2);
  }

  // range constructors: empty range, single element, input iterator
  {
    std::vector<int> empty_vec;
    xl::list<int> empty_list(empty_vec.begin(), empty_vec.end());
    assert(empty_list.empty());

    int single = 42;
    xl::list<int> single_list(&single, &single + 1);
    assert(single_list.size() == 1);
    assert(single_list.front() == 42);

    std::istringstream iss("1 2 3 4");
    std::istream_iterator<int> iit(iss), eos;
    xl::list<int> input_list(iit, eos);
    assert(input_list.size() == 4);
    assert(input_list.back() == 4);
  }

  // spaceship / relational operator completeness
  {
    xl::list<int> a = {1, 2, 3};
    xl::list<int> b = {1, 2, 3};
    xl::list<int> c = {1, 2, 4};
    xl::list<int> d = {1, 2};

    assert(a == b);
    assert(a != c);
    assert(a != d);
    assert(d <  a);
    assert(c >  a);
    assert(d <= a);
    assert(a >= b);
    assert(c >= a);
  }

  // range-constructed list equals C array
  {
    int const a[10]{};
    assert(std::ranges::equal(a, xl::list(a)));
  }

  // copy construction preserves source
  {
    xl::list<int> src = {7, 8, 9};
    auto dst(src);
    assert((dst == xl::list<int>{7, 8, 9}));
    assert((src == xl::list<int>{7, 8, 9}));   // source unchanged
  }

  // move construction from prvalue
  {
    xl::list<int> dst(xl::list<int>{1, 2, 3});
    assert((dst == xl::list<int>{1, 2, 3}));
  }

  // copy-assign then move-assign from prvalue
  {
    xl::list<int> a = {4, 5};
    xl::list<int> b = {6, 7, 8};

    a = b;
    assert((a == xl::list<int>{6, 7, 8}));
    assert((b == xl::list<int>{6, 7, 8}));     // b unchanged

    b = xl::list<int>{9, 10};
    assert((b == xl::list<int>{9, 10}));
  }

  // iterator stability after copy and move
  {
    xl::list<int> original = {10, 20, 30, 40, 50};
    auto it = std::next(original.begin(), 2);   // points to 30
    assert(*it == 30);

    xl::list<int> copy = original;
    *it = 99;
    assert((original == xl::list<int>{10, 20, 99, 40, 50}));
    assert((copy     == xl::list<int>{10, 20, 30, 40, 50}));  // copy untouched

    xl::list<int> moved = std::move(original);
    assert(original.empty());                                  // NOLINT(bugprone-use-after-move)
    assert((moved == xl::list<int>{10, 20, 99, 40, 50}));
  }

  // self-assignment guard
  {
    xl::list<int> lst = {11, 22, 33};
    lst = lst;
    assert((lst == std::array{11, 22, 33}));
  }

  // iterator arithmetic: decrement from end, increment back to end, std::distance
  {
    xl::list lst = {1, 2, 3, 4, 5};

    auto end_it = lst.end();
    --end_it;
    assert(*end_it == 5);

    ++end_it;
    assert(end_it == lst.end());

    assert(std::distance(lst.begin(), lst.end()) == 5);
  }

  // const list: size, front, back, empty, cbegin/cend
  {
    const xl::list<int> const_lst = {10, 20, 30};
    assert(const_lst.size() == 3);
    assert(const_lst.front() == 10);
    assert(const_lst.back()  == 30);
    assert(!const_lst.empty());

    auto cit = const_lst.cbegin();
    assert(*cit == 10);

    int sum = 0;
    for (auto it = const_lst.cbegin(); it != const_lst.cend(); ++it)
      sum += *it;
    assert(sum == 60);
  }

  // repeated push/pop cycles (stress test for allocator)
  {
    xl::list<int> lst;
    for (int cycle = 0; cycle < 100; ++cycle) {
      for (int i = 0; i < 10; ++i) lst.push_back(i);
      for (int i = 0; i <  5; ++i) lst.pop_back();
    }
    assert(lst.size() == 500);   // 100 cycles * 5 net additions
    lst.clear();
    assert(lst.empty());
  }

  // iterator validity after erase of middle element
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    std::vector<decltype(lst.begin())> iterators;
    for (auto it = lst.begin(); it != lst.end(); ++it)
      iterators.push_back(it);

    auto middle_it = std::next(lst.begin(), 2);
    lst.erase(middle_it);

    assert(*iterators[0] == 1);
    assert(*iterators[1] == 2);
    // iterators[2] is invalidated
    assert(*iterators[3] == 4);
    assert(*iterators[4] == 5);
  }

  // splice edge cases: empty range, self no-op, move suffix
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2 = {4, 5, 6};

    // splice empty range is a no-op
    lst1.splice(lst1.end(), lst2, lst2.end(), lst2.end());
    assert(lst1.size() == 3);
    assert(lst2.size() == 3);

    // splice single element to itself (no-op)
    auto it = std::next(lst1.begin());
    lst1.splice(it, lst1, it);
    assert((lst1 == xl::list<int>{1, 2, 3}));

    // splice suffix before the first element
    lst1.splice(lst1.begin(), lst1, std::next(lst1.begin()), lst1.end());
    assert((lst1 == xl::list<int>{2, 3, 1}));
  }

  // merge with descending comparator
  {
    xl::list<int> lst1 = {5, 3, 1};
    xl::list<int> lst2 = {6, 4, 2};
    lst1.merge(lst2, std::greater<int>());
    assert((lst1 == xl::list<int>{6, 5, 4, 3, 2, 1}));
    assert(lst2.empty());
  }

  // unique with default and custom predicate
  {
    xl::list<int> lst = {1, 2, 2, 3, 4, 4, 4, 5};

    auto original_size = lst.size();
    auto removed = lst.unique();
    assert(removed > 0);
    assert(lst.size() < original_size);
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));

    // custom predicate: remove elements differing by at most 1
    lst = {1, 2, 4, 5, 7, 8};
    lst.unique([](int a, int b) { return std::abs(a - b) <= 1; });
    assert(lst.size() < 6);
  }

  // remove_if: keep only odd numbers
  {
    xl::list<int> lst = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    lst.remove_if([](int x) { return x % 2 == 0; });
    assert(lst.size() == 5);
    for (const auto& val : lst)
      assert(val % 2 == 1);
  }

  // large list: sequential fill, xl::find, reverse, sort
  {
    const size_t N = 50000;
    xl::list<size_t> large_list;
    for (size_t i = 0; i < N; ++i)
      large_list.push_back(i);
    assert(large_list.size() == N);

    auto found = xl::find(large_list, N / 2);
    assert(found && *found == N / 2);

    large_list.reverse();
    assert(large_list.front() == N - 1);
    assert(large_list.back()  == 0);

    large_list.sort();
    assert(large_list.front() == 0);
    assert(large_list.back()  == N - 1);
  }

  // fragmented push/pop pattern preserves size and sortability
  {
    xl::list<int> lst;
    for (int i = 0; i < 1000; ++i) {
      lst.push_back(i);
      if (i % 3 == 0) lst.pop_front();
    }
    assert(!lst.empty());
    auto size = lst.size();
    lst.sort();
    assert(lst.size() == size);
  }

  // Container concept: static_assert on type aliases and iterator categories
  {
    xl::list<int> lst = {1, 2, 3};
    static_assert(std::is_same_v<xl::list<int>::value_type,       int>);
    static_assert(std::is_same_v<xl::list<int>::reference,        int&>);
    static_assert(std::is_same_v<xl::list<int>::const_reference,  const int&>);
    static_assert(std::bidirectional_iterator<xl::list<int>::iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::const_iterator>);
  }

  // SequenceContainer construction forms
  {
    xl::list<int> lst1;
    xl::list<int> lst2(5);
    xl::list<int> lst3(5, 42);
    xl::list<int> lst4{1, 2, 3, 4, 5};

    assert(lst1.empty());
    assert(lst2.size() == 5);
    assert(lst3.size() == 5 && lst3.front() == 42);
    assert(lst4.size() == 5 && lst4.back()  == 5);
  }

  // max_size; std::is_sorted after sort
  {
    xl::list<int> lst;
    assert(lst.max_size() > 0);

    lst = {5, 2, 8, 1, 9};
    assert(!std::is_sorted(lst.begin(), lst.end()));

    lst.sort();
    assert(std::is_sorted(lst.begin(), lst.end()));
  }

  // rotate via splice (forward and back)
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};

    auto it = std::next(lst.begin(), 2);
    lst.splice(lst.end(), lst, lst.begin(), it);
    assert((lst == xl::list<int>{3, 4, 5, 1, 2}));

    it = std::next(lst.begin(), 3);
    lst.splice(lst.begin(), lst, it, lst.end());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
  }

  // nested list: construct, access, flatten
  {
    xl::list<xl::list<int>> nested;
    nested.push_back({1, 2, 3});
    nested.push_back({4, 5, 6});
    nested.push_back({7, 8, 9});

    assert(nested.size() == 3);
    assert(nested.front().size() == 3);
    assert(nested.back().back()  == 9);

    xl::list<int> flattened;
    for (const auto& inner : nested)
      for (const auto& val : inner)
        flattened.push_back(val);
    assert(flattened.size() == 9);
    assert(flattened.back() == 9);
  }

  // independent lists: push, move via splice
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2 = {4, 5, 6};

    lst1.push_back(7);
    lst2.push_front(0);
    assert(lst1.back()  == 7);
    assert(lst2.front() == 0);

    lst1.splice(lst1.end(), std::move(lst2));
    assert(lst2.empty());
    assert(lst1.size() == 8);
  }

  // std::accumulate, std::transform, std::count_if
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};

    auto sum = std::accumulate(lst.begin(), lst.end(), 0);
    assert(sum == 15);

    std::transform(lst.begin(), lst.end(), lst.begin(),
                   [](int x) { return x * 2; });
    assert(lst.front() == 2);
    assert(lst.back()  == 10);

    auto even_count = std::count_if(lst.begin(), lst.end(),
                                    [](int x) { return x % 2 == 0; });
    assert(even_count == 5);   // all are even after multiplying by 2
  }

  // ── Additional tests ────────────────────────────────────────────────────────

  // assign(iter, iter): replace contents using an iterator range
  {
    xl::list lst = {1, 2, 3, 4, 5};
    int arr[] = {10, 20, 30};
    lst = arr;
    assert(lst.size() == 3);
    assert(lst.front() == 10);
    assert(lst.back()  == 30);
  }

  // insert(pos, count, value): multi-element fill insert
  {
    xl::list lst = {1, 2, 3};
    lst.insert(std::next(lst.begin()), 3, 99);
    assert(lst.size() == 6);
    assert(lst.front() == 1);
    // three 99s follow the first element
    auto it = std::next(lst.begin());
    assert(*it == 99); ++it;
    assert(*it == 99); ++it;
    assert(*it == 99); ++it;
    assert(*it == 2);
  }

  // sort stability: elements with equal keys retain relative order
  {
    struct Item { int key; int order; };

    xl::list<Item> lst;
    lst.push_back({1, 0});
    lst.push_back({2, 1});
    lst.push_back({1, 2});
    lst.push_back({2, 3});
    lst.push_back({1, 4});

    lst.sort([](const Item& a, const Item& b) { return a.key < b.key; });

    // all key=1 items should come first, preserving insertion order
    auto it = lst.begin();
    assert(it->key == 1 && it->order == 0); ++it;
    assert(it->key == 1 && it->order == 2); ++it;
    assert(it->key == 1 && it->order == 4); ++it;
    assert(it->key == 2 && it->order == 1); ++it;
    assert(it->key == 2 && it->order == 3);
  }

  // operator[] write access
  {
    xl::list lst = {1, 2, 3, 4, 5};
    lst[0] = 100;
    lst[4] = 500;
    assert(lst.front() == 100);
    assert(lst.back()  == 500);
    assert(lst[2] == 3);    // middle element unchanged
  }

  // resize(0) empties a non-empty list
  {
    xl::list lst = {1, 2, 3, 4, 5};
    lst.resize(0);
    assert(lst.empty());
    assert(lst.size() == 0);
  }

  // bidirectional traversal: forward and backward yield the same elements
  {
    xl::list lst = {10, 20, 30, 40, 50};
    std::vector<int> forward_pass, backward_pass;

    for (auto it = lst.begin();  it != lst.end();  ++it) forward_pass.push_back(*it);
    for (auto it = lst.rbegin(); it != lst.rend(); ++it) backward_pass.push_back(*it);

    assert(forward_pass.size() == backward_pass.size());
    for (size_t i = 0; i < forward_pass.size(); ++i)
      assert(forward_pass[i] == backward_pass[forward_pass.size() - 1 - i]);
  }

  // chained operations: sort + unique + reverse
  {
    xl::list lst = {5, 1, 3, 1, 4, 2, 5, 3};
    lst.sort();
    lst.unique();
    lst.reverse();
    assert((lst == std::array{5, 4, 3, 2, 1}));
  }

  // erase(begin, begin) is a no-op (empty range)
  {
    xl::list lst = {1, 2, 3};
    auto it = lst.erase(lst.begin(), lst.begin());
    assert(lst.size() == 3);
    assert(it == lst.begin());
  }

  // std::distance between const_iterators equals size
  {
    const xl::list lst = {1, 2, 3, 4, 5};
    auto dist = std::distance(lst.cbegin(), lst.cend());
    assert(dist == lst.size());
  }

  // copy independence: modifying the copy does not affect the original
  {
    xl::list original = {1, 2, 3};
    xl::list copy = original;

    copy.push_back(4);
    copy[0] = 99;

    assert(original.size()  == 3);
    assert(original.front() == 1);   // unaffected
    assert(copy.size()      == 4);
    assert(copy.front()     == 99);
  }

  // push_back / push_front with variadic arguments
  {
    xl::list<int> lst;
    lst.push_back(1, 2, 3);
    lst.push_front(0);
    assert(lst.size() == 4);
    assert(lst.front() == 0);
    assert(lst.back()  == 3);
    assert((lst == std::array{0, 1, 2, 3}));
  }

  // splice from empty source into non-empty destination range overload
  {
    xl::list dst = {1, 2, 3};
    xl::list<int> src;
    dst.splice(dst.begin(), src, src.begin(), src.end());
    assert(dst.size() == 3);
    assert(src.empty());
  }

  // large sort + find_if via standard algorithm
  {
    xl::list<int> lst;
    for (int i = 99; i >= 0; --i)
      lst.push_back(i);
    lst.sort();

    auto it = std::find_if(lst.begin(), lst.end(), [](int x) { return x > 50; });
    assert(it);
    assert(*it == 51);
    assert(std::is_sorted(lst.begin(), lst.end()));
  }
}

int main()
{
  test();
  std::cout << "All tests passed!\n";
  return 0;
}

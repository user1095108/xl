#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>
#include "list.hpp" // Replace with the actual container header

// Include your testing framework of choice (e.g., Google Test or Catch2)

// Function to test various operations of your random-access container
void test1() {
  {
  // Create an instance of your container
  xl::list<int> container;

  // Test adding elements to the container
  container.push_back(1, 2, 3);

  // Test the size of the container
  assert(container.size() == 3);

  // Test accessing elements by index
  assert(container[0] == 1);
  assert(container[1] == 2);
  assert(container[2] == 3);

  // Test modifying elements
  container[1] = 42;
  assert(container[1] == 42);

  // Test removing elements
  container.pop_back();
  assert(container.size() == 2);

  // Add more test cases for your container's methods (e.g., clear, resize, etc.)

  // Test iterator functionality
  auto it = container.begin();
  assert(*it == 1);
  ++it;
  assert(*it == 42);
  ++it;
  assert(it == container.end());

  // Test edge cases
  container.clear();
  assert(container.empty());
  assert(container.size() == 0);

  // Additional test cases:

  // Test inserting elements at a specific position
  container.push_back(4);
  container.insert(std::next(container.begin()), 5);
  assert(container.size() == 2);
  assert(container[0] == 4);
  assert(container[1] == 5);

  // Test erasing elements at a specific position
  container.erase(container.begin());
  assert(container.size() == 1);
  assert(container[0] == 5);

  // Test swapping containers
  decltype(container) otherContainer;
  otherContainer.push_back(6, 7);
  container.swap(otherContainer);
  assert(container.size() == 2);
  assert(otherContainer.size() == 1);

  // Test copying a container
  decltype(container) copiedContainer = container;
  assert(copiedContainer.size() == 2);
  assert(copiedContainer[0] == 6);

  // Test range-based for loop
  int sum = 0;
  for (const auto& item : container) {
      sum += item;
  }
  assert(sum == 13);
  }

  {
  xl::list<int> l;

  // Add elements to the list
  l.push_back(1);
  l.push_back(2);
  l.push_back(3);

  // Test the size of the list
  assert(l.size() == 3);

  // Test if the list is empty
  assert(!l.empty());

  // Test accessing elements in the list
  assert(l.front() == 1);
  assert(l.back() == 3);

  // Test inserting elements at specific positions
  auto it = l.begin();
  ++it; // Move to the second element
  it = l.insert(it, 4);
  assert(l.size() == 4);
  assert(*it == 4);

  // Test erasing elements
  it = l.begin();
  ++it; // Move to the second element
  it = l.erase(it);
  assert(l.size() == 3);
  assert(*it == 2);

  // Test clearing the list
  l.clear();
  assert(l.size() == 0);
  assert(l.empty());
  }

  {
  xl::list<int> l;

  // Test that the list is empty after default construction.
  assert(l.empty());

  // Add some elements to the list.
  l.push_back(1);
  l.push_front(2);
  l.insert(++l.begin(), 3);  // Insert 3 at the second position.

  // Test that the list has the expected size.
  assert(l.size() == 3);

  // Test that the list is not empty.
  assert(!l.empty());

  // Test that the list has the correct elements in the correct order.
  auto it = l.begin();
  assert(*it == 2);
  ++it;
  assert(*it == 3);
  ++it;
  assert(*it == 1);

  // Test removal of elements.
  l.pop_back();
  assert(l.size() == 2);
  l.pop_front();
  assert(l.size() == 1);
  l.clear();
  assert(l.empty());
  }

  {
  xl::list<int> l;

  // Assert that the linked list is empty.
  assert(l.empty());

  // Add some values to the linked list.
  l.push_back(1, 2, 3);

  // Assert that the linked list is not empty.
  assert(!l.empty());

  // Assert that the linked list has the correct size.
  assert(l.size() == 3);

  // Assert that the first value in the linked list is 1.
  assert(l.front() == 1);

  // Assert that the last value in the linked list is 3.
  assert(l.back() == 3);

  // Remove the last value from the linked list.
  l.pop_back();

  // Assert that the last value in the linked list is now 2.
  assert(l.back() == 2);

  // Clear the linked list.
  l.clear();

  // Assert that the linked list is now empty.
  assert(l.empty());
  }

  {
  xl::list<int> numbers = {1, 2, 3, 4, 5};

  // Accessing elements using iterators
  auto it = numbers.begin();
  assert(*it == 1);

  // Modifying elements
  *it = 10;
  assert(*it == 10);

  // Inserting elements
  it = numbers.insert(std::next(it), 20);
  assert(*it == 20);

  // Erasing elements
  it = numbers.erase(it);
  assert(*it == 2);

  // Iterating over the list
  int sum = 0;
  for (const auto& num : numbers) {
      sum += num;
  }
  assert(sum == 24);
  }

  {
  xl::list<int> l{xl::multi_t{}, 1, 2, 3, 4, 5};

  // Check the size of the list
  assert(l.size() == 5);

  // Insert a new value at the beginning of the list
  l.push_front(0);
  assert(l.size() == 6);
  assert(l.front() == 0);

  // Insert a new value at the end of the list
  l.push_back(6);
  assert(l.size() == 7);
  assert(l.back() == 6);

  // Remove the first element from the list
  l.pop_front();
  assert(l.size() == 6);
  assert(l.front() == 1);

  // Remove the last element from the list
  l.pop_back();
  assert(l.size() == 5);
  assert(l.back() == 5);

  // Clear the list
  l.clear();
  assert(l.empty());

  // Test the empty list
  assert(l.empty());

  // Test the push_front and pop_front methods
  l.push_front(1);
  assert(l.front() == 1);
  l.pop_front();
  assert(l.empty());

  // Test the push_back and pop_back methods
  l.push_back(6);
  assert(l.back() == 6);
  l.pop_back();

  // Test the insert method
  l.insert(l.begin(), 2);
  assert(*l.begin() == 2);

  // Test the erase method
  l.erase(l.begin());
  assert(l.size() == 0);

  // Test the clear method
  l.clear();
  assert(l.empty());
  }

  {
  xl::list<int> l;

  // Test push_back
  for(int i = 0; i < 10; ++i) {
      l.push_back(i);
  }
  assert(l.back() == 9);

  // Test push_front
  l.push_front(100);
  assert(l.front() == 100);

  // Test pop_back
  l.pop_back();
  assert(l.back() == 8);

  // Test pop_front
  l.pop_front();
  assert(l.front() == 0);

  // Test iterating over the list
  int expected_value = 0;
  for(auto val : l) {
      assert(val == expected_value++);
  }
  }

  {
  xl::list<int> l(5, 10);

  // Check that the list has the correct size
  assert(l.size() == 5);

  // Check that all elements in the list are 10
  for (const auto& e: l) assert(e == 10);
  }

  {
  xl::list palindrome(xl::from_range, std::string_view("racecar"));

  while(palindrome.size() > 1) {
    assert(palindrome.front() == palindrome.back());
    palindrome.pop_front();
    palindrome.pop_back();
  }

  assert(1 == palindrome.size());
  palindrome.sort();
  }

  {
  xl::list l(xl::from_range, std::views::iota(0, 100));
  assert(100 == l.size());
  l = std::views::iota(0, 10);
  assert(10 == l.size());
  }

  {
  xl::list<int> myList = {1, 2, 3, 4, 5};
  myList.insert(myList.begin(), 0);
  assert(myList.front() == 0);

  //Test that insert() inserts an element at the end of the list:
  myList = {1, 2, 3, 4, 5};
  myList.insert(myList.end(), 6);
  assert(myList.back() == 6);

  //Test that insert() inserts an element at a specific position in the list:
  myList = {1, 2, 3, 4, 5};
  myList.insert(std::next(myList.begin(), 2), 10);
  assert(myList[2] == 10);

  //Test that insert() inserts multiple elements at the beginning of the list:
  myList = {1, 2, 3, 4, 5};
  myList.insert(myList.begin(), {0, 0, 0});
  assert(myList.front() == 0);
  assert(myList.size() == 8);

  //Test that insert() inserts multiple elements at the end of the list:
  myList = {1, 2, 3, 4, 5};
  myList.insert(myList.end(), {6, 6, 6});
  assert(myList.back() == 6);
  assert(myList.size() == 8);

  //Test that insert() inserts multiple elements at a specific position in the list:
  myList = {1, 2, 3, 4, 5};
  myList.insert(std::next(myList.begin(), 2), {10, 10, 10});
  assert(myList[2] == 10);
  assert(myList.size() == 8);
  }

  {
  // Test 1: Empty list
  xl::list<int> my_list;
  assert(my_list.unique() == 0);

  // Test 7: List with all duplicates
  my_list = {1, 1, 1};
  assert(my_list.unique() == 2);

  // Test 8: List with all non-duplicates
  my_list = {1, 2, 3};
  assert(!my_list.unique());

  // Test 9: List with mixed duplicates and non-duplicates
  my_list = {1, 1, 2, 2, 3};
  assert(my_list.unique() == 2);

  // Test 10: List with all duplicates and non-duplicates
  my_list = {1, 1, 2, 2, 3, 4};
  assert(my_list.unique() == 2);

  my_list = {1, 2, 1, 1, 3, 3, 3, 4, 5, 4};
  assert(my_list.unique() == 3);
  }

  {
  xl::list<int> mylist = {1, 2, 3, 4, 5};

  // Test case 1: Resize to a smaller size
  mylist.resize(3);
  assert(mylist.size() == 3);

  // Test case 2: Resize to the same size
  mylist.resize(3);
  assert(mylist.size() == 3);

  // Test case 3: Resize to a larger size with default values (0)
  mylist.resize(5);
  assert(mylist.size() == 5);

  // Test case 4: Resize to a larger size with a specific value (10)
  mylist.resize(8, 10);
  assert(mylist.size() == 8);
  }

  {
  xl::list<int> list1 = {1, 2, 3, 4, 5};
  xl::list<int> list2 = {6, 7, 8, 9, 10};

  // Test merge()
  list1.merge(std::move(list2));
  assert(list1.size() == 10);
  assert(list1.front() == 1);
  assert(list1.back() == 10);

  // Test resize()
  list1.resize(5);
  assert(list1.size() == 5);
  assert(list1.front() == 1);
  assert(list1.back() == 5);
  }

  {
  // Test 1: Merge two empty lists
  xl::list<int> list1;
  xl::list<int> list2;
  list1.merge(std::move(list2));
  assert(list1.empty());

  // Test 2: Merge two non-empty lists
  list1.push_back(1);
  list1.push_back(2);
  list2.push_back(3);
  list2.push_back(4);
  list1.merge(std::move(list2));
  assert(list1.size() == 4);
  assert(list1.front() == 1);
  assert(list1.back() == 4);

  // Test 3: Merge two lists with different sizes
  list1.clear();
  list2.clear();
  list1.push_back(1);
  list1.push_back(2);
  list2.push_back(3);
  list2.push_back(4);
  list2.push_back(5);
  list1.merge(std::move(list2));
  assert(list1.size() == 5);
  assert(list1.front() == 1);
  assert(list1.back() == 5);

  // Test 4: Merge two lists with the same size
  list1.clear();
  list2.clear();
  list1.push_back(1);
  list1.push_back(2);
  list2.push_back(3);
  list2.push_back(4);
  list1.merge(std::move(list2));
  assert(list1.size() == 4);
  assert(list1.front() == 1);
  assert(list1.back() == 4);

  // Test 5: Merge two lists with different elements
  list1.clear();
  list2.clear();
  list1.push_back(1, 2, 3, 4, 5);
  list1.merge(list2);
  assert(list1.front() == 1);
  assert(list1.back() == 5);
  assert(list1.size() == 5);

  // Test 6: Merge two lists with the same elements
  list1.clear();
  list2.clear();
  list1.push_back(1, 2);
  list2.push_back(1, 2);
  list1.merge(list2);
  assert(list1.front() == 1);
  assert(list1.back() == 2);
  assert(list1.size() == 4);

  // Test 7: Merge two lists with different sizes and elements
  list1.clear();
  list2.clear();
  list1.push_back(1, 2, 3, 4, 5);
  list1.merge(list2);
  assert(list1.front() == 1);
  assert(list1.back() == 5);
  assert(list1.size() == 5);

  // Test 8: Merge two lists with the same size and elements
  list1.clear();
  list2.clear();
  list1.push_back(1);
  list1.push_back(2);
  list2.push_back(1);
  list2.push_back(2);
  list1.merge(list2);
  assert(list1.front() == 1);
  assert(list1.back() == 2);
  assert(list1.size() == 4);

  // Test 9: Merge two lists with different sizes and elements, and check the order
  list1.clear();
  list2.clear();
  list1.push_back(1,2, 3, 4, 5);
  list1.merge(list2);
  assert(list1.front() == 1);
  assert(list1.back() == 5);
  assert(list1.at(1) == 2);
  assert(list1.at(2) == 3);
  assert(list1.at(3) == 4);
  assert(list1.size() == 5);

  // Test 10: Merge two lists with the same size and elements, and check the order
  list1.clear();
  list2.clear();
  list1.push_back(1);
  list1.push_back(2);
  list2.push_back(1);
  list2.push_back(2);
  list1.merge(std::move(list2));
  assert(list1.size() == 4);
  assert(list1.front() == 1);
  assert(list1.back() == 2);
  assert(list1.at(1) == 1);
  assert(list1.at(2) == 2);
  }

  {
  // Test 1: Resize an empty list
  xl::list<int> list1;
  list1.resize(0);
  assert(list1.empty());

  // Test 2: Resize a non-empty list
  xl::list<int> list2;
  list2.push_back(1);
  list2.push_back(2);
  list2.resize(2);
  assert(list2.size() == 2);
  assert(list2.front() == 1);
  assert(list2.back() == 2);

  // Test 3: Resize a list with a different size
  xl::list<int> list3;
  list3.push_back(1);
  list3.push_back(2);
  list3.resize(3);
  assert(list3.size() == 3);
  assert(list3.front() == 1);
  assert(list3.back() == 0);

  // Test 4: Resize a list with the same size
  xl::list<int> list4;
  list4.push_back(1);
  list4.push_back(2);
  list4.resize(2);
  assert(list4.size() == 2);
  assert(list4.front() == 1);
  assert(list4.back() == 2);

  // Test 5: Resize a list with a different size and elements
  xl::list<int> list5;
  list5.push_back(1);
  list5.push_back(2);
  list5.resize(3);
  assert(list5.size() == 3);
  assert(list5.front() == 1);
  assert(list5.back() == 0);

  // Test 6: Resize a list with the same size and elements
  xl::list<int> list6;
  list6.push_back(1);
  list6.push_back(2);
  list6.resize(2);
  assert(list6.size() == 2);
  assert(list6.front() == 1);
  assert(list6.back() == 2);

  // Test 7: Resize a list with a different size and elements, and check the order
  xl::list<int> list7;
  list7.push_back(1);
  list7.push_back(2);
  list7.resize(3);
  assert(list7.size() == 3);
  assert(list7.front() == 1);
  assert(list7.back() == 0);
  assert(list7.at(1) == 2);

  // Test 8: Resize a list with the same size and elements, and check the order
  xl::list<int> list8;
  list8.push_back(1);
  list8.push_back(2);
  list8.resize(2);
  assert(list8.size() == 2);
  assert(list8.front() == 1);
  assert(list8.back() == 2);
  assert(list8.at(1) == 2);
  }

  {
  // Empty list
  xl::list<int> emptyList;
  assert(emptyList.size() == 0);
  assert(emptyList.empty());
  // List with one element
  xl::list<int> list1 = {42};
  assert(list1.size() == 1);
  assert(list1.front() == 42);
  // List with multiple elements
  xl::list<int> list2 = {1, 2, 3, 4, 5};
  assert(list2.size() == 5);
  assert(list2.front() == 1);
  assert(list2.back() == 5);
  // Modifying the list
  list2.push_back(6);
  assert(list2.size() == 6);
  assert(list2.front() == 1);
  assert(list2.back() == 6);
  // Iterating over the list
  int sum = 0;
  for (const auto& element : list2) {
  sum += element;
  }
  assert(sum == 21);
  // Erasing an element
  list2.erase(list2.begin());
  assert(list2.size() == 5);
  assert(list2.front() == 2);
  // Checking container properties
  assert(list2.empty() == false);
  }

  {
  // Create two lists
  xl::list<int> list1 = {1, 2, 3, 4, 5};
  xl::list<int> list2 = {10, 20, 30};

  // Test case 1: splice at the beginning of the list
  auto it1 = list1.begin();
  auto it2 = list2.begin();
  list1.splice(it1, std::move(list2));
  assert((list1 == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}));
  assert(list2.empty());

  // Test case 2: splice in the middle of the list
  it1 = list1.begin();
  std::advance(it1, 3); // Move iterator to position 3
  it2 = list2.begin();
  list1.splice(it1, std::move(list2));
  assert((list1 == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}));
  assert(list2.empty());

  // Test case 3: splice at the end of the list
  it1 = list1.end();
  it2 = list2.begin();
  list1.splice(it1, std::move(list2));
  assert((list1 == xl::list<int>{10, 20, 30, 1, 2, 3, 4, 5}));
  assert(list2.empty());

  // Test case 4: splice a single element from one list to another
  xl::list<int> list3 = {100};
  it1 = list1.begin();
  it2 = list3.begin();
  list1.splice(it1, std::move(list3), it2);
  assert((list1 == xl::list<int>{100, 10, 20, 30, 1, 2, 3, 4, 5}));
  assert(list3.empty());

  it1 = list1.begin();
  auto it2_begin = list1.begin();
  std::advance(it2_begin, 3); // Move iterator to position 3
  auto it2_end = list1.begin();
  std::advance(it2_end, 6); // Move iterator to position 6
  list3 = {200, 300};
  list3.splice(list3.begin(), std::move(list1), it2_begin, it2_end);
  assert(list1 == xl::list<int>({100, 10, 20, 3, 4, 5}));
  assert(list3 == xl::list<int>({30, 1, 2, 200, 300})); // Ensure list3 contains spliced elements
  }

  {
  // Testing first overload
  {
  xl::list<int> list1{1, 2, 3, 4, 5};
  xl::list<int> list2{6, 7, 8, 9, 10};

  list1.splice(list1.begin(), list2);

  assert(list1.size() == 10);
  assert(list2.empty());
  }

  // Testing second overload
  {
  xl::list<int> list1{1, 2, 3, 4, 5};
  xl::list<int> list2{6, 7, 8, 9, 10};

  auto it = list2.begin();
  std::advance(it, 2); // pointing to 8

  list1.splice(list1.begin(), list2, it);

  assert(list1.size() == 6);
  assert(list2.size() == 4);
  assert(list1.front() == 8);
  }

  // Testing third overload
  {
  xl::list<int> list1{1, 2, 3, 4, 5};
  xl::list<int> list2{6, 7, 8, 9, 10};

  auto it1 = list2.begin();
  auto it2 = it1;
  std::advance(it2, 3); // range [6, 7, 8]

  list1.splice(list1.begin(), list2, it1, it2);

  assert(list1.size() == 8);
  assert(list2.size() == 2);
  assert(list1.front() == 6);
  }
  }

  {
  // Test case 1: Merging two empty lists
  xl::list<int> list1, list2;
  list1.merge(list2);
  assert(list1.empty() && list2.empty());

  // Test case 2: Merging an empty list into a non-empty list
  xl::list<int> list3 = {1, 2, 3};
  xl::list<int> list4;
  list3.merge(list4);
  assert(list3 == xl::list<int>({1, 2, 3}) && list4.empty());

  // Test case 3: Merging a non-empty list into an empty list
  xl::list<int> list5;
  xl::list<int> list6 = {4, 5, 6};
  list5.merge(list6);
  assert(list5 == xl::list<int>({4, 5, 6}) && list6.empty());

  // Test case 4: Merging two non-empty lists of equal size
  xl::list<int> list7 = {1, 3, 5};
  xl::list<int> list8 = {2, 4, 6};
  list7.merge(list8);
  assert(list7 == xl::list<int>({1, 2, 3, 4, 5, 6}) && list8.empty());

  // Test case 5: Merging two non-empty lists of different sizes
  xl::list<int> list9 = {1, 3, 5};
  xl::list<int> list10 = {2, 4};
  list9.merge(list10);
  assert(list9 == std::initializer_list<int>({1, 2, 3, 4, 5}) && list10.empty());

  // Test case 6: Merging two non-empty lists with duplicates
  xl::list<int> list11 = {1, 2, 3};
  xl::list<int> list12 = {2, 3, 4};
  list11.merge(list12);
  assert(list11 == xl::list<int>({1, 2, 2, 3, 3, 4}) && list12.empty());

  // Test case 7: Merging two non-empty lists with a custom comparator
  xl::list<int> list13 = {5, 3, 1};
  xl::list<int> list14 = {6, 4, 2};
  list13.merge(list14, [](int a, int b) { return a > b; });
  assert(list13 == xl::list<int>({6, 5, 4, 3, 2, 1}) && list14.empty());
  }

  {
  xl::list<int> lst{1, 2, 3};

  // Create a vector with elements 4, 5, 6
  int const vec[]{4, 5, 6};

  // Test append_range()
  lst.append_range(vec);
  assert((lst == xl::list<int>{1, 2, 3, 4, 5, 6}));

  // Test prepend_range()
  lst.prepend_range(vec);
  assert((lst == xl::list<int>{4, 5, 6, 1, 2, 3, 4, 5, 6}));

  // Test insert_range()
  auto it = lst.begin();
  std::advance(it, 3); // Move iterator to the 4th element
  lst.insert_range(it, vec);
  assert((lst == xl::list<int>{4, 5, 6, 4, 5, 6, 1, 2, 3, 4, 5, 6}));
  }

  {
  xl::list<int> list1 = {1, 2, 3, 4, 5};
  int const vector1[]{6, 7, 8, 9, 10};

  // Use assign_range() to replace elements in list1 with elements from vector1
  list1.assign_range(vector1);

  // Check the results using assert()
  assert(list1.size() == std::size(vector1));
  auto list_it = list1.begin();
  auto vector_it = std::begin(vector1);
  while (list_it) assert(*list_it++ == *vector_it++);

  list1 = vector1;
  assert(list1.size() == std::size(vector1));
  }

  {
  xl::list<int> myList = {1, 2, 3};
  xl::list<int> appendList = {4, 5, 6};
  xl::list<int> prependList = {7, 8, 9};
  xl::list<int> insertList = {10, 11, 12};
  xl::list<int> assignList = {13, 14, 15};

  // Testing xl::list::append_range
  myList.append_range(appendList);
  assert(myList == xl::list<int>({1, 2, 3, 4, 5, 6}));

  // Testing xl::list::prepend_range
  myList.prepend_range(prependList);
  assert(myList == xl::list<int>({7, 8, 9, 1, 2, 3, 4, 5, 6}));

  // Testing xl::list::insert_range
  auto insertPos = std::find(myList.begin(), myList.end(), 1);
  myList.insert_range(insertPos, insertList);
  assert(myList == xl::list<int>({7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6}));

  // Testing xl::list::assign_range
  myList.assign_range(assignList);
  assert(myList == xl::list<int>({13, 14, 15}));
  }

  {
  // Test 1: Create a list
  xl::list<int> list1;
  assert(list1.empty());
  list1.sort();

  // Test 2: Add elements to the list
  list1.push_back(10);
  list1.push_back(20);
  list1.push_back(30);
  assert(list1.size() == 3);

  // Test 3: Remove elements from the list
  list1.pop_front();
  assert(list1.size() == 2);

  // Test 4: Check the elements in the list
  auto it = list1.begin();
  assert(*it == 20);
  ++it;
  assert(*it == 30);

  // Test 5: Clear the list
  list1.clear();
  assert(list1.empty());

  // Test 6: Add elements to the list using push_front
  list1.push_front(10);
  list1.push_front(20);
  list1.push_front(30);
  assert(list1.size() == 3);

  // Test 7: Remove elements from the list using pop_back
  list1.pop_back();
  assert(list1.size() == 2);

  // Test 8: Check the elements in the list
  it = list1.begin();
  assert(*it == 30);
  ++it;
  assert(*it == 20);

  // Test 9: Create a list with initializer list
  xl::list<int> list2 = {10, 20, 30, 40, 50};
  assert(list2.size() == 5);

  // Test 10: Remove elements from the list using erase
  list2.erase(list2.begin());
  assert(list2.size() == 4);

  // Test 11: Check the elements in the list
  it = list2.begin();
  assert(*it == 20);

  // Test 12: Insert elements into the list using insert
  list2.insert(list2.begin(), 10);
  assert(list2.size() == 5);

  // Test 13: Check the elements in the list
  it = list2.begin();
  assert(*it == 10);

  // Test 14: Create a list with a custom allocator
  xl::list<int> list3;

  // Test 15: Add elements to the list using emplace
  list3.emplace_back(10);
  list3.emplace_back(20);
  list3.emplace_back(30);
  assert(list3.size() == 3);

  // Test 16: Check the elements in the list
  it = list3.begin();
  assert(*it == 10);
  ++it;
  assert(*it == 20);
  ++it;
  assert(*it == 30);

  // Test 17: Create a list with a custom allocator and initializer list
  xl::list<int> list4{10, 20, 30, 40, 50};
  assert(list4.size() == 5);

  // Test 18: Check the elements in the list
  it = list4.begin();
  assert(*it == 10);
  ++it;
  assert(*it == 20);
  ++it;
  assert(*it == 30);
  ++it;
  assert(*it == 40);
  ++it;
  assert(*it == 50);

  // Test 19: Create a list with a custom allocator and initializer list
  xl::list<int> list5(list4);
  assert(list5.size() == 5);

  // Test 20: Check the elements in the list
  it = list5.begin();
  assert(*it == 10);
  ++it;
  assert(*it == 20);
  ++it;
  assert(*it == 30);
  ++it;
  assert(*it == 40);
  ++it;
  assert(*it == 50);
  }

  {
  xl::list<int> lst;

  // Test push_back
  lst.push_back(10);
  lst.push_back(20);
  lst.push_back(30);
  assert(lst.size() == 3);
  assert(lst.back() == 30);

  // Test push_front
  lst.push_front(5);
  assert(lst.size() == 4);
  assert(lst.front() == 5);

  // Test pop_back
  lst.pop_back();
  assert(lst.size() == 3);
  assert(lst.back() == 20);

  // Test pop_front
  lst.pop_front();
  assert(lst.size() == 2);
  assert(lst.front() == 10);

  // Test insert
  auto it = lst.begin();
  lst.insert(it, 15);
  assert(lst.size() == 3);
  assert(lst.front() == 15);

  // Test erase
  it = lst.begin();
  lst.erase(it);
  assert(lst.size() == 2);
  assert(lst.front() == 10);

  // Test clear
  lst.clear();
  assert(lst.empty());

  // Test resize
  lst.resize(5, 100);
  assert(lst.size() == 5);
  assert(lst.back() == 100);

  // Test assign
  lst.assign(3, 200);
  assert(lst.size() == 3);
  assert(lst.front() == 200);

  // Test splice
  xl::list<int> lst2 = {1, 2, 3};
  it = lst.begin();
  lst.splice(it, lst2);
  assert(lst.size() == 6);
  assert(lst.front() == 1);
  assert(lst2.empty());

  // Test remove
  lst.remove(1);
  assert(lst.size() == 5);
  assert(lst.front() == 2);

  // Test remove_if
  lst.remove_if([](int i){ return i > 2; });
  assert(lst.size() == 1);
  assert(lst.back() == 2);

  // Test unique
  lst.push_back(2);
  lst.unique();
  assert(lst.size() == 1);
  assert(lst.back() == 2);

  // Test merge
  xl::list<int> lst3 = {1, 3};
  lst.merge(lst3);
  assert(lst.size() == 3);
  assert(lst.front() == 1);
  assert(lst3.empty());

  // Test sort
  lst.sort();
  assert(lst.front() == 1);
  assert(lst.back() == 3);

  // Test reverse
  lst.reverse();
  assert(lst.front() == 3);
  assert(lst.back() == 1);
  }

  {
  xl::list<int> lst = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  lst.erase(std::remove(lst.begin(), lst.end(), 2), lst.end());
  assert(std::find(lst.begin(), lst.end(), 2) == lst.end());
  assert(!xl::find(lst, 2));

  lst.erase(std::remove_if(lst.begin(), lst.end(), [](int i){ return i % 2 == 0; }), lst.end());
  assert(std::find_if(lst.begin(), lst.end(), [](int i){ return i % 2 == 0; }) == lst.end());
  assert(!xl::find_if(lst, [](int i){ return i % 2 == 0; }));
  }

  {
  xl::list<int> lst = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  xl::erase(lst, 2);
  assert(std::ranges::find(lst, 2) == lst.end());
  assert(!xl::find(lst, 2));

  xl::erase_if(lst, [](int i){ return i % 2 == 0; });
  assert(std::ranges::find_if(lst, [](int i){ return i % 2 == 0; }) == lst.end());
  assert(!xl::find_if(lst, [](int i){ return i % 2 == 0; }));
  }

  {
  xl::list<int> myList = {1, 2, 3, 4, 5, 3, 6, 3, 7};

  // Using std::erase() to remove a specific value (e.g., 3) from the list
  xl::erase(myList, 3);

  // Using std::erase_if() to remove elements that satisfy a specific condition (e.g., value greater than 4)
  xl::erase_if(myList, [](int value) { return value > 4; });

  // Asserting the expected results
  assert(std::find(myList.begin(), myList.end(), 3) == myList.end()); // 3 should be removed
  assert(!xl::find(myList, 3)); // 3 should be removed
  assert(std::none_of(myList.begin(), myList.end(), [](int value) { return value > 4; })); // No value greater than 4 should remain
  }

  {
  xl::list<int> list1;

  // Test 1: Check if list is empty.
  assert(list1.empty());

  // Test 2: Add elements to the list using push_back() and check the size.
  list1.push_back(1);
  list1.push_back(2);
  list1.push_back(3);
  assert(list1.size() == 3);

  // Test 3: Access elements using front() and back() and check the values.
  assert(list1.front() == 1);
  assert(list1.back() == 3);

  // Test 4: Add elements using insert() and check the size.
  list1.insert(list1.begin(), 0);
  list1.insert(list1.end(), 4);
  assert(list1.size() == 5);

  // Test 5: Remove elements using pop_back() and check the size.
  list1.pop_back();
  assert(list1.size() == 4);

  // Test 6: Remove elements using erase() and check the size.
  list1.erase(list1.begin());
  assert(list1.size() == 3);

  // Test 7: Check if elements are in the correct order using iterator.
  xl::list<int>::iterator it = list1.begin();
  assert(*it++ == 1);
  assert(*it++ == 2);
  assert(*it++ == 3);

  // Test 8: Check if elements are in the correct order using range-based for loop.
  for (int i : list1)
  {
    assert(i == 1 || i == 2 || i == 3);
  }

  // Test 9: Check if elements are in the correct order using reverse iterator.
  xl::list<int>::reverse_iterator rit = list1.rbegin();
  assert(*rit == 3);
  rit++;
  assert(*rit == 2);
  rit++;
  assert(*rit == 1);

  // Test 10: Clear the list and check if it is empty.
  list1.clear();
  assert(list1.empty());
  }

  {
  // Test default constructor
  xl::list<int> list1;
  assert(list1.empty());
  assert(list1.size() == 0);

  // Test constructor with initializers
  xl::list<int> list2{1, 2, 3, 4, 5};
  assert(!list2.empty());
  assert(list2.size() == 5);
  assert(list2.front() == 1);
  assert(list2.back() == 5);

  // Test copy constructor
  xl::list<int> list3(list2);
  assert(!list3.empty());
  assert(list3.size() == 5);
  assert(list3.front() == 1);
  assert(list3.back() == 5);

  // Test move constructor
  xl::list<int> list4(std::move(list2));
  assert(list2.empty());
  assert(list2.size() == 0);
  assert(!list4.empty());
  assert(list4.size() == 5);
  assert(list4.front() == 1);
  assert(list4.back() == 5);

  // Test assignment operator
  list2 = list3;
  assert(!list2.empty());
  assert(list2.size() == 5);
  assert(list2.front() == 1);
  assert(list2.back() == 5);

  // Test move assignment operator
  list2 = std::move(list3);
  assert(list3.empty());
  assert(list3.size() == 0);
  assert(!list2.empty());
  assert(list2.size() == 5);
  assert(list2.front() == 1);
  assert(list2.back() == 5);

  // Test push_front
  list2.push_front(0);
  assert(!list2.empty());
  assert(list2.size() == 6);
  assert(list2.front() == 0);
  assert(list2.back() == 5);

  // Test push_back
  list2.push_back(6);
  assert(!list2.empty());
  assert(list2.size() == 7);
  assert(list2.front() == 0);
  assert(list2.back() == 6);

  // Test pop_front
  list2.pop_front();
  assert(!list2.empty());
  assert(list2.size() == 6);
  assert(list2.front() == 1);
  assert(list2.back() == 6);

  // Test pop_back
  list2.pop_back();
  assert(!list2.empty());
  assert(list2.size() == 5);
  assert(list2.front() == 1);
  assert(list2.back() == 5);

  // Test insert
  list2.insert(list2.begin(), 0);
  assert(!list2.empty());
  assert(list2.size() == 6);
  assert(list2.front() == 0);
  assert(list2.back() == 5);

  // Test erase
  list2.erase(list2.begin());
  assert(!list2.empty());
  assert(list2.size() == 5);
  assert(list2.front() == 1);
  assert(list2.back() == 5);

  // Test clear
  list2.clear();
  assert(list2.empty());
  assert(list2.size() == 0);

  // Test swap
  xl::list<int> list5{1, 2, 3};
  list2.swap(list5);
  assert(!list2.empty());
  assert(list2.size() == 3);
  assert(list2.front() == 1);
  assert(list2.back() == 3);
  assert(list5.empty());
  assert(list5.size() == 0);

  // Test relational operators
  assert(list2 > list3);
  assert(list2 >= list3);
  assert(list3 < list2);
  assert(list3 <= list2);
  assert(list2 == list2);
  assert(list2 != list3);

  // Test reverse
  xl::list<int> list9 = {13, 14, 15, 16, 17};
  list9.reverse();
  assert(!list9.empty());
  assert(list9.size() == 5);
  assert(list9.front() == 17);
  assert(list9.back() == 13);

  // Test sort
  xl::list<int> list10 = {18, 15, 20, 12, 19};
  list10.sort();
  assert(!list10.empty());
  assert(list10.size() == 5);
  assert(list10.front() == 12);
  assert(list10.back() == 20);

  // Test merge
  xl::list<int> list11 = {21, 23, 25};
  xl::list<int> list12 = {22, 24, 26};
  list11.merge(list12);
  assert(!list11.empty());
  assert(list11.size() == 6);
  assert(list11.front() == 21);
  assert(list11.back() == 26);

  // Test splice
  xl::list<int> list13 = {27, 28, 29};
  xl::list<int> list14 = {30, 31, 32};
  list14.splice(list14.begin(), list13);
  assert(!list14.empty());
  assert(list14.size() == 6);
  assert(list14.front() == 27);
  assert(list14.back() == 32);

  // Test unique
  xl::list<int> list15 = {33, 34, 34, 35, 35, 36};
  list15.unique();
  assert(!list15.empty());
  assert(list15.size() == 4);
  assert(list15.front() == 33);
  assert(list15.back() == 36);

  // Test reverse iterator
  xl::list<int> list16 = {37, 38, 39, 40, 41};
  for (auto it = list16.rbegin(); it != list16.rend(); ++it)
    assert(*it > 0);
  }

  {
  // Test operator[]
  xl::list l1{1, 2, 3};
  assert(l1[0] == 1);
  assert(l1[1] == 2);
  assert(l1[2] == 3);
  }

  {
  // Test erase with multiple elements
  xl::list<int> l2{1, 2, 3, 4, 5};
  auto it = l2.begin();
  l2.erase(std::next(it, 2));
  l2.erase(it);
  assert(l2.size() == 3);
  assert(l2.front() == 2);
  assert(l2.back() == 5);
  }

  {
  // Test clear with a non-empty list
  xl::list<int> l3{1, 2, 3, 4, 5};
  l3.clear();
  assert(l3.empty());
  assert(l3.size() == 0);
  }

  {
  // Test insert with a non-existing iterator
  xl::list<int> l4{1, 2, 3};
  auto it = l4.begin();
  ++it; // Move to the second element
  l4.insert(l4.end(), 4); // Insert 4 at the end of the list
  assert(l4.size() == 4);
  assert(*it == 2);
  }

  {
  // Test sort with a non-empty list
  xl::list<int> l5{18, 15, 20, 12, 19};
  l5.sort();
  assert(!l5.empty());
  assert(l5.size() == 5);
  assert(l5.front() == 12);
  assert(l5.back() == 20);
  }

  {
  // Test merge with two non-empty lists
  xl::list<int> l6{21, 23, 25};
  xl::list<int> l7{22, 24, 26};
  l6.merge(l7);
  assert(!l6.empty());
  assert(l6.size() == 6);
  assert(l6.front() == 21);
  assert(l6.back() == 26);
  }

  {
   // Test splice with two non-empty lists
   xl::list<int> l8{27, 28, 29};
   xl::list<int> l9{30, 31, 32};
   l9.splice(l9.begin(), l8);
   assert(!l9.empty());
   assert(l9.size() == 6);
   assert(l9.front() == 27);
   assert(l9.back() == 32);
  }

  {
  // Test unique with a non-empty list
  xl::list<int> l10 = {33, 34, 34, 35, 35, 36};
  l10.unique();
  assert(!l10.empty());
  assert(l10.size() == 4);
  }

  {
  xl::list<int> lst1 = {1, 2, 3};
  xl::list<int> lst2;
  lst1.splice(lst1.begin(), lst2);
  assert(lst1.size() == 3);
  assert(lst2.empty());
  }

  {
  xl::list<int> lst1 = {1, 2, 2, 3};
  xl::list<int> lst2 = {2, 4, 5};
  lst1.merge(lst2);
  assert(lst1.size() == 7);
  assert(lst1.front() == 1);
  assert(lst1.back() == 5);
  }

  {
  xl::list<int> lst;
  lst.unique();
  assert(lst.empty());
  }

  {
  xl::list<int> lst = {5};
  lst.sort();
  assert(lst.size() == 1);
  assert(lst.front() == 5);
  }

  {
  xl::list<int> lst;
  lst.reverse();
  assert(lst.empty());
  }

  {
  xl::list<int> lst = {1, 2, 3};
  auto it = lst.end();
  lst.insert(it, 4);
  assert(lst.size() == 4);
  assert(lst.back() == 4);
  }

  {
  xl::list<int> lst;
  lst.assign(5, 100);
  assert(lst.size() == 5);
  assert(lst.front() == 100);
  }

  {
  // Test case 8: Merging an empty list into a non-empty list with duplicates
  xl::list<int> list11 = {1, 2, 3};
  xl::list<int> list12 = {2, 3, 4};
  list11.merge(list12);
  assert(list11 == xl::list<int>({1, 2, 2, 3, 3, 4}) && list12.empty());
  }

  {
  // Test case 9: Merging an empty list into a non-empty list with a custom comparator
  xl::list<int> list13 = {5, 3, 1};
  xl::list<int> list14;
  list13.merge(list14, [](int a, int b) { return a > b; });
  assert(list13 == xl::list<int>({5, 3, 1}) && list14.empty());
  }

  {
  xl::list<int> list1{1, 2, 3, 4, 5};
  xl::list<int> list2{6, 7, 8, 9, 10};

  list1.merge(list2);

  assert(list1.size() == 10);
  assert(list2.empty());
  }

  {
  xl::list<int> list1{1, 2, 3, 4, 5};
  xl::list<int> list2{6, 7, 8, 9, 10};

  auto it1 = list2.begin();
  auto it2 = it1;
  std::advance(it2, 3); // range [6, 7, 8]

  list1.splice(list1.begin(), list2, it1, it2);

  assert(list1.size() == 8);
  assert(list2.size() == 2);
  assert(list1.front() == 6);
  }

  {
  xl::list<int> list1{1, 2, 3, 4, 5};

  list1.remove(3);

  assert(list1.size() == 4);
  assert(list1.front() == 1);
  assert(list1.back() == 5);
  }

  {
  xl::list<int> list1{1, 2, 3};

  list1.resize(5, 100);

  assert(list1.size() == 5);
  assert(list1.back() == 100);
  }

  {
  xl::list<int> list1;

  list1.assign(3, 200);

  assert(list1.size() == 3);
  assert(list1.front() == 200);
  }

  {
  xl::list lst{1, 2, 3, 4, 5};
  std::ranges::reverse(lst);

  assert((lst == xl::list{5, 4, 3, 2, 1}));
  }
}

void test2()
{
  {
  // Default constructor
  xl::list<int> l1;
  assert(l1.empty());
  }

  {
  // Fill constructor
  xl::list<int> l2(5, 100);
  assert(l2.size() == 5);
  assert(l2.front() == 100);
  assert(l2.back() == 100);
  }

  {
  // Range constructor
  int arr[] = {1, 2, 3, 4, 5};
  xl::list<int> l3(std::begin(arr), std::end(arr));
  assert(l3.size() == 5);
  assert(l3.front() == 1);
  assert(l3.back() == 5);
  }

  {
  // Range constructor
  int arr[] = {1, 2, 3, 4, 5};
  xl::list l3(xl::from_range, arr);
  assert(l3.size() == 5);
  assert(l3.front() == 1);
  assert(l3.back() == 5);
  }

  {
  // Copy constructor
  xl::list<int> l4{1, 2, 3};
  xl::list<int> l5(l4);
  assert(l5.size() == l4.size());
  assert(l5.front() == l4.front());
  assert(l5.back() == l4.back());
  }

  {
  // Move constructor
  xl::list<int> l6{1, 2, 3};
  xl::list<int> l7(std::move(l6));
  assert(l6.empty());
  assert(l7.size() == 3);
  }

  {
  // Copy assignment
  xl::list<int> l8{1, 2, 3};
  xl::list<int> l9;
  l9 = l8;
  assert(l9.size() == l8.size());
  assert(l9.front() == l8.front());
  assert(l9.back() == l8.back());
  }

  {
  // Move assignment
  xl::list<int> l10{1, 2, 3};
  xl::list<int> l11;
  l11 = std::move(l10);
  assert(l10.empty());
  assert(l11.size() == 3);
  }

  {
  // empty()
  xl::list<int> l12;
  assert(l12.empty());
  l12.push_back(1);
  assert(!l12.empty());
  }

  {
  // size()
  xl::list<int> l13;
  assert(l13.size() == 0);
  l13.push_back(1);
  assert(l13.size() == 1);
  }

  {
  // max_size()
  xl::list<int> l14;
  assert(l14.max_size() > 0);
  }

  {
  // front()
  xl::list<int> l15{1, 2, 3};
  assert(l15.front() == 1);
  }

  {
  // back()
  xl::list<int> l16{1, 2, 3};
  assert(l16.back() == 3);
  }

  {
  // push_front()
  xl::list<int> l17;
  l17.push_front(1);
  assert(l17.front() == 1);
  assert(l17.size() == 1);
  }

  {
  // pop_front()
  xl::list<int> l18{1, 2, 3};
  l18.pop_front();
  assert(l18.front() == 2);
  assert(l18.size() == 2);
  }

  {
  // push_back()
  xl::list<int> l19;
  l19.push_back(1);
  assert(l19.back() == 1);
  assert(l19.size() == 1);
  }

  {
  // pop_back()
  xl::list<int> l20{1, 2, 3};
  l20.pop_back();
  assert(l20.back() == 2);
  assert(l20.size() == 2);
  }

  {
  // insert() single element
  xl::list<int> l21{1, 2, 3};
  auto it = l21.insert(l21.begin(), 100);
  assert(*it == 100);
  assert(l21.front() == 100);
  assert(l21.size() == 4);
  }

  {
  // insert() fill
  xl::list<int> l22{1, 2, 3};
  l22.insert(l22.begin(), 2, 100);
  assert(l22.front() == 100);
  assert(l22.size() == 5);
  }

  {
  // insert() range
  xl::list<int> l23{1, 2, 3};
  int arr[] = {100, 200};
  l23.insert(l23.begin(), std::begin(arr), std::end(arr));
  assert(l23.front() == 100);
  assert(l23.size() == 5);
  }

  {
  // erase() single element
  xl::list<int> l24{1, 2, 3};
  auto it = l24.erase(l24.begin());
  assert(*it == 2);
  assert(l24.front() == 2);
  assert(l24.size() == 2);
  }

  {
  // erase() range
  xl::list<int> l25{1, 2, 3, 4, 5};
  auto it = l25.erase(l25.begin(), std::next(l25.begin(), 3));
  assert(*it == 4);
  assert(l25.size() == 2);
  }

  {
  // swap()
  xl::list<int> l26{1, 2, 3};
  xl::list<int> l27{4, 5, 6};
  l26.swap(l27);
  assert(l26.front() == 4);
  assert(l27.front() == 1);
  }

  {
  // clear()
  xl::list<int> l28{1, 2, 3};
  l28.clear();
  assert(l28.empty());
  }

  {
  // splice() entire list
  xl::list<int> l29{1, 2, 3};
  xl::list<int> l30{4, 5, 6};
  l29.splice(l29.begin(), std::move(l30));
  assert(l29.size() == 6);
  assert(l30.empty());
  }

  {
  // splice() single element
  xl::list<int> l31{1, 2, 3};
  xl::list<int> l32{4, 5, 6};
  l31.splice(l31.begin(), std::move(l32), l32.begin());
  assert(l31.front() == 4);
  assert(l31.size() == 4);
  assert(l32.size() == 2);
  }

  {
  // splice() element range
  xl::list<int> l33{1, 2, 3};
  xl::list<int> l34{4, 5, 6};
  l33.splice(l33.begin(), std::move(l34), std::next(l34.begin()), l34.end());
  assert(l33.size() == 5);
  assert(l34.size() == 1);
  }

  {
  // remove()
  xl::list<int> l35{1, 2, 1, 3, 1, 4};
  l35.remove(1);
  assert(l35.size() == 3);
  assert(l35.front() == 2);
  }

  {
  // remove_if()
  xl::list<int> l36{1, 2, 3, 4, 5};
  l36.remove_if([](int x) { return x % 2 == 0; });
  assert(l36.size() == 3);
  assert(l36.front() == 1);
  }

  {
  // unique()
  xl::list<int> l37{1, 2, 2, 3, 3, 3};
  l37.unique();
  assert(l37.size() == 3);
  }

  {
  // merge()
  xl::list<int> l38{1, 3, 5};
  xl::list<int> l39{2, 4, 6};
  l38.merge(std::move(l39));
  assert(l38.size() == 6);
  assert(l38.front() == 1);
  assert(l38.back() == 6);
  }

  {
  // reverse()
  xl::list<int> l40{1, 2, 3};
  l40.reverse();
  assert(l40.front() == 3);
  assert(l40.back() == 1);
  }

  {
  // sort()
  xl::list<int> l41{3, 1, 2};
  l41.sort();
  assert(l41.front() == 1);
  assert(l41.back() == 3);
  }

  {
  // find(), erase()
  xl::list<int> l42{3, 1, 2};
  *xl::find(l42, 3) = 1;
  assert(l42.front() == 1);
  xl::erase(l42, 3, 2, 1);
  assert(l42.empty());
  }
}

void test3()
{
  // Test emplace and emplace_back
  {
    xl::list<std::pair<int, int>> lst;
    lst.emplace_back(1, 2);
    lst.emplace_front(3, 4);
    auto it = lst.begin();
    ++it;
    it = lst.emplace(it, 5, 6);

    assert(lst.size() == 3);
    assert(lst.front() == std::make_pair(3, 4));
    assert(lst.back() == std::make_pair(1, 2));
    assert(*it == std::make_pair(5, 6));
  }

  // Test assign with initializer list
  {
    xl::list<int> lst = {1, 2, 3};
    lst.assign({4, 5, 6, 7});
    assert(lst.size() == 4);
    assert(lst.front() == 4);
    assert(lst.back() == 7);
  }

  // Test swap between empty and non-empty lists
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2;
    lst1.swap(lst2);
    assert(lst1.empty());
    assert(lst2.size() == 3);
  }

  // Test custom sort comparator
  {
    xl::list<int> lst = {3, 1, 4, 2, 5};
    lst.sort(std::greater<int>());
    assert((lst == xl::list<int>{5, 4, 3, 2, 1}));
  }

  // Test double reverse
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    lst.reverse();
    lst.reverse();
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
  }

  // Test remove non-existing element
  {
    xl::list<int> lst = {1, 2, 3};
    lst.remove(42);
    assert(lst.size() == 3);
  }

  // Test self-splice
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it = lst.begin();
    std::advance(it, 2);
    lst.splice(lst.begin(), lst, it, lst.end());
    assert((lst == xl::list<int>{3, 4, 5, 1, 2}));
  }

  // Test self-splice
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    lst.splice(lst.begin(), lst, lst.begin());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
    lst.splice(std::next(lst.begin()), lst, lst.begin());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
    lst.splice(lst.end(), lst, lst.begin(), lst.end());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
  }

  // Test unique with non-consecutive duplicates
  {
    xl::list<int> lst = {1, 2, 1, 3, 3, 2, 4};
    lst.sort();
    lst.unique();
    assert((lst == xl::list<int>{1, 2, 3, 4}));
  }

  // Test merge with partially overlapping ranges
  {
    xl::list<int> lst1 = {1, 3, 5};
    xl::list<int> lst2 = {2, 3, 4, 6};
    lst1.merge(lst2);
    assert((lst1 == xl::list<int>{1, 2, 3, 3, 4, 5, 6}));
    assert(lst2.empty());
  }

  // Test exception safety in push_back
  {
    struct TestException : std::exception {};
    struct Thrower {
      Thrower() = default;
      Thrower(int) { throw TestException(); }
    };

    xl::list<Thrower> lst;
    lst.push_back(Thrower{});  // Should succeed

    try {
      lst.push_back(42);  // Should throw
      assert(false && "Exception not thrown");
    } catch (const TestException&) {
      assert(lst.size() == 1);
    }
  }

  // Test const iterators
  {
    const xl::list<int> lst = {1, 2, 3};
    int sum = 0;
    for (auto it = lst.cbegin(); it != lst.cend(); ++it) {
      sum += *it;
    }
    assert(sum == 6);
  }

  // Test very large lists
  {
    xl::list<size_t> large_list;
    const size_t count = 100000;
    for (size_t i = 0; i < count; ++i) {
      large_list.push_back(i);
    }
    assert(large_list.size() == count);
    assert(large_list.front() == 0);
    assert(large_list.back() == count - 1);

    // Test iterator traversal
    size_t sum = 0;
    for (auto it = large_list.begin(); it != large_list.end(); ++it) {
      sum += *it;
    }
    assert(sum == (count - 1) * count / 2);
  }

  // Test move semantics with large data
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
    assert(dest.back().buffer[0] == 2);
  }

  // Test exception safety in complex operations
  {
    struct ThrowOnCopy {
      int value;
      ThrowOnCopy(int v) : value(v) {}
      ThrowOnCopy(const ThrowOnCopy& other) : value(other.value) {
        if (value == 42) throw std::runtime_error("Copy failed");
      }
    };

    xl::list<ThrowOnCopy> lst;
    lst.emplace_back(1);
    lst.emplace_back(2);

    try {
      // This insert should throw
      lst.insert(lst.begin(), ThrowOnCopy(42));
      assert(false && "Exception not thrown");
    } catch (const std::runtime_error&) {
      assert(lst.size() == 2);
      assert(lst.front().value == 1);
    }
  }

  // Test stateful comparators
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

    // Should be grouped: values <=5 first, then >5
    auto it = lst.begin();
    while (*it <= 5) ++it;
    while (it != lst.end()) {
      assert(*it > 5);
      ++it;
    }
  }

  // Test splice with same list
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it = lst.begin();
    std::advance(it, 2);  // Pointing to 3
    lst.splice(lst.end(), lst, it);
    assert((lst == xl::list<int>{1, 2, 4, 5, 3}));

    lst.splice(lst.begin(), lst, std::prev(lst.end()));
    assert((lst == xl::list<int>{3, 1, 2, 4, 5}));
  }

  // Test merge with unstable elements
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

  // Test performance of sort
  {
    xl::list<int> perf_list;
    const int count = 10000;
    for (int i = count; i > 0; --i) {
      perf_list.push_back(i);
    }

    perf_list.sort();
    assert(perf_list.front() == 1);
    assert(perf_list.back() == count);

    // Verify sorted order
    int expected = 1;
    for (const auto& val : perf_list) {
      assert(val == expected++);
    }
  }

  // Test heterogeneous comparisons
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<long> lst2 = {1, 2, 3};
    xl::list<int> lst3 = {1, 2, 4};

    assert(lst1 == lst2);
    assert(lst1 != lst3);
    assert(lst1 < lst3);
    assert(lst3 > lst1);
  }

  // Test emplace at different positions
  {
    xl::list<std::string> lst = {"world"};
    lst.emplace(lst.begin(), "hello");
    lst.emplace(lst.end(), "!");

    assert(lst.size() == 3);
    assert(lst.front() == "hello");
    assert(lst.back() == "!");

    auto it = lst.begin();
    ++it;
    it = lst.emplace(it, "there");
    assert(*it == "there");
    assert((lst == xl::list<std::string>{"hello", "there", "world", "!"}));
  }

  // Test initializer list assignment
  {
    xl::list<int> lst;
    lst = {1, 2, 3, 4, 5};
    assert(lst.size() == 5);
    assert(lst.back() == 5);

    lst.assign({6, 7, 8});
    assert(lst.size() == 3);
    assert(lst.front() == 6);
  }

  // Test reverse iterators
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    int sum = 0;
    for (auto rit = lst.rbegin(); rit != lst.rend(); ++rit) {
      sum = sum * 10 + *rit;
    }
    assert(sum == 54321);
  }

  // Test swap performance
  {
    xl::list<int> listA;
    xl::list<int> listB;
    const int count = 100000;

    for (int i = 0; i < count; ++i) {
      listA.push_back(i);
      listB.push_back(i * 2);
    }

    listA.swap(listB);

    assert(listA.front() == 0);
    assert(listB.front() == 0);
  }
{
    xl::list<std::string> lst;
    lst.push_back("hello");
    lst.push_back("world");
    assert(lst.size() == 2);
    assert(lst.front() == "hello");
    assert(lst.back() == "world");

    // Test move construction
    xl::list<std::string> lst2 = std::move(lst);
    assert(lst.empty());
    assert(lst2.size() == 2);
    assert(lst2.front() == "hello");
    assert(lst2.back() == "world");

    // Test move assignment
    lst = std::move(lst2);
    assert(lst2.empty());
    assert(lst.size() == 2);
    assert(lst.front() == "hello");
    assert(lst.back() == "world");

    // Test copy construction
    xl::list<std::string> lst3 = lst;
    assert(lst3.size() == 2);
    assert(lst3.front() == "hello");
    assert(lst.size() == 2); // Original list should remain unchanged

    // Test copy assignment
    xl::list<std::string> lst4;
    lst4 = lst;
    assert(lst4.size() == 2);
    assert(lst4.front() == "hello");

    // Test swap
    lst3.swap(lst4);
    assert(lst3.size() == 2);
    assert(lst4.size() == 2);
  }

  // Test iterator invalidation rules
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it = lst.begin();
    ++it; // Points to 2
    auto it2 = std::next(it); // Points to 3

    // Erase element at 'it' (value 2)
    lst.erase(it);
    // it2 should still be valid (pointing to 3)
    assert(*it2 == 3);

    // Insert should not invalidate iterators
    auto it3 = lst.begin(); // Points to 1
    lst.insert(std::next(it3), 10);
    assert(*it3 == 1); // it3 still valid
    assert(*it2 == 3); // it2 still valid
  }

  // Test resource management with custom destructor
  {
    static int counter = 0;
    struct Counted {
      Counted() { ++counter; }
      Counted(const Counted&) { ++counter; }
      ~Counted() { --counter; }
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

  // Test emplace with multiple arguments
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
    assert(lst.back().name == "Alice");
    assert(it->name == "Charlie");
    assert(it->age == 40);
  }

  // Test reverse iterators
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    int sum = 0;
    for (auto rit = lst.rbegin(); rit != lst.rend(); ++rit) {
      sum = sum * 10 + *rit;
    }
    assert(sum == 54321);
  }

  // Test edge cases for single-element list
  {
    xl::list<int> lst;
    lst.push_back(42);
    
    // Test operations on single-element list
    assert(!lst.empty());
    assert(lst.size() == 1);
    assert(lst.front() == 42);
    assert(lst.back() == 42);
    
    lst.pop_front();
    assert(lst.empty());
    
    lst.push_front(100);
    lst.pop_back();
    assert(lst.empty());
    
    lst.push_back(200);
    lst.erase(lst.begin());
    assert(lst.empty());
  }

  // Test self-assignment
  {
    xl::list<int> lst = {1, 2, 3};
    lst = lst; // Should handle self-assignment gracefully
    assert(lst.size() == 3);
    assert(lst.front() == 1);
    assert(lst.back() == 3);
  }

  // Test comparison operators
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2 = {1, 2, 3};
    xl::list<int> lst3 = {4, 5, 6};
    xl::list<int> lst4 = {1, 2};

    assert(lst1 == lst2);
    assert(lst1 != lst3);
    assert(lst1 != lst4);
    assert(lst4 < lst1);
    assert(lst3 > lst1);
    assert(lst1 <= lst2);
    assert(lst1 >= lst2);
  }

  // Test initializer list assignment
  {
    xl::list<int> lst;
    lst = {1, 2, 3, 4, 5};
    assert(lst.size() == 5);
    assert(lst.back() == 5);

    lst.assign({6, 7, 8});
    assert(lst.size() == 3);
    assert(lst.front() == 6);
  }

  // test iterator validity
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it1 = lst.begin();
    auto it2 = std::next(it1);
    auto it3 = std::next(it2);

    // Test iterator validity after erase
    it3 = lst.erase(it2);
    assert(*it1 == 1);
    assert(*it3 == 3);  // Valid, returned by erase()

    // Test iterator validity after insert
    auto it4 = lst.insert(it3, 10);
    assert(*it3 == 3);  // Can be dereferenced, but is invalid
    assert(*it4 == 10);

    // Test iterator validity after splice
    xl::list<int> lst2 = {20, 30};
    lst.splice(it3 = std::next(it4), lst2);
    assert(*it3 == 3);  // Should remain valid
    assert(lst.size() == 7);
  }

  {
    // Case-insensitive string comparison
    auto case_insensitive = [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(
            a.begin(), a.end(), b.begin(), b.end(),
            [](char c1, char c2) {
                return std::tolower(c1) < std::tolower(c2);
            }
        );
    };

    xl::list<std::string> words = {"Banana", "apple", "Carrot", "date"};
    words.sort(case_insensitive);

    assert((words == xl::list<std::string>{"apple", "Banana", "Carrot", "date"}));
  }

  {
    xl::list<int> a = {1, 2, 3};
    xl::list<int> b = {4, 5, 6};
    auto it = a.begin();
    std::advance(it, 2);  // Points to 3

    // Test splicing between different lists
    b.splice(b.begin(), a, a.begin(), it);
    assert(a.size() == 1);
    assert(a.front() == 3);
    assert(b.size() == 5);
    assert(b.front() == 1);

    // Test merging different lists
    a = {3, 5, 7};
    b = {2, 4, 6};
    a.merge(b);
    assert(b.empty());
    assert((a == xl::list<int>{2, 3, 4, 5, 6, 7}));
  }

  {
    constexpr size_t N = 100'000;
    xl::list<size_t> large;

    // Test forward filling
    for (size_t i = 0; i < N; ++i) {
        large.push_back(i);
    }
    assert(large.size() == N);

    // Test reverse filling
    xl::list<size_t> reversed;
    for (auto it = large.rbegin(); it != large.rend(); ++it) {
        reversed.push_back(*it);
    }
    assert(reversed.front() == N - 1);

    // Test sorting performance
    large.sort();
    assert(large.front() == 0);
    assert(large.back() == N - 1);
  }

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

  {
    // Test move insertion
    xl::list<std::unique_ptr<int>> ptr_list;
    ptr_list.push_back(std::make_unique<int>(42));
    ptr_list.emplace_back(new int(100));

    assert(**ptr_list.begin() == 42);

    // Test move assignment
    xl::list<std::unique_ptr<int>> moved_list = std::move(ptr_list);
    assert(ptr_list.empty());
    assert(moved_list.size() == 2);

    // Test move construction
    xl::list<std::unique_ptr<int>> new_list(std::move(moved_list));
    assert(moved_list.empty());
    assert(new_list.size() == 2);
  }

  {
    // Test empty range
    std::vector<int> empty_vec;
    xl::list<int> empty_list(empty_vec.begin(), empty_vec.end());
    assert(empty_list.empty());

    // Test single-element range
    int single = 42;
    xl::list<int> single_list(&single, &single + 1);
    assert(single_list.size() == 1);
    assert(single_list.front() == 42);

    // Test input iterator range
    std::istringstream iss("1 2 3 4");
    std::istream_iterator<int> iit(iss), eos;
    xl::list<int> input_list(iit, eos);
    assert(input_list.size() == 4);
    assert(input_list.back() == 4);
  }

  {
    xl::list<int> a = {1, 2, 3};
    xl::list<int> b = {1, 2, 3};
    xl::list<int> c = {1, 2, 4};
    xl::list<int> d = {1, 2};

    assert(a == b);
    assert(a != c);
    assert(a != d);
    assert(d < a);
    assert(c > a);
    assert(d <= a);
    assert(a >= b);
    assert(c >= a);
  }

  {
    int const a[10]{};
    assert(std::ranges::equal(a, xl::list(a)));
  }

  // 1. Copy construction from a temporary
  {
    xl::list<int> src = { 7, 8, 9 };
    auto dst(src);          // copy-ctor
    assert((dst == xl::list<int>{7, 8, 9}));
    assert((src == xl::list<int>{7, 8, 9}));   // src unchanged
  }

  // 2. Move construction from a temporary
  {
    xl::list<int> dst(xl::list<int>{1, 2, 3}); // move-ctor from prvalue
    assert((dst == xl::list<int>{1, 2, 3}));
  }

  // 3. Copy assignment followed by move assignment
  {
    xl::list<int> a = {4, 5};
    xl::list<int> b = {6, 7, 8};

    a = b;                       // copy-assign
    assert((a == xl::list<int>{6, 7, 8}));
    assert((b == xl::list<int>{6, 7, 8}));     // b unchanged

    b = xl::list<int>{9, 10};    // move-assign from prvalue
    assert((b == xl::list<int>{9, 10}));
  }

  // 4. Iterator stability after copy/move
  {
    xl::list<int> original = {10, 20, 30, 40, 50};
    auto it = std::next(original.begin(), 2);   // points to 30
    assert(*it == 30);

    xl::list<int> copy = original;              // deep copy
    *it = 99;                                   // mutate original
    assert((original == xl::list<int>{10, 20, 99, 40, 50}));
    assert((copy   == xl::list<int>{10, 20, 30, 40, 50})); // copy untouched

    xl::list<int> moved = std::move(original);  // move
    assert(original.empty());                   // NOLINT(bugprone-use-after-move)
    assert((moved  == xl::list<int>{10, 20, 99, 40, 50}));
  }

  // 5. Self-assignment guard
  {
    xl::list<int> lst = {11, 22, 33};
    lst = lst;                                  // self-assign
    assert((lst == xl::list<int>{11, 22, 33}));
  }


  // Test 1: Iterator arithmetic and bounds checking
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    auto it = lst.begin();
    
    // Test increment/decrement at boundaries
    auto end_it = lst.end();
    --end_it; // Should point to last element
    assert(*end_it == 5);
    
    ++end_it; // Should be back to end
    assert(end_it == lst.end());
    
    // Test distance calculation
    assert(std::distance(lst.begin(), lst.end()) == 5);
  }

  // Test 2: Const correctness
  {
    const xl::list<int> const_lst = {10, 20, 30};
    
    // Test const methods
    assert(const_lst.size() == 3);
    assert(const_lst.front() == 10);
    assert(const_lst.back() == 30);
    assert(!const_lst.empty());
    
    // Test const iterators
    auto cit = const_lst.cbegin();
    assert(*cit == 10);
    
    int sum = 0;
    for (auto it = const_lst.cbegin(); it != const_lst.cend(); ++it) {
        sum += *it;
    }
    assert(sum == 60);
  }

  // Test 3: Memory allocation patterns
  {
    xl::list<int> lst;
    
    // Test repeated push/pop patterns
    for (int cycle = 0; cycle < 100; ++cycle) {
      for (int i = 0; i < 10; ++i) {
        lst.push_back(i);
      }
      for (int i = 0; i < 5; ++i) {
        lst.pop_back();
      }
    }
    assert(lst.size() == 500); // 100 cycles * 5 net additions
    
    lst.clear();
    assert(lst.empty());
  }

  // Test 6: Iterator invalidation scenarios
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    std::vector<decltype(lst.begin())> iterators;
    
    // Store iterators to each element
    for (auto it = lst.begin(); it != lst.end(); ++it) {
      iterators.push_back(it);
    }
    
    // Erase middle element - only that iterator should be invalidated
    auto middle_it = std::next(lst.begin(), 2);
    lst.erase(middle_it);
    
    // Other iterators should still be valid
    assert(*iterators[0] == 1);
    assert(*iterators[1] == 2);
    // iterators[2] is invalidated
    assert(*iterators[3] == 4);
    assert(*iterators[4] == 5);
  }

  // Test 7: Splice edge cases
  {
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2 = {4, 5, 6};
    
    // Splice empty range
    lst1.splice(lst1.end(), lst2, lst2.end(), lst2.end());
    assert(lst1.size() == 3);
    assert(lst2.size() == 3);

    // Splice single element to itself (should be no-op)
    auto it = std::next(lst1.begin());
    lst1.splice(it, lst1, it);
    assert((lst1 == xl::list<int>{1, 2, 3}));

    // Splice range that includes insertion point
    lst1.splice(lst1.begin(), lst1, std::next(lst1.begin()), lst1.end());
    assert((lst1 == xl::list<int>{2, 3, 1}));
  }

  // Test 8: Merge with custom predicates
  {
    xl::list<int> lst1 = {5, 3, 1};
    xl::list<int> lst2 = {6, 4, 2};
    
    // Merge in descending order
    lst1.merge(lst2, std::greater<int>());
    assert((lst1 == xl::list<int>{6, 5, 4, 3, 2, 1}));
    assert(lst2.empty());
  }

  // Test 9: Unique with custom predicates
  {
    xl::list<int> lst = {1, 2, 2, 3, 4, 4, 4, 5};
    
    // Remove consecutive duplicates (default behavior)
    auto original_size = lst.size();
    auto removed = lst.unique();
    assert(removed > 0);
    assert(lst.size() < original_size);
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
    
    // Custom predicate - remove elements that differ by 1
    lst = {1, 2, 4, 5, 7, 8};
    lst.unique([](int a, int b) { return abs(a - b) <= 1; });
    assert(lst.size() < 6);
  }

  // Test 10: Remove with complex predicates
  {
    xl::list<int> lst = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Remove even numbers
    lst.remove_if([](int x) { return x % 2 == 0; });
    assert(lst.size() == 5);
    
    // Verify only odd numbers remain
    for (const auto& val : lst) {
      assert(val % 2 == 1);
    }
  }

  // Test 1: Large list operations
  {
    const size_t N = 50000;
    xl::list<size_t> large_list;
    
    // Fill with sequential numbers
    for (size_t i = 0; i < N; ++i) {
        large_list.push_back(i);
    }
    assert(large_list.size() == N);
    
    // Test find operation
    auto found = xl::find(large_list, N/2);
    assert(found && *found == N/2);
    
    // Test reverse operation
    large_list.reverse();
    assert(large_list.front() == N-1);
    assert(large_list.back() == 0);
    
    // Test sort operation
    large_list.sort();
    assert(large_list.front() == 0);
    assert(large_list.back() == N-1);
  }

  // Test 2: Fragmented memory usage
  {
    xl::list<int> lst;
    
    // Create fragmented access pattern
    for (int i = 0; i < 1000; ++i) {
      lst.push_back(i);
      if (i % 3 == 0) {
        lst.pop_front();
      }
    }
    
    // Verify integrity
    assert(!lst.empty());
    auto size = lst.size();
    lst.sort();
    assert(lst.size() == size); // Size shouldn't change after sort
  }

  // Test 1: Container concept requirements
  {
    xl::list<int> lst = {1, 2, 3};
    
    // Test Container requirements
    static_assert(std::is_same_v<xl::list<int>::value_type, int>);
    static_assert(std::is_same_v<xl::list<int>::reference, int&>);
    static_assert(std::is_same_v<xl::list<int>::const_reference, const int&>);
    
    // Test that iterators satisfy requirements
    static_assert(std::bidirectional_iterator<xl::list<int>::iterator>);
    static_assert(std::bidirectional_iterator<xl::list<int>::const_iterator>);
  }

  // Test 2: Sequence container requirements
  {
    xl::list<int> lst;
    
    // Test constructors required by SequenceContainer
    xl::list<int> lst1;
    xl::list<int> lst2(5);
    xl::list<int> lst3(5, 42);
    xl::list<int> lst4{1, 2, 3, 4, 5};
    
    assert(lst1.empty());
    assert(lst2.size() == 5);
    assert(lst3.size() == 5 && lst3.front() == 42);
    assert(lst4.size() == 5 && lst4.back() == 5);
  }

  // Test 3: Allocator-aware container requirements
  {
    xl::list<int> lst;

    // Test max_size
    assert(lst.max_size() > 0);

    // Test that list works with standard algorithms
    lst = {5, 2, 8, 1, 9};
    assert(!std::is_sorted(lst.begin(), lst.end()));

    lst.sort();
    assert(std::is_sorted(lst.begin(), lst.end()));
  }

  // Test 1: Circular operations
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
 
    // Rotate elements using splice
    auto it = std::next(lst.begin(), 2);
    lst.splice(lst.end(), lst, lst.begin(), it);
    assert((lst == xl::list<int>{3, 4, 5, 1, 2}));

    // Rotate back
    it = std::next(lst.begin(), 3);
    lst.splice(lst.begin(), lst, it, lst.end());
    assert((lst == xl::list<int>{1, 2, 3, 4, 5}));
  }

  // Test 2: Nested container operations
  {
    xl::list<xl::list<int>> nested;
    nested.push_back({1, 2, 3});
    nested.push_back({4, 5, 6});
    nested.push_back({7, 8, 9});
    
    assert(nested.size() == 3);
    assert(nested.front().size() == 3);
    assert(nested.back().back() == 9);
    
    // Flatten nested structure
    xl::list<int> flattened;
    for (const auto& inner : nested) {
      for (const auto& val : inner) {
        flattened.push_back(val);
      }
    }
    assert(flattened.size() == 9);
    assert(flattened.back() == 9);
  }

  // Test 4: Thread-safety considerations (basic)
  {
    // Test that independent operations don't interfere
    xl::list<int> lst1 = {1, 2, 3};
    xl::list<int> lst2 = {4, 5, 6};
    
    // Operations on separate lists
    lst1.push_back(7);
    lst2.push_front(0);

    assert(lst1.back() == 7);
    assert(lst2.front() == 0);

    // Move between lists
    lst1.splice(lst1.end(), std::move(lst2));
    assert(lst2.empty());
    assert(lst1.size() == 8);
  }

  // Test 5: Range-based algorithms
  {
    xl::list<int> lst = {1, 2, 3, 4, 5};
    
    // Test with standard algorithms
    auto sum = std::accumulate(lst.begin(), lst.end(), 0);
    assert(sum == 15);
    
    // Test transform
    std::transform(lst.begin(), lst.end(), lst.begin(), 
                  [](int x) { return x * 2; });
    assert(lst.front() == 2);
    assert(lst.back() == 10);
    
    // Test count_if
    auto even_count = std::count_if(lst.begin(), lst.end(),
                        [](int x) { return x % 2 == 0; });
    assert(even_count == 5); // All should be even now
  }
}

int main() {
  // Run the test suite
  test1();
  test2();
  test3();

  std::cout << "All tests passed!" << std::endl;

  return 0;
}

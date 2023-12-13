#include <cassert>
#include <iostream>
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
  list1.push_back(1);
  list1.push_back(2);
  list2.push_back(3);
  list2.push_back(4);
  list2.push_back(5);
  list1.merge(std::move(list2));
  assert(list1.size() == 5);
  assert(list1.front() == 1);
  assert(list1.back() == 5);

  // Test 6: Merge two lists with the same elements
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

  // Test 7: Merge two lists with different sizes and elements
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

  // Test 8: Merge two lists with the same size and elements
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

  // Test 9: Merge two lists with different sizes and elements, and check the order
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
  assert(list1.at(1) == 2);
  assert(list1.at(2) == 3);
  assert(list1.at(3) == 4);

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
  assert(list9 == xl::list<int>({1, 2, 3, 4, 5}) && list10.empty());

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
  std::vector<int> vec{4, 5, 6};

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
  std::vector<int> const vector1 = {6, 7, 8, 9, 10};

  // Use assign_range() to replace elements in list1 with elements from vector1
  list1.assign_range(vector1);

  // Check the results using assert()
  assert(list1.size() == vector1.size());
  auto list_it = list1.begin();
  auto vector_it = vector1.begin();
  while (list_it != list1.end()) {
      assert(*list_it == *vector_it);
      ++list_it;
      ++vector_it;
  }
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

  lst.erase(std::remove_if(lst.begin(), lst.end(), [](int i){ return i % 2 == 0; }), lst.end());
  assert(std::find_if(lst.begin(), lst.end(), [](int i){ return i % 2 == 0; }) == lst.end());
  }

  {
  xl::list<int> lst = {1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  xl::erase(lst, 2);
  assert(std::ranges::find(lst, 2) == lst.end());

  xl::erase_if(lst, [](int i){ return i % 2 == 0; });
  assert(std::ranges::find_if(lst, [](int i){ return i % 2 == 0; }) == lst.end());
  }

  {
  xl::list<int> myList = {1, 2, 3, 4, 5, 3, 6, 3, 7};

  // Using std::erase() to remove a specific value (e.g., 3) from the list
  xl::erase(myList, 3);

  // Using std::erase_if() to remove elements that satisfy a specific condition (e.g., value greater than 4)
  xl::erase_if(myList, [](int value) { return value > 4; });

  // Asserting the expected results
  assert(std::find(myList.begin(), myList.end(), 3) == myList.end()); // 3 should be removed
  assert(std::none_of(myList.begin(), myList.end(), [](int value) { return value > 4; })); // No value greater than 4 should remain
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
}

int main() {
  // Run the test suite
  test1();
  test2();

  std::cout << "All tests passed!" << std::endl;

  return 0;
}

#include <cassert>
#include <iostream>
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
  xl::list<char> palindrome(std::string_view("racecar"));

  while(palindrome.size() > 1) {
    assert(palindrome.front() == palindrome.back());
    palindrome.pop_front();
    palindrome.pop_back();
  }

  assert(1 == palindrome.size());
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
  xl::list<int> l3(arr);
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

  /* {
  // splice() entire list
  std::list<int> l29{1, 2, 3};
  std::list<int> l30{4, 5, 6};
  l29.splice(l29.begin(), l30);
  assert(l29.size() == 6);
  assert(l30.empty());
  }

  {
  // splice() single element
  std::list<int> l31{1, 2, 3};
  std::list<int> l32{4, 5, 6};
  l31.splice(l31.begin(), l32, l32.begin());
  assert(l31.front() == 4);
  assert(l31.size() == 4);
  assert(l32.size() == 2);
  }

  {
  // splice() element range
  std::list<int> l33{1, 2, 3};
  std::list<int> l34{4, 5, 6};
  l33.splice(l33.begin(), l34, std::next(l34.begin()), l34.end());
  assert(l33.size() == 5);
  assert(l34.size() == 1);
  } */

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

#include <iostream>
#include "list.hpp"
 
void print(int id, const xl::list<int>& container)
{
  std::cout << id << ". ";
  for (const int x : container)
    std::cout << x << ' ';
  std::cout << '\n';
}

void print_container(const xl::list<int>& c) 
{
  for (int i : c)
    std::cout << i << ' ';
  std::cout << '\n';
}

std::ostream& operator<<(std::ostream& ostr, const xl::list<int>& list)
{
  for (auto& i : list) ostr << ' ' << i; return ostr;
}

int main ()
{
  { // https://en.cppreference.com/w/cpp/container/list/insert
  xl::list<int> c1(3, 100);
  print(1, c1);

  auto it = c1.begin();
  it = c1.insert(it, 200);
  print(2, c1);

  c1.insert(it, 2, 300);
  print(3, c1);

  // reset `it` to the begin:
  it = c1.begin();

  xl::list<int> c2(2, 400);
  c1.insert(std::next(it, 2), c2.begin(), c2.end());
  print(4, c1);

  int arr[] = {501, 502, 503};
  c1.insert(c1.begin(), arr, arr + std::size(arr));
  print(5, c1);

  c1.insert(c1.end(), {601, 602, 603});
  print(6, c1);
  }

  { // https://en.cppreference.com/w/cpp/container/list/erase
  xl::list<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  print_container(c);

  c.erase(c.begin());
  print_container(c);

  auto range_begin = c.begin();
  auto range_end = c.begin();
  std::advance(range_begin, 2);
  std::advance(range_end, 5);

  c.erase(range_begin, range_end);
  print_container(c);

  // Erase all even numbers
  for (auto it = c.begin(); it != c.end();)
  {
    if (*it % 2 == 0)
      it = c.erase(it);
    else
      ++it;
  }
  print_container(c);
  }

  { // https://en.cppreference.com/w/cpp/container/list
  // Create a list containing integers
  xl::list<int> l = {7, 5, 16, 8};

  // Add an integer to the front of the list
  l.push_front(25);
  // Add an integer to the back of the list
  l.push_back(13);

  // Insert an integer before 16 by searching
  auto it = std::find(l.begin(), l.end(), 16);
  if (it != l.end())
      l.insert(it, 42);

  // Print out the list
  std::cout << "l = { ";
  for (int n : l)
      std::cout << n << ", ";
  std::cout << "};\n";
  }

  { // https://en.cppreference.com/w/cpp/container/list/splice
  xl::list<int> list1{1, 2, 3, 4, 5};
  xl::list<int> list2{10, 20, 30, 40, 50};

  auto it = list1.begin();
  std::advance(it, 2);

  list1.splice(it, std::move(list2));

  std::cout << "list1:" << list1 << '\n';
  std::cout << "list2:" << list2 << '\n';

  it = std::prev(list1.end(), 3); // it is invalidated

  list2.splice(list2.begin(), std::move(list1), it, list1.end());

  std::cout << "list1:" << list1 << '\n';
  std::cout << "list2:" << list2 << '\n';
  }

  { // https://en.cppreference.com/w/cpp/container/list/merge
  xl::list<int> list1 = {5, 9, 1, 3, 3};
  xl::list<int> list2 = {8, 7, 2, 3, 4, 4};

  list1.sort();
  list2.sort();
  std::cout << "list1: " << list1 << '\n';
  std::cout << "list2: " << list2 << '\n';

  list1.merge(list2);
  std::cout << "merged:" << list1 << '\n';
  }

  return 0;
}


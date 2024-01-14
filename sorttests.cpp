#include <cassert>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <numeric>

#include "list.hpp"

int main()
{
  std::list<int> l1(100000);
  std::iota(l1.rbegin(), l1.rend(), 0);
  xl::list l2(xl::from_range, l1);
  xl::list l3(xl::from_range, l1);

  // Measure the time it takes to sort the vector using std::sort
  auto start = std::chrono::high_resolution_clock::now();
  l1.sort();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> std_sort_time = end - start;

  std::cout << "std::sort time: " << std_sort_time.count() << " seconds" << std::endl;

  // Measure the time it takes to sort the vector using your own sort implementation
  start = std::chrono::high_resolution_clock::now();
  l2.sort();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> xl_sort_time = end - start;

  // Print the results
  std::cout << "xl::sort time: " << xl_sort_time.count() << " seconds" << std::endl;
  assert(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));

  start = std::chrono::high_resolution_clock::now();
  l3.sort2();
  end = std::chrono::high_resolution_clock::now();
  xl_sort_time = end - start;

  std::cout << "xl::sort2 time: " << xl_sort_time.count() << " seconds" << std::endl;
  assert(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));

  return 0;
}

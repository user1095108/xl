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
  xl::list<int> l2(xl::from_range, l1);

  // Measure the time it takes to sort the vector using std::sort
  auto start = std::chrono::high_resolution_clock::now();
  l1.sort();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> std_sort_time = end - start;

  // Measure the time it takes to sort the vector using your own sort implementation
  start = std::chrono::high_resolution_clock::now();
  l2.sort();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> xl_sort_time = end - start;

  // Print the results
  std::cout << "std::sort time: " << std_sort_time.count() << " seconds" << std::endl;
  std::cout << "xl::sort time: " << xl_sort_time.count() << " seconds" << std::endl;
  assert(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));

  return 0;
}

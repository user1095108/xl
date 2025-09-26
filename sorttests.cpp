#include <cassert>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <random>

#include "list.hpp"

void test_run(std::string_view const& title, auto& l1)
{
  std::cout << "=== " << title << " ===" << std::endl;

  // Prepare
  xl::list l2(xl::from_range, l1);
  xl::list l3(xl::from_range, l1);

  decltype(std::chrono::high_resolution_clock::now()) start, end;

  // Measure
  start = std::chrono::high_resolution_clock::now();
  l1.sort();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const std_sort_time(end - start);

  start = std::chrono::high_resolution_clock::now();
  l2.sort();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const xl_sort_time(end - start);

  start = std::chrono::high_resolution_clock::now();
  l3.template sort<1>();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const xl_sort_time2(end - start);

  // Print the results
  std::cout << "std::list::sort time: " << std_sort_time.count() << " seconds" << std::endl;
  std::cout << "xl::sort time: " << xl_sort_time.count() << " seconds" << std::endl;
  std::cout << "xl::sort2 time: " << xl_sort_time2.count() << " seconds" << std::endl;

  assert(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
  assert(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));
}

int main()
{
  constexpr std::size_t N(200000);
  std::list<int> l(N);

  //
  std::iota(l.rbegin(), l.rend(), 0);

  test_run("reverse-sorted", l);

  //
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<int> dist(
    std::numeric_limits<int>::min()
  );

  std::generate(l.begin(), l.end(), [&]{ return dist(gen); });

  test_run("random", l);

  //
  std::iota(l.begin(), l.end(), 0);

  test_run("already sorted", l);

  //
  for (std::size_t i = 0; i < N / 1000; ++i)
    *std::next(l.begin(), gen() % N) = dist(gen);

  test_run("mostly-sorted", l);

  //
  std::fill(l.begin(), l.end(), dist(gen));

  test_run("constant-value", l);

  //
  auto it = l.begin();
  for (std::size_t i = 0; i != N / 2; ++i) *it++ = i;
  for (std::size_t i = N / 2; i != N;  ++i) *it++ = N - i;

  test_run("organ-pipe", l);

  //
  return 0;
}

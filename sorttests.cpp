#include <cassert>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <random>

#include "list.hpp"

template <template <typename...> class L, int N = -1>
bool is_stable_sort()
{
  auto const v(
    std::views::iota(0, 1000) |
    std::views::transform(
      [](int const i) noexcept
      {
        return std::pair{i % 3, i};
      }
    )
  );

  L<std::pair<int, int>> c(v.begin(), v.end());

  auto const cmp([](auto const& a, auto const& b) noexcept
    { return std::get<0>(a) < std::get<0>(b); });

  if constexpr (requires { c.sort(cmp); })
  {
    if constexpr (N > 0)
      c.template sort<N>(cmp);
    else
      c.sort(cmp);
  }
  else
    std::ranges::sort(c, cmp);

  if (!std::ranges::is_sorted(c, cmp)) return false;

  return std::ranges::adjacent_find(std::as_const(c),
    [](auto const& l, auto const& r) noexcept
    { // equal keys but wrong order - not stable
      return l.first == r.first && l.second > r.second;
    }
  ) == c.cend();
}

void test_run(std::string_view const& title, auto& l1)
{
  std::cout << "=== " << title << " ===" << std::endl;

  // Prepare
  xl::list l2(xl::from_range, l1);
  xl::list l3(xl::from_range, l1);
  xl::list l4(xl::from_range, l1);
  xl::list l5(xl::from_range, l1);
  xl::list l6(xl::from_range, l1);

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
  std::chrono::duration<double> const xl_sort_time1(end - start);

  start = std::chrono::high_resolution_clock::now();
  l4.template sort<2>();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const xl_sort_time2(end - start);

  start = std::chrono::high_resolution_clock::now();
  l5.template sort<3>();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const xl_sort_time3(end - start);

  start = std::chrono::high_resolution_clock::now();
  l6.template sort<4>();
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const xl_sort_time4(end - start);

  // Print the results
  std::cout << "std::list::sort time: " << std_sort_time.count() << " seconds" << std::endl;
  std::cout << "xl::sort time: " << xl_sort_time.count() << " seconds" << std::endl;
  std::cout << "xl::sort1 time: " << xl_sort_time1.count() << " seconds" << std::endl;
  std::cout << "xl::sort2 time: " << xl_sort_time2.count() << " seconds" << std::endl;
  std::cout << "xl::sort3 time: " << xl_sort_time3.count() << " seconds" << std::endl;
  std::cout << "xl::sort4 time: " << xl_sort_time4.count() << " seconds" << std::endl;

  assert(l1 == l2);
  assert(l1 == l3);
  assert(l1 == l4);
  assert(l1 == l5);
  assert(l1 == l6);
}

int main()
{
  std::cout << "std::list::sort is stable? " << is_stable_sort<std::list>() << std::endl;
  std::cout << "xl::list::sort is stable? " << is_stable_sort<xl::list>() << std::endl;
  std::cout << "xl::list::sort1 is stable? " << is_stable_sort<xl::list, 1>() << std::endl;
  std::cout << "xl::list::sort2 is stable? " << is_stable_sort<xl::list, 2>() << std::endl;
  std::cout << "xl::list::sort3 is stable? " << is_stable_sort<xl::list, 3>() << std::endl;
  std::cout << "xl::list::sort4 is stable? " << is_stable_sort<xl::list, 4>() << std::endl;

  constexpr std::size_t N(200000);
  std::list<int> l(N);

  //
  std::iota(l.rbegin(), l.rend(), 0);
  test_run("reverse-sorted", l);

  //
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<int> dist(
    std::numeric_limits<int>::min(),
    std::numeric_limits<int>::max()
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
  {
    auto it = l.begin();
    for (std::size_t i = 0; i != N / 2; ++i) *it++ = i;
    for (std::size_t i = N / 2; i != N;  ++i) *it++ = N - i;
  }
  test_run("organ-pipe", l);

  // Sawtooth
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back(i % 1000);
  test_run("sawtooth", l);

  // Checkerboard (alternating high-low)
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back((i % 2 == 0) ? i : N - i);
  test_run("checkerboard", l);

  // Two-valued (binary)
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back(i % 2);
  test_run("two-valued", l);

  // Ascending blocks
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back((i / 1000) * 1000 + (i % 1000));
  test_run("ascending-blocks", l);

  // Descending blocks
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back((i / 1000) * 1000 + (999 - (i % 1000)));
  test_run("descending-blocks", l);

  // Gaussian distribution
  {
    std::normal_distribution<> normal_dist(N / 2, N / 10);
    l.clear();
    for (std::size_t i = 0; i < N; ++i)
      l.push_back(static_cast<int>(normal_dist(gen)));
    test_run("gaussian", l);
  }

  // V-shape
  l.clear();
  for (std::size_t i = 0; i < N / 2; ++i) l.push_back(N/2 - i);
  for (std::size_t i = 0; i < N / 2; ++i) l.push_back(i);
  test_run("v-shape", l);

  // Duplicate-heavy
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back((i % 100 == 0) ? dist(gen) : 42);
  test_run("duplicate-heavy", l);

  // Single outlier
  std::iota(l.begin(), l.end(), 0);
  *std::next(l.begin(), N / 2) = N * 10;
  test_run("single-outlier", l);

  // Reversed tail
  std::iota(l.begin(), l.end(), 0);
  std::reverse(std::next(l.begin(), N - N/10), l.end());
  test_run("reversed-tail", l);

  // Sorted with random noise
  std::iota(l.begin(), l.end(), 0);
  for (std::size_t i = 0; i < N; i += 100)
    *std::next(l.begin(), i) = dist(gen);
  test_run("sorted-with-noise", l);

  // Small range (many duplicates)
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back(dist(gen) % 100);
  test_run("small-range", l);

  // Large gaps
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back(i * 1000);
  test_run("large-gaps", l);

  // Bit-reversed order
  {
    auto bit_reverse = [](unsigned x, unsigned bits) {
      unsigned r = 0;
      for (unsigned i = 0; i < bits; ++i) {
        r = (r << 1) | (x & 1);
        x >>= 1;
      }
      return r;
    };
    l.clear();
    unsigned bits = std::ceil(std::log2(N));
    for (unsigned i = 0; i < N; ++i)
      l.push_back(bit_reverse(i, bits));
    test_run("bit-reversed", l);
  }

  // Fibonacci mod N
  l.clear();
  {
    int a = 0, b = 1;
    for (std::size_t i = 0; i < N; ++i) {
      l.push_back(a % N);
      int next = a + b;
      a = b; b = next;
    }
  }
  test_run("fibonacci-mod", l);

  // Plateau (half constant, half increasing)
  l.clear();
  for (std::size_t i = 0; i < N/2; ++i) l.push_back(42);
  for (std::size_t i = N/2; i < N; ++i) l.push_back(i);
  test_run("plateau", l);

  // Zig-zag
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back((i % 2 == 0) ? i : N - i);
  test_run("zig-zag", l);

  // Geometric growth
  l.clear();
  {
    long long v = 1;
    for (std::size_t i = 0; i < N; ++i) {
      l.push_back(v % 1000000);
      v *= 2;
    }
  }
  test_run("geometric", l);

  // Primes mod N
  {
    auto is_prime = [](int x) {
      if (x < 2) return false;
      for (int i = 2; i*i <= x; ++i)
        if (x % i == 0) return false;
      return true;
    };
    l.clear();
    for (int i = 0, count = 0; count < (int)N; ++i) {
      if (is_prime(i)) {
        l.push_back(i % N);
        ++count;
      }
    }
    test_run("primes", l);
  }

  // Interleaved sorted runs
  l.clear();
  for (std::size_t i = 0; i < N; ++i)
    l.push_back((i % 2 == 0) ? i/2 : (N/2 + i/2));
  test_run("interleaved-runs", l);

  return 0;
}

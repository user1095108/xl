#include <iostream>
#include <memory>

#include "list.hpp"

int main()
{
  xl::list<int> l(xl::multi_t{}, 3u, 4ll, 5ull);

  l.push_back(0ull, 1ll);
  l.push_front(1l, 2u, 3ll);

  erase(l, 1, 1ull, 1ll);

  //
  std::cout << "size: " << l.size() << std::endl;

  l.sort();
  l.reverse();

  std::for_each(
    l.crbegin(),
    l.crend(),
    [](auto&& p) noexcept
    {
      std::cout << p << std::endl;
    }
  );

  //
  l.pop_back();
  l.pop_front();
  l.erase(std::prev(l.cend()));

  std::cout << "size: " << l.size() << std::endl;

  std::for_each(
    l.cbegin(),
    l.cend(),
    [](auto&& p) noexcept
    {
      std::cout << p << std::endl;
    }
  );

  decltype(l) a(l), b(std::move(l));

  std::cout << a.size() << " " << b.size() << " " << (a == b) << std::endl;

  xl::list<std::unique_ptr<int>> m(xl::multi_t{}, std::make_unique<int>(2));

  m.push_back(std::make_unique<int>(1));
  m.insert(xl::multi_t{}, m.begin(), std::make_unique<int>(3), std::make_unique<int>(4));
  m.insert(xl::multi_t{}, m.end(), std::make_unique<int>(3), std::make_unique<int>(4));

  return 0;
}

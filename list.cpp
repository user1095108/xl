#include <iostream>
#include <memory>
#include <list>

#include "list.hpp"

int main()
{
  xl::list<int> l(xl::push_t{}, 3, 4, 5);

  l.push_back(0ull, 1ll);
  l.push_front(1l, 2u, 3ll);

  erase(l, 1);
  erase(l, 1ull);

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

  xl::list<std::unique_ptr<int>> m(xl::push_t{}, std::make_unique<int>(2));

  m.push_back(std::make_unique<int>(1));

  return 0;
}

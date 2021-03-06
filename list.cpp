#include <iostream>

#include "list.hpp"

int main()
{
  xl::list<int> l{3, 4, 5};

  l.emplace_back(0);
  l.emplace_back(1);

  l.insert(l.cend(), {1, 2, 3});

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

  return 0;
}

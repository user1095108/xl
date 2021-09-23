#include <iostream>

#include "xl.hpp"

int main()
{
  xl::list<int> l{3, 4, 5};

  l.emplace_back(0);
  l.emplace_back(1);

  //
  std::cout << "size: " << l.size() << std::endl;

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

  return 0;
}

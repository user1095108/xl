# xl
This is a c++ implementation of an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list), a [doubly linked list](https://en.wikipedia.org/wiki/Doubly_linked_list) with reduced storage requirements, and a model of a `(container, iterator)` pair implementation.

As usual with XOR containers, when you erase an iterator, the preceding and succeeding iterators, but not `end()`, are invalidated but can still be dereferenced. This means that you can still access the values of the elements before and after the erased iterator, but you cannot use those invalidated iterators for any other operations like incrementing.

[STL](https://en.wikipedia.org/wiki/Standard_Template_Library) containers are often slowed by various safety checks, exception safety and legacy code. A general rule of thumb seems to be, that pretty much anything we implement ourselves will outperform an [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) implementation; we have more flexibility than [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) programmers and can adapt/optimize our implementations to our particular needs.

# build instructions
    g++ -std=c++20 -Ofast list.cpp -o l

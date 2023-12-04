# xl
This is a c++ implementation of an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list), a [doubly linked list](https://en.wikipedia.org/wiki/Doubly_linked_list) with reduced storage requirements, and a model of a `(container, iterator)` pair implementation.

[STL](https://en.wikipedia.org/wiki/Standard_Template_Library) containers are often slowed by various safety checks, exception safety and legacy code. A general rule of thumb seems to be, that pretty much anything we implement ourselves will outperform an [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) implementation in some way; we have more flexibility than [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) programmers and can adapt/optimize our implementations to our particular needs.

As usual with XOR containers, when you erase an iterator, the succeeding iterator, but not `end()`, is invalidated. However, this iterator can still be dereferenced. You should not use invalidated iterators for any operations, other than dereferencing. The same applies to insertion.

# build instructions
    g++ -std=c++20 -Ofast list.cpp -o l

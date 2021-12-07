# xl
This is a c++ implementation of an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list), a [doubly linked list](https://en.wikipedia.org/wiki/Doubly_linked_list) with reduced storage requirements, and a model of a `(container, iterator)` pair implementation.

[STL](https://en.wikipedia.org/wiki/Standard_Template_Library) containers are generally slow, probably because of various safety checks, exception safety and legacy code. I wanted to see whether an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list) would outperform `std::list` in my use case and it did. A general **rule of thumb**, seems to be, that pretty much anything we implement ourselves will outperform an [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) implementation; we have more flexibility than [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) programmers and can adapt/optimize our implementation to our particular needs.

# build instructions
    g++ -std=c++20 -Ofast xl.cpp -o xl

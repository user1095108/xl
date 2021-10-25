# xl
This is a c++ implementation of an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list), a [doubly linked list](https://en.wikipedia.org/wiki/Doubly_linked_list) with reduced storage requirements.

[STL](https://en.wikipedia.org/wiki/Standard_Template_Library) containers are generally slow, probably because of various safety checks, exception safety and legacy code. I wanted to see whether an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list) would outperform `std::list` in my use case and it did. A general **rule of thumb**, [<ins>**even though it is discouraged**</ins>](https://stackoverflow.com/questions/6831231/should-programmers-use-stl-or-write-their-own-code), seems to be, that pretty much anything we implement ourselves will outperform an [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) implementation and this example is a model of an easy-to-understand `(container, iterator)` pair.

# build instructions
    g++ -std=c++20 -Ofast xl.cpp -o xl

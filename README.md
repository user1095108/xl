# xl
This is an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list), a [doubly linked list](https://en.wikipedia.org/wiki/Doubly_linked_list) with reduced storage requirements.

[STL](https://en.wikipedia.org/wiki/Standard_Template_Library) containers are generally slow, probably because of various safety checks, exception throwing and legacy code. I wanted to see whether this strange linked list would outperform `std::list` in my use case and it did. It seems a general rule of thumb, <ins>**even though it is discouraged**</ins>, that pretty much anything you write will outperform the [STL](https://en.wikipedia.org/wiki/Standard_Template_Library) implementation.

# build instructions
    g++ -std=c++20 -Ofast xl.cpp -o xl

# xl
This is an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list), a [doubly linked list](https://en.wikipedia.org/wiki/Doubly_linked_list) with reduced storage requirements.

[STL](https://en.wikipedia.org/wiki/Standard_Template_Library) containers are generally slow, probably because of various safety checks and exception throwing. I wanted to see whether this strange linked list would outperform `stl::list` in my use case and it did. It seems a general rule of thumb, **even though it is discouraged**, that pretty much anything you write will outperform [STL](https://en.wikipedia.org/wiki/Standard_Template_Library).

# build instructions
    g++ -std=c++20 -Ofast xl.cpp -o xl

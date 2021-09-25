# xl
This is an [XOR linked list](https://en.wikipedia.org/wiki/XOR_linked_list), a [doubly linked list](https://en.wikipedia.org/wiki/Doubly_linked_list) with reduced storage requirements.

STL containers are generally slow, probably because of various safety checks and exception throwing. I wanted to see whether this strange linked list would outperform `stl::list` in my use case and it did.

# build instructions
    g++ -std=c++20 -Ofast xl.cpp -o xl

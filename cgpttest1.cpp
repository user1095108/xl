#include <cassert>
#include <iostream>
#include "list.hpp" // Replace with the actual container header

// Include your testing framework of choice (e.g., Google Test or Catch2)

// Function to test various operations of your random-access container
void testRandomAccessContainer() {
    // Create an instance of your container
    xl::list<int> container;

    // Test adding elements to the container
    container.push_back(1);
    container.push_back(2);
    container.push_back(3);

    // Test the size of the container
    assert(container.size() == 3);

    // Test accessing elements by index
    assert(container[0] == 1);
    assert(container[1] == 2);
    assert(container[2] == 3);

    // Test modifying elements
    container[1] = 42;
    assert(container[1] == 42);

    // Test removing elements
    container.pop_back();
    assert(container.size() == 2);

    // Add more test cases for your container's methods (e.g., clear, resize, etc.)

    // Test iterator functionality
    auto it = container.begin();
    assert(*it == 1);
    ++it;
    assert(*it == 42);
    ++it;
    assert(it == container.end());

    // Test edge cases
    container.clear();
    assert(container.empty());
    assert(container.size() == 0);

    // Additional test cases:

    // Test inserting elements at a specific position
    container.push_back(4);
    container.insert(std::next(container.begin()), 5);
    assert(container.size() == 2);
    assert(container[0] == 4);
    assert(container[1] == 5);

    // Test erasing elements at a specific position
    container.erase(container.begin());
    assert(container.size() == 1);
    assert(container[0] == 5);

    // Test swapping containers
    decltype(container) otherContainer;
    otherContainer.push_back(6, 7);
    container.swap(otherContainer);
    assert(container.size() == 2);
    assert(otherContainer.size() == 1);

    // Test copying a container
    decltype(container) copiedContainer = container;
    assert(copiedContainer.size() == 2);
    assert(copiedContainer[0] == 6);

    // Test range-based for loop
    int sum = 0;
    for (const auto& item : container) {
        sum += item;
    }
    assert(sum == 13);
}

int main() {
    // Run the test suite
    testRandomAccessContainer();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}

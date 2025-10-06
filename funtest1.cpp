#include "list.hpp"
#include <list>
#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>

struct Person {
    int id;
    std::string name;
    Person(int i = 0, std::string n = "") : id(i), name(std::move(n)) {}
    bool operator==(Person const& o) const { return id == o.id && name == o.name; }
    bool operator<(Person const& o) const { return id < o.id || (id == o.id && name < o.name); }
};

int main()
{
    /* ----------------------------------------------------------
     * 1.  EVERY ORIGINAL TEST (unchanged)
     * ---------------------------------------------------------- */
    // --- basics: push, emplace, front/back, size, empty
    xl::list<int> a;
    assert(a.empty());
    a.push_back(1);
    a.push_front(0);
    a.emplace_back(2); // now: 0,1,2
    assert(a.size() == 3);
    {
        int expected[] = {0,1,2};
        assert(std::equal(a.begin(), a.end(), std::begin(expected)));
    }

    // --- insert before an iterator
    auto it = a.begin(); ++it; // points to 1
    a.insert(it, 42); // 0,42,1,2
    {
        int expected[] = {0,42,1,2};
        assert(std::equal(a.begin(), a.end(), std::begin(expected)));
    }

    // --- erase returns iterator to next element
    it = std::find(a.begin(), a.end(), 42);
    assert(it != a.end());
    auto it_after = a.erase(it); // should point to 1
    assert(it_after != a.end() && *it_after == 1);
    assert(std::find(a.begin(), a.end(), 42) == a.end());

    // --- splice: move single element from one list to another
    xl::list<int> b = {10, 11, 12}; // b: 10,11,12
    auto b_it = std::next(b.begin()); // points to 11
    a.splice(a.begin(), b, b_it); // move 11 to front of a
    // a should now begin with 11, and b should be of size 2 with 11 removed
    assert(a.front() == 11);
    assert(b.size() == 2);
    assert(a.size() == 4); // previously 3, we moved one in -> 4

    // --- splice: move a range
    xl::list<int> c = {100, 200, 300, 400}; // c: 100,200,300,400
    auto c_first = std::next(c.begin());           // -> 200
    auto c_last  = std::next(c.begin(), 3);         // -> 400 (range is [200,300])
    a.splice(a.end(), c, c_first, c_last); // move 200,300 to end of a
    // c now should contain two elements: 100 and 400
    assert(c.size() == 2);
    {
        int expected_c[] = {100, 400};
        assert(std::equal(c.begin(), c.end(), std::begin(expected_c)));
    }

    // --- sort & merge (must sort before merge)
    xl::list<int> s1 = {5, 1, 3};
    xl::list<int> s2 = {4, 2, 6};
    s1.sort();
    s2.sort();
    s1.merge(s2);
    assert(s2.empty()); // merge empties source
    assert(std::is_sorted(s1.begin(), s1.end()));

    // --- unique: removes adjacent duplicates
    xl::list<int> u = {1,1,2,2,2,3,3};
    u.unique();
    {
        int expected[] = {1,2,3};
        assert(std::equal(u.begin(), u.end(), std::begin(expected)));
    }

    // --- reverse
    u.reverse(); // now 3,2,1
    {
        int expected[] = {3,2,1};
        assert(std::equal(u.begin(), u.end(), std::begin(expected)));
    }

    // --- remove_if and remove (user-defined type)
    xl::list<Person> ppl;
    ppl.emplace_back(1, "Alice");
    ppl.emplace_back(2, "Bob");
    ppl.emplace_back(3, "Eve");
    ppl.emplace_back(2, "Bob2"); // same id as Bob but different name

    // remove by value (requires exact match) - demonstrate remove on copy
    Person ptemp(3, "Eve");
    ppl.remove(ptemp); // should remove Eve
    assert(std::none_of(ppl.begin(), ppl.end(), [](const Person& p){ return p.id == 3; }));

    // remove_if to remove id==2
    ppl.remove_if([](const Person& p){ return p.id == 2; });
    assert(std::none_of(ppl.begin(), ppl.end(), [](const Person& p){ return p.id == 2; }));
    assert(ppl.size() == 1 && ppl.front().id == 1);

    // --- iterator stability: iterators to other elements remain valid across non-erasing ops
    ppl.push_back(Person(4, "Zed"));
    auto preserved = ppl.begin(); // points to id=1
    // do operations that don't erase the preserved element
    ppl.push_front(Person(0, "Zero"));
    ppl.emplace_back(5, "Last");
    // preserved must still point to the same element (id==1)
    assert(preserved != ppl.end());
    assert(preserved->id == 1 && preserved->name == "Alice");

    // --- assign, clear, swap
    xl::list<int> asgn;
    asgn.assign(5, 7); // five 7s
    assert(asgn.size() == 5);
    assert(asgn.front() == 7 && asgn.back() == 7);
    asgn.clear();
    assert(asgn.empty());

    xl::list<int> x = {1,2,3}, y = {9,8};
    x.swap(y);
    {
        int expected_x[] = {9,8};
        int expected_y[] = {1,2,3};
        assert(std::equal(x.begin(), x.end(), std::begin(expected_x)));
        assert(std::equal(y.begin(), y.end(), std::begin(expected_y)));
    }

    // --- merge of custom type with custom comparator
    xl::list<Person> alpha = { Person(1,"A"), Person(4,"D") };
    xl::list<Person> beta  = { Person(2,"B"), Person(3,"C") };
    alpha.sort([](const Person& a, const Person& b){ return a.id < b.id; });
    beta.sort([](const Person& a, const Person& b){ return a.id < b.id; });
    alpha.merge(beta, [](const Person& a, const Person& b){ return a.id < b.id; });
    assert(beta.empty());
    assert(alpha.size() == 4);
    // check order ids = 1,2,3,4
    {
        int idx = 1;
        for (const auto& p : alpha) {
            assert(p.id == idx++);
        }
    }

    // --- splice invariants check: sizes update correctly
    xl::list<int> L1 = {1,2,3};
    xl::list<int> L2 = {4,5};
    auto itL2 = std::next(L2.begin()); // -> 5
    L1.splice(std::next(L1.begin()), L2, itL2); // move '5' to L1 after first element
    assert(L1.size() == 4);
    assert(L2.size() == 1);
    assert(std::find(L1.begin(), L1.end(), 5) != L1.end());
    assert(std::find(L2.begin(), L2.end(), 5) == L2.end() || *L2.begin() == 4); // L2 now contains only 4

    /* ----------------------------------------------------------
     * 2.  NEW TESTS – corner cases & stress
     * ---------------------------------------------------------- */

    // 2.1  default ctor + clear + re-use
    {
        xl::list<int> z;
        assert(z.empty() && z.begin() == z.end());
        z.clear();                       // double clear must be safe
        assert(z.empty());
        z = {7,8,9};
        assert(z.size()==3 && z.back()==9);
    }

    // 2.2  range ctor from empty iterators
    {
        int* p = nullptr;
        xl::list<int> z(p, p);           // empty range
        assert(z.empty());
    }

    // 2.3  self-splice single element (should be no-op)
    {
        xl::list<int> z = {1,2,3};
        auto it = std::next(z.begin());
        z.splice(z.begin(), z, it);      // move '2' to front – still 3 elements
        assert(z.size()==3);
        assert(((int[]){2,1,3} == z));
    }

    // 2.4  self-splice whole list
    {
        xl::list<int> z = {1,2,3};
        z.splice(z.end(), z, z.begin(), z.end());
        assert(3 == z.size());
    }

    // 2.5  splice entire list into another, then reverse-merge
    {
        xl::list<int> A = {1,3,5};
        xl::list<int> B = {2,4,6};
        A.splice(A.end(), B);            // move all nodes
        assert(B.empty() && A.size()==6);
        assert(((int[]){1,3,5,2,4,6} == A));
    }

    // 2.6  merge with duplicates interleaved
    {
        xl::list<int> A = {1,3,3,7};
        xl::list<int> B = {2,3,3,8};
        A.sort();  B.sort();
        A.merge(B);
        assert(B.empty());
        assert(((int[]){1,2,3,3,3,3,7,8} == A));
    }

    // 2.7  unique with no duplicates, and with all equal
    {
        xl::list<int> no = {1,2,3};
        no.unique();                     // nothing changes
        assert(no.size()==3);

        xl::list<int> all = {5,5,5};
        all.unique();
        assert(all.size()==1 && all.front()==5);
    }

    // 2.8  remove_if that removes everything
    {
        xl::list<int> z = {1,1,1};
        z.remove_if([](int){ return true; });
        assert(z.empty());
    }

    // 2.9  large assign & iterator arithmetic
    {
        xl::list<int> z;
        z.assign(1'000'000, 42);
        assert(z.size()==1'000'000);
        assert(std::all_of(z.begin(), z.end(), [](int v){ return v==42; }));
        z.assign(0, 0);                  // shrink to zero
        assert(z.empty());
    }

    // 2.10  exception safety: ensure push/pop don't leak if copy throws
    static int cnt;
    struct ThrowOnCopy {
        int val{};
        ThrowOnCopy() = default;
        ThrowOnCopy(int v):val(v){}
        ThrowOnCopy(const ThrowOnCopy&){
            if(++cnt == 2) throw std::bad_alloc();
        }
    };
    {
        xl::list<ThrowOnCopy> z;
        z.emplace_back(1);
        z.emplace_back(2);
        bool caught = false;
        try {
            z.push_back(ThrowOnCopy(3)); // will throw
        } catch(const std::bad_alloc&) {
            caught = true;
        }
        assert(z.size()==2);             // list unchanged
        assert(caught);
    }

    // 2.11  move semantics (if your list supports move-only types)
    struct MoveOnly {
        std::unique_ptr<int> p;
        explicit MoveOnly(int v):p(std::make_unique<int>(v)){}
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
        MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
        int value() const { return *p; }
    };
    {
        xl::list<MoveOnly> z;
        z.emplace_back(7);
        z.emplace_front(5);
        assert(z.front().value()==5 && z.back().value()==7);
        z.pop_front();
        assert(z.front().value()==7);
    }

    // 2.12  const-correctness & cbegin/cend
    {
        const xl::list<int> z = {1,2,3};
        assert(*z.cbegin() == 1);
        assert(std::distance(z.cbegin(), z.cend()) == 3);
    }

    // 2.13  erase on end() must return end()
    {
        xl::list<int> z = {1};
        auto it = z.erase(z.begin());
        assert(it == z.end() && z.empty());
    }

    // 2.14  insert at end() appends
    {
        xl::list<int> z = {1,2};
        z.insert(z.end(), 3);
        int want[] = {1,2,3};
        assert(want == z);
    }

    /* ----------------------------------------------------------
     * 3.  FINAL REPORT
     * ---------------------------------------------------------- */
    std::cout << "All xl::list tests passed — high five! ✋\n";
    return 0;
}

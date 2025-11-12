#include "list.hpp"
#include <list>
#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

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
        assert(((int[]){1,2,3} == z));
    }

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
    {
      static int cnt;
      struct ThrowOnCopy {
          int val{};
          ThrowOnCopy() = default;
          ThrowOnCopy(int v):val(v){}
          ThrowOnCopy(const ThrowOnCopy&){
              if(++cnt == 2) throw std::bad_alloc();
          }
      };

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
    {
        struct MoveOnly {
            std::unique_ptr<int> p;
            explicit MoveOnly(int v):p(std::make_unique<int>(v)){}
            MoveOnly(const MoveOnly&) = delete;
            MoveOnly& operator=(const MoveOnly&) = delete;
            MoveOnly(MoveOnly&&) = default;
            MoveOnly& operator=(MoveOnly&&) = default;
            int value() const { return *p; }
        };

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
        assert(((int[]){1,2,3} == z));
    }

    /* ----------  40 extra focused tests start here  ---------- */

    // 2.15  reverse on singleton
    { xl::list<int> z = {42}; z.reverse(); assert(z.front()==42); }

    // 2.16  sort already sorted list
    { xl::list<int> z = {1,2,3,4}; z.sort(); assert(std::is_sorted(z.begin(),z.end())); }

    // 2.17  sort reverse-sorted list
    { xl::list<int> z = {4,3,2,1}; z.sort(); assert(std::is_sorted(z.begin(),z.end())); }

    // 2.18  sort with duplicates
    { xl::list<int> z = {3,1,3,2,1}; z.sort(); assert(((int[]){1,1,2,3,3}==z)); }

    // 2.19  merge two empty lists
    { xl::list<int> a,b; a.merge(b); assert(a.empty()&&b.empty()); }

    // 2.20  merge into empty list
    { xl::list<int> a,b={1,2}; a.merge(b); assert((((int[]){1,2}==a)&&b.empty())); }

    // 2.21  merge from empty list
    { xl::list<int> a={1,2},b; a.merge(b); assert((((int[]){1,2}==a)&&b.empty())); }

    // 2.22  unique on single element
    { xl::list<int> z = {7}; z.unique(); assert(z.size()==1&&z.front()==7); }

    // 2.23  unique with no adjacent dups
    { xl::list<int> z = {1,2,3,4}; z.unique(); assert(z.size()==4); }

    // 2.24  unique removes all dups
    { xl::list<int> z = {2,2,2,2}; z.unique(); assert(z.size()==1&&z.front()==2); }

    // 2.25  remove_if removes nothing
    { xl::list<int> z = {1,2,3}; z.remove_if([](int){return false;}); assert(z.size()==3); }

    // 2.26  remove non-existent value
    { xl::list<int> z = {1,2,3}; z.remove(42); assert(z.size()==3); }

    // 2.27  swap with self (must work)
    { xl::list<int> z = {1,2,3}; z.swap(z); assert(((int[]){1,2,3}==z)); }

    // 2.28  assign iterator range to empty list
    { std::vector<int> v = {9,8,7}; xl::list<int> z; z.assign(v.begin(),v.end()); assert(((int[]){9,8,7}==z)); }

    // 2.29  assign count=0 keeps list empty
    { xl::list<int> z = {1,2}; z.assign(0,5); assert(z.empty()); }

    // 2.30  insert count=0 does nothing
    { xl::list<int> z = {1,2}; z.insert(z.begin(),0,99); assert(z.size()==2); }

    // 2.31  insert range of length 1
    { int a[]={42}; xl::list<int> z = {1,2}; z.insert(z.begin(),a,a+1); assert(z.front()==42&&z.size()==3); }

    // 2.32  erase single returns next
    { xl::list<int> z = {1,2,3}; auto it=z.erase(std::next(z.begin())); assert(*it==3&&z.size()==2); }

    // 2.33  erase range removes all
    { xl::list<int> z = {1,2,3}; z.erase(z.begin(),z.end()); assert(z.empty()); }

    // 2.34  splice single to self (different position)
    { xl::list<int> z = {1,2,3,4}; auto it=std::next(z.begin(),2); z.splice(z.begin(),z,it); assert(((int[]){3,1,2,4}==z)); }

    // 2.35  splice range inside same list
    { xl::list<int> z = {1,2,3,4,5}; auto f=std::next(z.begin()), l=std::next(z.begin(),3); z.splice(z.end(),z,f,l); assert(((int[]){1,4,5,2,3}==z)); }

    // 2.36  front/back on 2-element list
    { xl::list<int> z = {8,9}; assert(z.front()==8&&z.back()==9); }

    // 2.37  pop_back on 1-element list
    { xl::list<int> z = {99}; z.pop_back(); assert(z.empty()); }

    // 2.38  pop_front on 1-element list
    { xl::list<int> z = {77}; z.pop_front(); assert(z.empty()); }

    // 2.39  iterator post-increment
    { xl::list<int> z = {1,2}; auto it=z.begin(); assert(*it++==1&&*it==2); }

    // 2.40  iterator pre-decrement
    { xl::list<int> z = {1,2}; auto it=z.end(); assert(*--it==2); }

    // 2.41  const iterator equality
    { const xl::list<int> z = {1,2}; assert(z.begin()==z.cbegin()); }

    // 2.42  resize shrink
    { xl::list<int> z = {1,2,3,4}; z.resize(2); assert(((int[]){1,2}==z)); }

    // 2.43  resize grow with value
    { xl::list<int> z = {1}; z.resize(3,42); assert(((int[]){1,42,42}==z)); }

    // 2.44  resize grow default
    { xl::list<int> z; z.resize(2); assert(z.size()==2&&z.front()==0&&z.back()==0); }

    // 2.45  emplace front
    { xl::list<std::pair<int,char>> z; z.emplace_front(1,'a'); assert(z.front().first==1&&z.front().second=='a'); }

    // 2.46  emplace back
    { xl::list<std::pair<int,char>> z; z.emplace_back(2,'b'); assert(z.back().first==2&&z.back().second=='b'); }

    // 2.47  clear on empty list
    { xl::list<int> z; z.clear(); z.clear(); assert(z.empty()); }

    // 2.48  splice empty range does nothing
    { xl::list<int> a = {1,2}, b = {3,4}; auto f=b.begin(), l=b.begin(); a.splice(a.begin(),b,f,l); assert(a.size()==2&&b.size()==2); }

    // 2.49  merge with custom comparator descending
    { xl::list<int> a = {5,3,1}, b = {6,4,2}; a.sort(std::greater<int>()); b.sort(std::greater<int>()); a.merge(b,std::greater<int>()); assert(((int[]){6,5,4,3,2,1}==a)); }

    // 2.50  list of pointers maintains order
    { int x=1,y=2,z=3; xl::list<int*> lst; lst.push_back(&x); lst.push_back(&y); lst.push_back(&z); assert(*lst.front()==1&&*lst.back()==3); }

    // 2.51  splice entire list into middle
    { xl::list<int> a = {1,2,3}, b = {4,5}; auto it=std::next(a.begin()); a.splice(it,b); assert(((int[]){1,4,5,2,3}==a&&b.empty())); }

    // 2.52  self-merge (must be no-op)
    { xl::list<int> a = {1,2}; a.sort(); a.merge(a); assert(((int[]){1,2}==a)); }

    // 2.53  unique with custom binary predicate
    { xl::list<int> z = {1,1,2,3,3}; z.unique([](int a,int b){return a==b;}); assert(((int[]){1,2,3}==z)); }

    // 2.54  remove_if with capture
    { int threshold=2; xl::list<int> z = {1,2,3,4}; z.remove_if([&](int v){return v>threshold;}); assert(((int[]){1,2}==z)); }

    // 2.55  reverse empty list
    { xl::list<int> z; z.reverse(); assert(z.empty()); }

    // 2.56  sort stable (Person by id, keep name order)
    { xl::list<Person> p = {{2,"B"},{1,"A"},{2,"C"}}; p.sort(); assert((p.front().name=="A"&&p.back().name=="C")); }

    // 2.57  iterator difference type
    { xl::list<int> z = {1,2,3,4,5}; assert(std::distance(z.begin(),z.end())==5); }

    // 2.58  const reverse iterator
    { const xl::list<int> z = {1,2,3}; assert(*z.rbegin()==3); }

    // 2.59  reverse iterator arithmetic
    { xl::list<int> z = {1,2,3}; auto it=z.rbegin(); ++it; assert(*it==2); }

    // 2.60  shrink via pop until empty
    { xl::list<int> z = {1,2,3}; while(!z.empty()) z.pop_back(); assert(z.empty()); }

    /* ----------------------------------------------------------
     * 3.  FINAL REPORT
     * ---------------------------------------------------------- */
    std::cout << "All xl::list tests passed — high five! ✋\n";
    return 0;
}

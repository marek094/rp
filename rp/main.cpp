//
//  main.cpp
//  rp
//

#ifndef DEBUG_
#define DEBUG_
#endif

#include "simple_avoiders.hpp"
#include <iostream>
#include <utility>
#include <chrono>
#include <iomanip>
#include <map>
#include <memory>
#include <unordered_map>

using namespace std;
using namespace rp;

void test_avoiders();
void test_up_down_perm();
void test_count_hits();
void test_bounded_set();


int main(int argc, const char * argv[]) {

#ifdef DEBUG_
    cerr << "Running tests... " << endl;
    test_avoiders();
    test_up_down_perm();
    test_count_hits();
    test_bounded_set();
    cerr << "...tests completed.\n" << endl;
#endif
    
    // testing build_avoiders
    /*
    bounded_set patterns{"132", "21"};
    
    set res = build_avoiders(patterns, 4);
    for (const perm &p : res) {
        //if (p.size() < 4) continue;
        cout << p << endl;
    }
    cout << "count: " << res.size() << endl;
     
    //*/
    
    return 0;
}

template <class T>
inline void run_test(T expected, std::function<T()> f) {
    static int tid = 0; ++tid;
    T value = f();
    if (value == expected) return;
    
    cerr << "Test #" << tid << " (WA) "
    << "expected: " << expected << ",\tgiven " << value << endl;
}

void test_avoiders() {
    
    // tuple< > represents relation of a function:
    // - the first few of parameters is input
    // - the last parametr is _expected_ output.
    using test = tuple< bounded_set, size_t, size_t>;
    vector< test> tests { // TODO: test inputs
        test{ {"12"}, 1, 1},
        test{ {"12"}, 2, 2},
        test{ {"132"}, 4, 1+2+5+14},
    };
    
    for (auto&& t: tests) {
        run_test< size_t>(get<2>(t), [&t](){
            return build_avoiders(get<0>(t), get<1>(t)).size();
        });
    }
    
}

void test_bounded_set() {

    using test = tuple< bounded_set, size_t>;
    vector< test> tests;
    
    set ss{"123", "132"};
    bounded_set s = std::move(ss);
    
    tests.emplace_back(s, 3);
    
    vector< perm> a{"aa", "bbbbb", "ccc"};
    bounded_set s2{a.begin(), a.end()};
    
    tests.emplace_back(s2, 5);
    
    for (auto&& t: tests) {
        run_test< size_t>(get<1>(t), [&t](){ return get<0>(t).bound();} );
    }
}

void test_up_down_perm() {
    
    using test = tuple< perm, int, perm>;
    vector< test> up_tests{
        test{"12345", 5, "123456"},
        test{"25341", 4, "253461"},
    };
    
    for (auto&& t : up_tests) {
        run_test< perm>(get<2>(t), [&t](){
            return perm_up(get<0>(t), get<1>(t)); });
    }
    
    vector< test> down_tests {
        test{"12345", 0, "1234"},
        test{"13542", 0, "1243"},
        test{"25431", 2, "2431"},
    };
    
    for (auto&& t : down_tests) {
        run_test< perm>(get<2>(t), [&t](){
            return perm_down(get<0>(t), get<1>(t));
        });
    }
}

void test_count_hits() {
    
    using test = tuple< perm, bounded_set, size_t>;
    vector< test> tests{
        test{"1432", {"12"}, 3},
        test{"12345", {"12345"}, 1},
        test{"54231", {"1"}, 5},
        test{"12345", {"12"}, 10},
        test{"15432", {"132"}, 6},
    };
    
    for (auto&& t : tests) {
        run_test< size_t>(get<2>(t), [&t](){
            hits_table ht;
            return count_hits(get<0>(t), get<1>(t), ht)[0];;
        });
    }
}

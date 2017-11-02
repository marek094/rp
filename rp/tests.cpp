
#include "bitwise/avoiders.hpp"
#include "bitwise/avoiders_dfs.hpp"
#include "bitwise/permutation_set.hpp"
#include "bitwise/permutation.hpp"
#include "simple/simple_avoiders.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>

// void run_tests( istream, size_t);
// - function used testing on known results
//      - https://math.depaul.edu/bridget/patterns.html
// - size_t len - max length of pattern
// - formated istream:
//      file of pairs of lines
//      - the first: permutation patterns
//      - the second: known sequence begining from 1
//      - example:
//      - TODO: example
inline bool run_tests_simple( std::istream& is, std::size_t len) {
    bool all_tests_passed = true;
    for (std::string pattern_line, seq_line;
         getline(is, pattern_line) && getline(is, seq_line); ) {
        std::istringstream pss{pattern_line}, sss{seq_line};
        rp_simple::set patterns(
                                std::istream_iterator< rp_simple::perm>{pss},
                                std::istream_iterator< rp_simple::perm>{}
                                );
        std::vector< size_t> seq(
                                 std::istream_iterator< size_t>{sss},
                                 std::istream_iterator< size_t>{}
                                 );
        
        std::vector< size_t> result_seq(len+1, 0);
        auto result = rp_simple::build_avoiders(patterns, len);
        for (auto&&p : result) {
            result_seq[ p.size() ] ++;
        }
        bool test_passed = true;
        for (int i=1; i<result_seq.size() && i<seq.size(); i++) {
            std::cout << i << "\t";
            if (result_seq[i] != seq[i]) {
                all_tests_passed = test_passed = false;
                std::cout << "NO\t" << result_seq[i] << " ";
            } else {
                std::cout << "OK\t";
            }
            std::cout << seq[i] << std::endl;
        }
        if (test_passed) {
            std::cout << "Test OK" << std::endl;
        }
    }
    return all_tests_passed;
}

template<unsigned LETTER, unsigned SIZE, class Func>
inline bool run_tests_bitwise( std::istream& is, Func&& avoiders_func) {
    bool all_tests_passed = true;
    for (std::string pattern_line, seq_line;
         getline(is, pattern_line) && getline(is, seq_line); ) {
        std::istringstream pss{pattern_line}, sss{seq_line};
        
        rp::PermutationSet<LETTER, SIZE> patterns;
        for (std::string pattern_string; pss >> pattern_string; ) {
            std::cout << pattern_string << std::endl;
            for (char& c: pattern_string) c -= '1';
            patterns.insert(
                            rp::Permutation<LETTER, SIZE>{ pattern_string.begin(), pattern_string.end()},
                            pattern_string.size()
                            );
        }
        //        std::cout << pattern_line << std::endl;
        //        for (auto&& p : patterns.getTable()) {
        //            std::cout << p << std::endl;
        //        }
        
        std::vector< size_t> seq(
                                 std::istream_iterator< size_t>{sss},
                                 std::istream_iterator< size_t>{} );
        bool test_passed = true;
        auto result = avoiders_func(patterns);
        for (int i=1; i<result.size() && i<seq.size(); i++) {
            std::cout << i << "\t";
            if (result[i] != seq[i]) {
                all_tests_passed = test_passed = false;
                std::cout << "NO\t" << result[i] << " ";
            } else {
                std::cout << "OK\t";
            }
            std::cout << seq[i] << std::endl;
        }
        if (test_passed) {
            std::cout << "Test OK" << std::endl;
        }
        //        break;
    }
    return all_tests_passed;
}


template <class T> void print(T&& p) {
    using namespace std;
//    cout << hex << p.data[3] << " ";
    cout << hex << p.getWord(2) << " ";
    cout << hex << p.getWord(1) << " ";
    cout << hex << p.getWord(0) << endl;
}

void test_permutation() {
    
    using namespace rp;
    using namespace std;
    
    Permutation<4> p;
    for (int i=0; i<14; ++i) {
        p.up(i/2,i+1);
        print(p);
    }
    //    p.up(0, 0xa);
    //    print(p);
    //    p.up(1, 3);
    
    //    for (int i=0; i<20; ++i) {
    cout << p << endl;
    p.down(0);
    cout << p << endl;
    print(p);
    p.down(3);
    cout << p << endl;
    
    p.down(8);
    cout << p << endl;
    
    cout << Permutation<8, 10>::WORDS << endl;
    cout << Permutation<8, 10>::LETTERS_PER_WORD << endl;
//    return 0;
}

void test_permutation_set() {
    using namespace rp;
    PermutationSet<4> s;
}

void test_permutation_swap() {
    std::vector<unsigned> vec{1,0};
    rp::Permutation<8> p( vec.begin(), vec.end() );
    
    std::cout << p << std::endl;
    p.swapNext(0);
//    p.swapNext(1);
    std::cout << p << std::endl;
}

int main(int argc, char *argv[]) {
//    
//    test_permutation_swap();
//    return 0;
    
    if (argc < 2) {
        std::cerr << "Help: ./tests tests_file_path" << std::endl;
        return 0;
    }
    std::ifstream ifs{argv[1]};
//    int length = atoi(argv[2]);
    std::cerr << "file " << argv[1] /**< "\tmax_len " << length*/ << std::endl;
//    bool passed = run_tests_simple(ifs, 10);
    bool passed = run_tests_bitwise<4,10>(ifs, [](auto&& p){ return rp::buildAvoiders(p); });
    std::cout << passed << std::endl;
    return 0;
}

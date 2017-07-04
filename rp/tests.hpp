#ifndef tests_hpp
#define tests_hpp

#include "bitwise/avoiders.hpp"
#include <iostream>
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

template<unsigned LETTER, unsigned SIZE>
inline bool run_tests_bitwise( std::istream& is) {
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
        auto result = rp::buildAvoiders(patterns);
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
    }
    return all_tests_passed;
}


#endif /* tests_hpp */

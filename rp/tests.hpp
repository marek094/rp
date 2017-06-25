#ifndef tests_hpp
#define tests_hpp

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
inline bool run_tests( std::istream& is, std::size_t len) {
    bool all_tests_passed = true;
    for (std::string pattern_line, seq_line;
         getline(is, pattern_line) && getline(is, seq_line); ) {
        std::istringstream pss{pattern_line}, sss{seq_line};
        rp::set patterns(
                         std::istream_iterator< rp::perm>{pss},
                         std::istream_iterator< rp::perm>{}
                         );
        std::vector< size_t> seq(
                                 std::istream_iterator< size_t>{sss},
                                 std::istream_iterator< size_t>{}
                                 );
        
        std::vector< size_t> result_seq(len+1, 0);
        auto result = rp::build_avoiders(patterns, len);
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

#endif /* tests_hpp */

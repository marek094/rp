
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


// compare two arrays of elements
template <class ListA, class ListB, bool Verbose = true>
bool check_result(const ListA &result_seq, const ListB &seq) {
    bool test_passed = true;
    for (int i=1; i<result_seq.size() && i<seq.size(); i++) {
        if (Verbose) std::cout << i << "\t";
        if (result_seq[i] != seq[i]) {
            test_passed = false;
            if (Verbose) std::cout << "NO\t" << result_seq[i] << " ";
        } else {
            if (Verbose) std::cout << "OK\t";
        }
        if (Verbose) std::cout << seq[i] << std::endl;
    }
    if (test_passed) {
        if (Verbose) std::cout << "Test OK" << std::endl;
    }
    return test_passed;
}

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
        rp::simple::set patterns(
                                 std::istream_iterator< rp::simple::perm>{pss},
                                 std::istream_iterator< rp::simple::perm>{}
                                );
        std::vector< size_t> seq(
                                 std::istream_iterator< size_t>{sss},
                                 std::istream_iterator< size_t>{}
                                 );
        
        std::vector< size_t> result_seq(len+1, 0);
        auto result = rp::simple::build_avoiders(patterns, len);
        for (auto&&p : result) {
            result_seq[ p.size() ] ++;
        }
        all_tests_passed = check_result(result_seq, seq) && all_tests_passed;
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
        auto result = avoiders_func(patterns);
        all_tests_passed = check_result(result, seq) && all_tests_passed;
    }
    return all_tests_passed;
}

enum class Version {
    SIMPLE, BITWISE, BITWISE_DFS
};

int main(int argc, char *argv[]) {
    
    constexpr auto SIZE = 9;
    constexpr auto VERSION = Version::SIMPLE;
    
    if (argc < 2) {
        std::cerr << "Help: ./tests tests_file_path" << std::endl;
        return 0;
    }
    
    std::ifstream ifs{argv[1]};
    bool passed = true;
    switch (VERSION) {
        case Version::SIMPLE: {
            passed = run_tests_simple(ifs, SIZE-1);
            break;
        } case Version::BITWISE: {
            passed = run_tests_bitwise<4, SIZE+1>(ifs, [](auto&& p){
                return rp::buildAvoiders(p); });
            break;
        } case Version::BITWISE_DFS: {
            passed = run_tests_bitwise<4, SIZE+1>(ifs, [](auto&& p){
                return rp::buildAvoidersDfs(p); });
            break;
        }
    }
    std::cout << passed << std::endl;
    return  (int)(!passed);
}









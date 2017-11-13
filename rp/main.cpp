//
//  main.cpp
//  rp
//

#include "bitwise/avoiders.hpp"
#include "bitwise/permutation_set.hpp"
#include "bitwise/permutation.hpp"
#include <string>
#include <iostream>
#include <vector>

constexpr auto LETTER = 8;
constexpr auto SIZE = 14;

int main(int argc, const char * argv[]) {
    
    std::vector<std::string> args{argv + 1, argv + argc};
    bool verbose = args.empty() || args.front() != "-q";
    
    rp::PermutationSet<LETTER, SIZE> patterns;
    for (std::string pattern_string; std::cin >> pattern_string; ) {
        for (char& c: pattern_string) c -= '1';
        rp::Permutation<LETTER, SIZE> perm{ pattern_string.begin(), pattern_string.end()};
        patterns.insert(perm, perm.size());
    }
    
    if (verbose) {
        std::cout << "Pattern set: { ";
        for (auto&& p : patterns.getTable()) std::cout << p << " ";
        std::cout << "}" << std::endl;
    }
    
    try {
        auto ppa_result = rp::buildAvoiders(patterns);
        
        if (verbose) std::cout << "Avoiding sequence: ";
        for (unsigned cnt : ppa_result) std::cout << cnt << " ";
        std::cout << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cout << "Not enought memory" << e.what() << std::endl;
    }
    
    return 0;
}

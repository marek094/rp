//
//  main.cpp
//  rp
//

#include "bitwise/avoiders.hpp"
#include "bitwise/permutation_set.hpp"
#include "bitwise/permutation.hpp"
#include <string>
#include <iostream>

int main(int argc, const char * argv[]) {
    
    constexpr auto LETTER = 4;
    constexpr auto SIZE = 11;
    
    rp::PermutationSet<LETTER, SIZE> patterns;
    for (std::string pattern_string; std::cin >> pattern_string; ) {
        for (char& c: pattern_string) c -= '1';
        rp::Permutation<LETTER, SIZE> perm{ pattern_string.begin(), pattern_string.end()};
        patterns.insert(perm, perm.size());
    }
    
    std::cout << "Pattern set: { ";
    for (auto&& p : patterns.getTable()) std::cout << p << " ";
    std::cout << "}" << std::endl;
    
    auto ppa_result = rp::buildAvoiders(patterns);
    
    std::cout << "Avoiding sequence: ";
    for (unsigned cnt : ppa_result) std::cout << cnt << " ";
    std::cout << std::endl;

    return 0;
}






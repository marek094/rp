#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../bitwise/permutation.hpp"
#include "../bitwise/avoiders_dfs.hpp"

using namespace rp;
using Perm = rp::Permutation<4>;

TEST_CASE( "Construct from hex-literal", "[Permutation<>]") {
    int arr[] = {0,1,5,3,2,4};
    REQUIRE( Perm{arr, arr+6} == Perm{0x015324} );
}

//TEST_CASE( "Test of function", "[isAvoider]") {
//    rp::PermutationSet<4> ps;
//    ps.insert(Perm{0x120});
//
//    REQUIRE( rp::isAvoider(Perm{0x015324}, ps) == true);
//    REQUIRE( rp::isAvoider(Perm{0x035124}, ps) == false);
//
//}

#include "../bitwise/avoiders.hpp"
TEST_CASE( "Sort permutations", "[]") {
    
    using PermW = Permutation<8,8>;
    
    std::vector<int> vals = {0x0, 0x021, 0x120, 0x102, 0x210, 0x201};
    std::vector<PermW> vi, vi2;
    for (auto p : vals) {
        vi.push_back(PermW(p));
        vi2.push_back(PermW(p));
    }
    
    std::sort(vi.begin(), vi.end());
    
    std::cout << "{ ";
    for (auto p : vi2) std::cout << p << " ";
    std::cout << " }" << std::endl;
    
    auto vi3 = vi2;
    sort64b(vi2.begin(), vi2.end(), vi3.begin(), [](auto&& p) {
        return p.getWord(0);
    });
    
    std::cout << "{ ";
    for (auto p : vi) std::cout << p << " ";
    std::cout << " }" << std::endl;
    
    
    REQUIRE (vi == vi2);
    
}


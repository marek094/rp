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

TEST_CASE( "Test of function", "[isAvoider]") {
    rp::PermutationSet<4> ps;
    ps.insert(Perm{0x120});
    
    REQUIRE( rp::isAvoider(Perm{0x015324}, ps) == true);
    REQUIRE( rp::isAvoider(Perm{0x035124}, ps) == false);
    
}


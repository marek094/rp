//
//  altperm.cpp
//  rp
//
//  Created by Marek Černý on 08/11/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#include <iostream>
#include "lib/permutation.hpp"

int main() {
    using namespace rp;
    using namespace std;
    using Perm = Permutation<4>;
    Perm a;
    a.up(7);
    std::cout << a << std::endl;
//    a.dump(std::cout);
}

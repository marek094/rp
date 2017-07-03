
#include "bitwise/avoiders.hpp"
#include "bitwise/permutation_set.hpp"
#include "bitwise/permutation.hpp"
#include "simple/simple_avoiders.hpp"
#include "tests.hpp"
#include <iostream>
#include <iomanip>

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
    bool passed = run_tests_bitwise<4,10>(ifs);
    std::cout << passed << std::endl;
    return 0;
}

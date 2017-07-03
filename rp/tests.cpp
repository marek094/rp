
#include "avoiders.hpp"
#include "permutation_set.hpp"
#include "simple_avoiders.hpp"
#include "permutation.hpp"
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


int main(int argc, char *argv[]) {

    test_permutation_set();
    
    if (argc < 3) {
        std::cerr << "Help: ./tests tests_file_path max_permutaiton_length" << std::endl;
        return 0;
    }
    std::ifstream ifs{argv[1]};
    int length = atoi(argv[2]);
    std::cerr << "file " << argv[1] << "\tmax_len " << length << std::endl;
//    bool passed = run_tests_simple(ifs, length);
    bool passed = run_tests_bitwise<4,13>(ifs);
    std::cout << passed << std::endl;
    return 0;
}

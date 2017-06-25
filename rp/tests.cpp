
#include "simple_avoiders.hpp"
#include "tests.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Help: ./tests tests_file_path max_permutaiton_length" << std::endl;
        return 0;
    }
    std::ifstream ifs{argv[1]};
    int length = atoi(argv[2]);
    std::cerr << "file " << argv[1] << "\tmax_len " << length << std::endl;
    bool passed = run_tests(ifs, length);
    std::cout << passed << std::endl;
    return 0;
}

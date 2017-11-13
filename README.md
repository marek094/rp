## Counting Pattern Avoiders

Software Project based on the article 'Fast Algorithms for Finding Pattern Avoiders and Counting Pattern Occurrences in Permutations' [1].

[1] William Kuszmaul, https://arxiv.org/abs/1509.08216 

---------

#### Usage

Makefile is placed in directory [./rp](./rp). 

Program has two main parts: 
1) [tests.cpp](./rp/tests.cpp) — Test of correctness compared to sequences in [data.txt](./test/data.txt); build and run with `make tests && make tt`.


2) [main.cpp](./rp/main.cpp) — For testing on user defined pattern sets; build with `make main`.  Demo: 

```bash
$ cd ./rp
```
```bash
$ make main
g++ -Wall -pthread -std=c++1z -O3 -march=native    main.cpp   -o main
```
```bash
$ ./main <<EOF
> 1243
> 4231
> EOF
Pattern set: { 3120 0132 }
Avoiding sequence: 0 1 2 6 22 86 335 1266 4598 16016 
```
```bash
$ make clean
find . | sed -e 's/\.cpp//' | sort | uniq -d | tee /dev/stderr | xargs rm
./main
```

#### Permutation MAX_SIZE and LETTER paramaters
Both main.cpp and tests.cpp containst two compile-time parameters;
```c++
constexpr auto LETTER = 4;
constexpr auto SIZE = 10;
```
denoting _maximum size_ of given permutation which container 
```c++
rp::Permutation<LETTER,SIZE> 
```
can holds and _number of bits_ spended on each letter of given permutation. Therfore ```SIZE``` determines the maximum length of sequence generated in previous demo.

#### Requirements
- Project is written in C++14; few last commits require C++17 standard.
- Tested on **clang/macOS**, **clang/Linux** and **gcc 6.3/Linux**

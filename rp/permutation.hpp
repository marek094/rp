//
//  bitwise_avoiders.hpp
//  rp
//
//  Created by Marek Černý on 26/06/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#ifndef bitwise_avoiders_hpp
#define bitwise_avoiders_hpp

#include <cassert>
#include <iostream>

namespace rp {

    using uchar = unsigned char;
    using ull = unsigned long long;
    
    // constexpr int divUp(int a, int b);
    // helper function for setting data array sizes in Permutation<>
    constexpr int divUp(int a, int b) {
        return (a/b + (a%b > 0 ? 1 : 0));
    }
    
    // ull maskOne(unsigned a);
    // helper for computing mask with 'a' ones from begining
    // it is expected to be inlined
    // Example: 4 -> 0x0f
    inline ull maskOne(unsigned a) {
        return (1ull << a) - 1;
    }

    // Class Permutation
    // - LETTER - number of bits representing one letter
    // - MAXSIZE - maximal size of permutation; default is computed from LETTER width
    template <unsigned LETTER, unsigned MAX_SIZE = (1u<<LETTER)>
    class Permutation {
        
        static_assert(sizeof(ull) == 8, "64-bit unsigned long long is required");
        static_assert( ((LETTER - 1) & LETTER) == 0 && LETTER > 0,
                      "LETTER template parameter is required to be power of 2");
        static_assert( (1u<<LETTER) >= MAX_SIZE,  "LETTER is too small to construct MAXSIZE-permutation");
    
    public:
        Permutation() {
            clearData();
        }
        
        // void up(unsigned pos, unsigned size);
        // implementation of UP operation defined in Definition 2.11 exept
        // the 'pos' argument is in range {0..n} (not {1..n+1})
        void up(unsigned pos, unsigned size) {
            assert(0 <= pos);
            assert(pos < MAX_SIZE);
            // position in array data[]
            const unsigned insoff = pos/LETTERS_PER_WORD;
            // position in integer
            const unsigned inspos = (pos%LETTERS_PER_WORD) * LETTER;
            const unsigned last_letter_pos = LETTER * (LETTERS_PER_WORD-1);
            // creating masks for shift
            const ull mask_lower = maskOne(inspos);
            const ull mask_upper = ~mask_lower;
            
            ull& inspart = data[ insoff ];
            // save the last letter
            ull carryletter = inspart >> last_letter_pos;
            // insert "size" letter
            inspart = (inspart & mask_lower) |
                      ( ((ull)size & maskOne(LETTER)) << inspos ) |
                      ((inspart & mask_upper) << LETTER);
            
            // shift other parts
            for (int i = insoff+1; i<WORDS; ++i) {
                const ull new_carryletter = data[i] >> last_letter_pos;
                data[i] = ( data[i] << LETTER ) | carryletter;
                carryletter = new_carryletter;
            }
        }
        
        // void down(unsigned pos);
        // implementation of DOWN operation defined in Definition 2.12 exept
        // the 'pos' argument is in range {0..n-1} (not {1..n})
        void down(unsigned pos) {
            assert(0 <= pos);
            assert(pos < MAX_SIZE);
            // position in array data[]
            const unsigned deloff = pos/LETTERS_PER_WORD;
            // position in integer
            const unsigned delpos = (pos%LETTERS_PER_WORD) * LETTER;
            const unsigned first_letter_pos = LETTER * (LETTERS_PER_WORD-1);
            // create masks for shift
            const ull mask_lower = maskOne(delpos);
            const ull mask_upper = ~maskOne(delpos) << LETTER;
            // backup deleted value
            ull delvalue = (data[ deloff ] >> delpos) & maskOne(LETTER);
            ull& delpart = data[ deloff ];
            delpart = ((delpart & mask_lower) | ((delpart & mask_upper) >> LETTER));
            // shift other parts
            for (int i = deloff+1; i<WORDS; ++i) {
                data[i-1] |= ( maskOne(LETTER) & data[i] ) << first_letter_pos;
                data[i] = data[i] >> LETTER;
            }
            // decrease all greater
            for (int i = 0; i<WORDS; ++i) {
                for (int j = 0; j<LETTERS_PER_WORD; ++j) {
                    // get the right letter
                    const ull letter = (data[i] >> LETTER*j) & maskOne(LETTER);
                    // replace it
                    if (letter >= delvalue) {
                        data[i] ^= (letter ^ (letter-1)) << LETTER*j;
                    }
                }
            }
        }
       
        // access the nth letter
        unsigned operator[](unsigned pos) {
            const unsigned insoff = pos/LETTERS_PER_WORD;
            const unsigned inspos = (pos%LETTERS_PER_WORD) * LETTER;
            return (unsigned)(data[ insoff ] >> inspos) & maskOne(LETTER);
            
        }
    private:
        void clearData() {
            for (int i=0; i<WORDS; i++) {
                data[i] = 0ull;
            }
        }
        
    public:
        static constexpr auto WORD = sizeof(ull) * 8;
        static constexpr auto LETTERS_PER_WORD = divUp( sizeof(ull) * 8, LETTER);
        static constexpr auto WORDS = divUp(MAX_SIZE, LETTERS_PER_WORD);
        static constexpr unsigned getMAXSIZE() {
            return MAX_SIZE;
        }
        
        using ull = unsigned long long;
        ull data[ WORDS ];
    };
    
    // operator for printing permutation
    template <unsigned LETTER, unsigned MAX_SIZE>
    std::ostream& operator<<(std::ostream& os, Permutation<LETTER, MAX_SIZE>& p) {
        for (int i=0; i<MAX_SIZE; ++i) {
            os << p[i] << " ";
        }
        return os;
    }
    
    
}





#endif /* bitwise_avoiders_hpp */

//
//  invariant.hpp
//  rp
//
//  Created by Marek Černý on 01/07/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#ifndef invariant_hpp
#define invariant_hpp

#include <cassert>

namespace rp {

    using uchar = unsigned char;
    using ull = unsigned long long;
    
    // constexpr int divUp(int a, int b);
    // helper function for setting data array sizes in Permutation<>
    constexpr int divUp(int a, int b) {
        return (a/b + (a%b > 0 ? 1 : 0));
    }
    
    // constexpr unsigned defaultMAXSIZE(unsigned letter);
    // helper function used for default letter
    constexpr unsigned defaultMAXSIZE(unsigned letter) {
        return 1u << letter;
    }
    
    // ull maskOne(unsigned a);
    // helper for computing mask with 'a' ones from begining
    // it is expected to be inlined
    // Example: 4 -> 0x0f
    inline constexpr ull maskOne(unsigned a) {
        return (1ull << a) - 1;
    }
    
    // Class Permutation
    // - LETTER - number of bits representing one letter
    // - MAXSIZE - maximal size of permutation; default is computed from LETTER width
    template <unsigned _LETTER, unsigned _MAX_SIZE = defaultMAXSIZE(_LETTER)>
    class Permutation {
    public:
        static constexpr auto MAX_SIZE = _MAX_SIZE;
        static constexpr auto LETTER = _LETTER;
        static constexpr auto WORD = sizeof(ull) * 8;
        static constexpr auto LETTERS_PER_WORD = divUp(WORD, LETTER);
        static constexpr auto WORDS = divUp(MAX_SIZE, LETTERS_PER_WORD);
        static constexpr auto LETTER_MASK = maskOne(LETTER);
        
        static_assert(sizeof(ull) == 8, "64-bit unsigned long long is required");
        static_assert( ((LETTER - 1) & LETTER) == 0 && LETTER > 0,
                      "LETTER template parameter is required to be power of 2");
        static_assert( (1ull<<LETTER) >= MAX_SIZE,  "LETTER is too small to construct MAXSIZE-permutation");
        
        using Self = Permutation<LETTER, MAX_SIZE>;
        
        Permutation() {
            clearData();
        }
        
        void next() {
            ++data[0];
        }
        
        void up(unsigned pos) {
           // assert(pos & LETTER_MASK == pos);
            data[0] <<= LETTER_MASK;
            data[0]  &= pos;
        }
        
        // access the nth letter
        unsigned operator[](unsigned pos) const {
            const unsigned insoff = pos/LETTERS_PER_WORD;
            const unsigned inspos = (pos%LETTERS_PER_WORD) * LETTER;
            return (unsigned)(data[ insoff ] >> inspos) & maskOne(LETTER);
        }
        
        ull getWord(unsigned offset) const {
            return data[ offset ];
        }
        
        void dump(std::ostream &os) const {
            for (int i=0; i<WORDS; i++) {
                os << data[i] << ' ';
            }
            os << std::endl;
        }
        
    private:
        void clearData() {
            for (int i=0; i<WORDS; i++) {
                data[i] = 0ull;
            }
        }
        
        ull data[WORDS];
    };
    
    // operator for printing permutation
    template <unsigned LETTER, unsigned MAX_SIZE>
    std::ostream& operator<<(std::ostream& os, const Permutation<LETTER, MAX_SIZE>& p) {
        for (int i=0; i<MAX_SIZE; ++i) {
            os << p[i] << "";
        }
        return os;
    }

}

#endif /* avoiders_hpp */

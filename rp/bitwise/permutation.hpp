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
#include <vector>

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
    inline ull maskOne(unsigned a) {
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
        
        static_assert(sizeof(ull) == 8, "64-bit unsigned long long is required");
        static_assert( ((LETTER - 1) & LETTER) == 0 && LETTER > 0,
                      "LETTER template parameter is required to be power of 2");
        static_assert( (1ull<<LETTER) >= MAX_SIZE,  "LETTER is too small to construct MAXSIZE-permutation");
    
        using Self = Permutation<LETTER, MAX_SIZE>;

        Permutation() {
            clearData();
        }
        
        template <class Iterator>
        Permutation(Iterator begin, Iterator end) {
            clearData();
            int pos = 0;
            for (Iterator it = begin; it != end; ++it) {
                this->up(pos, *it);
                ++pos;
            }
        }
        
        explicit Permutation(unsigned hex) {
            auto result = std::vector<int>{};
            for (int zerocnt = 0;;) {
                unsigned v = hex % 16;
                if (v == 0) zerocnt++;
                if (zerocnt > 1) break;
                result.push_back(v);
                hex /= 16;
            }
            // backwards
            *this = Self{result.rbegin(), result.rend()};
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
            for (unsigned i = insoff+1; i<WORDS; ++i) {
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
            for (unsigned i = deloff+1; i<WORDS; ++i) {
                data[i-1] |= ( maskOne(LETTER) & data[i] ) << first_letter_pos;
                data[i] = data[i] >> LETTER;
            }
            // decrease all greater
            for (unsigned i = 0; i<WORDS; ++i) {
                for (unsigned j = 0; j<LETTERS_PER_WORD; ++j) {
                    // get the right letter
                    const ull letter = (data[i] >> LETTER*j) & maskOne(LETTER);
                    // replace it
                    if (letter > delvalue) {
                        data[i] ^= (letter ^ (letter-1)) << LETTER*j;
//                        if (letter == delvalue) std::cout << *this << std::endl;
                    }
                }
            }
        }
        
        // unsigned size() const;
        // compute size by number of zeros in permutation
        // ( 0 0 0 0 0 ) -> 1
        // ( 2 0 1 3 0 ) -> 4
        unsigned size() const {
            int result = MAX_SIZE+1;
            for (unsigned i = 0; i < MAX_SIZE; ++i) {
                if (this->operator[](i) == 0) {
                    result--;
                }
            }
            return (unsigned) result;
        }
        
        // void swapNext(unsigned pos);
        // - swaps letterns on positions [pos] and [pos+1]
        // ( 1 0 3 4 ).swapNext(1) -> ( 1 3 0 4)
        void swapNext(unsigned pos) {
            const unsigned insoff = pos/LETTERS_PER_WORD;
            const unsigned inspos = (pos%LETTERS_PER_WORD) * LETTER;
            if (inspos == (LETTERS_PER_WORD-1)*LETTER) {
                assert(insoff < WORDS-1);
                const ull last_letter_pos = (LETTERS_PER_WORD-1)*LETTER;
                
                const ull mask_st = maskOne(LETTER) << last_letter_pos;
                const ull mask_nd = maskOne(LETTER);
                
                const ull st = mask_st & data[ insoff ];
                const ull nd = mask_nd & data[ insoff+1 ];
                
                data[ insoff ] &= ~mask_st;
                data[ insoff+1 ] &= ~mask_nd;
                
                data[ insoff ] |= nd << last_letter_pos;
                data[ insoff+1 ] |= st >> last_letter_pos;
            } else {
                const ull letter_pos = pos*LETTER;
                const ull mask_st = maskOne(LETTER) << letter_pos;
                const ull mask_nd = maskOne(LETTER) << (letter_pos+LETTER);
                
                const ull st = mask_st & data[ insoff ];
                const ull nd = mask_nd & data[ insoff];
                
                data[ insoff ] &= ~(mask_st | mask_nd);
                data[ insoff ] |= (st << LETTER) | (nd >> LETTER);
            }
            
        }
        
        // void walkChildren(Func&& func) const;
        // - similar as high-order std::for_each,
        //   where elements are children
        //   ( 0 1 )'s children are: (201) (021) (012)
        template < class Func >
        void walkChildren(Func&& func) const {
            Permutation child = *this; child.up(0, size());
            func((const Permutation)child);
            for (unsigned i=1; i<size()+1; ++i) {
                child.swapNext(i-1);
                func((const Permutation)child);
            }
        }
        
        template <
            class Ret,
            class Func
        >
        Ret reduceChildren(Ret base, Func&& func) const {
            Permutation child = *this; child.up(0, size());
            Ret result = func((const Permutation)child, base);
            for (unsigned i=1; i<size()+1; ++i) {
                child.swapNext(i-1);
                result = func((const Permutation)child, result);
            }
            return result;
        }
    
        template <
            class Func
        >
        void walkDowns(Func&& func) const {
            for (unsigned i=0; i<size(); i++) {
                Permutation par = *this; par.down(i);
                func((const Permutation)par);
            }
        }
        
        template <
        class Func
        >
        void walkDownsi(Func&& func) const {
            for (unsigned i=0; i<size(); i++) {
                Permutation par = *this; par.down(i);
                func((const Permutation)par, i);
            }
        }
        
        // unsigned operator[](unsigned pos) const;
        // access the nth letter
        unsigned operator[](unsigned pos) const {
            const unsigned insoff = pos/LETTERS_PER_WORD;
            const unsigned inspos = (pos%LETTERS_PER_WORD) * LETTER;
            return (unsigned)(data[ insoff ] >> inspos) & maskOne(LETTER);
        }
        
        bool operator==(const Self& p) const {
            for (unsigned i=0; i<WORDS; ++i) {
                if (data[i] != p.data[i]) {
                    return false;
                }
            }
            return true;
        }
        
        bool operator<(const Self& p) const {
            assert(size() == p.size());
            for (unsigned i=0; i<size(); ++i) {
                if (this->operator[](i) == p[i]) continue;
                else return this->operator[](i) < p[i];
            }
            return false; // eq
        }

        bool operator>(const Self& p) const { return p < *this; }
        bool operator<=(const Self& p) const {return !(*this > p);}
        bool operator>=(const Self& p) const {return !(*this < p);}
        
        ull getWord(unsigned offset) const {
            return data[ offset ];
        }
        
        template <class T>
        T toContainer() const {
            T res;
            for (unsigned i=0; i < size(); ++i) {
                res.push_back(this->operator[](i));
            }
            return res;
        }
        
        // static constexpr Self getIdentity();
        // generate identity permutation
        // in static context
        static constexpr Self getIdentity() {
            Self p;
            for (unsigned i = 0; i < MAX_SIZE; ++i) {
                p.up(i, i);
            }
            return p;
        }
        
    private:
        void clearData() {
            for (unsigned i=0; i<WORDS; i++) {
                data[i] = 0ull;
            }
        }
        
        ull data[ WORDS ];
    };
    
    // operator for printing permutation
    template <unsigned LETTER, unsigned MAX_SIZE>
    std::ostream& operator<<(std::ostream& os, const Permutation<LETTER, MAX_SIZE>& p) {
        for (unsigned i=0; i<p.size(); ++i) {
            os << p[i] << "";
        }
        return os;
    }
    
}





#endif /* bitwise_avoiders_hpp */

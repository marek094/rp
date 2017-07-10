//
//  avoiders.hpp
//  rp
//
//  Created by Marek Černý on 01/07/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#ifndef avoiders_hpp
#define avoiders_hpp

#include "permutation.hpp"
#include "permutation_set.hpp"
#include <cassert>
#include <queue>
#include <array>
#include <utility>
#include <algorithm>

#define DEBUG 0

namespace rp {

    template <typename U>
    inline void setBit(U& u, unsigned pos) {
        u |= U(1) << pos;
    }
    
    template <typename U>
    inline bool getBit(const U u, unsigned pos) {
        return (u & (U(1) << pos)) > 0;
    }
    
    template <class PermutationSet, class Permutation = typename PermutationSet::Permutation>
    std::array<int, Permutation::MAX_SIZE>
    buildAvoiders(const PermutationSet& patterns) {
        
        PermutationSet avoiders, next_avoiders;
        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
        sizes_cnt.fill(0);
        sizes_cnt[1] = 1;
        
        if (Permutation::MAX_SIZE == 0) return sizes_cnt;
        
        if ( !patterns.lookup(Permutation{}) ) {
            avoiders.insert(Permutation{});
        }
        
        if (Permutation::MAX_SIZE == 1) return sizes_cnt;
        
        for (int actual_size = 2; actual_size < Permutation::MAX_SIZE; ++actual_size) {
            for (Permutation perm : avoiders.getTable()) {
                unsigned not_avoiders = 0;
#if DEBUG
                std::cout  << "pop" << actual_size<<"\t" << perm << std::endl;
#endif
                perm.up(0, actual_size-1);
#if DEBUG
                std::cout  << "up\t" << perm << std::endl;
#endif
                for (int j=1; j < std::min((unsigned)actual_size, patterns.getBound()+1); ++j) {
                    Permutation down = perm; down.down(j);
                    unsigned not_avoider = 0;
                    for (int i = 0; i < actual_size; ++i) {
                        if (i!=j && i > 0) {
                            down.swapNext( i-1 - (i>j) );
                        }
                        if ( !avoiders.lookup( down ) ) {
                            setBit(not_avoider, i);
#if DEBUG
                            std::cout << " no" << std::endl;
#endif
                        } else {
#if DEBUG
                            std::cout << " ok" << std::endl;
#endif
                        }
                        
                    }
                    not_avoiders |= not_avoider;
                }
                
                if ( actual_size < patterns.getBound()+1 ) {
                    Permutation p = perm;
                    for (int i = 0; i < actual_size; ++i) {
                        if (i > 0) p.swapNext(i-1);
                        if ( patterns.lookup(p) )
                            setBit(not_avoiders, i);
                    }
                }
                
                if ( actual_size+1 < Permutation::MAX_SIZE ) {
                    for (int i = 0; i < actual_size; ++i) {
                        if (i > 0) perm.swapNext(i-1);
                        if (!getBit(not_avoiders, i) ) {
                            sizes_cnt[actual_size]++;
                            auto ins = next_avoiders.insert(perm);
                            assert(ins.second == true); // inserted new
                        }
                    }
                } else {
                    sizes_cnt[actual_size] += actual_size - __builtin_popcount(not_avoiders);
                }
                
        
            }
//            if (actual_size < 6) {
//                std::cout << "Avoides " << actual_size << std::endl;
//                for (auto&& p : avoiders.getTable()) {
//                    std::cout << p << std::endl;
//                }
//            }
            if (actual_size+1 < Permutation::MAX_SIZE) {
                avoiders.getTable().clear();
                double coeff = (double)sizes_cnt[actual_size] / (double)sizes_cnt[actual_size-1];
                avoiders.getTable().reserve(sizes_cnt[actual_size] * 4 * std::ceil(coeff) );
                std::swap( avoiders, next_avoiders);
            }
        }
        return sizes_cnt;
    }
    
};

#endif /* avoiders_hpp */

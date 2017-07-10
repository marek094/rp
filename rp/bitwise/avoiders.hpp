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
#include <unordered_map>

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
    
    template <typename U>
    inline U copyIthBit(const U u, unsigned pos) {
        const unsigned mask_lower =  ( U(1) << (pos+1) )-1;
        const unsigned mask_upper = ~(( U(1) << pos )-1);
        const unsigned upper = u & mask_upper;
        const unsigned lower = u & mask_lower;
        return lower | (upper << 1);
    }
    
    template <class PermutationSet, class Permutation = typename PermutationSet::Permutation>
    std::array<int, Permutation::MAX_SIZE>
    buildAvoiders(const PermutationSet& patterns) {
        
        std::unordered_map<Permutation, unsigned, typename PermutationSet::Hash> avoiders, next_avoiders;
        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
        sizes_cnt.fill(0);
        sizes_cnt[1] = 1;
        
        if (Permutation::MAX_SIZE == 0) return sizes_cnt;
        
        if ( !patterns.lookup(Permutation{}) ) {
            avoiders.insert({Permutation{},0});
        }
        
        if (Permutation::MAX_SIZE == 1) return sizes_cnt;
        
        for (unsigned actual_size = 2; actual_size < Permutation::MAX_SIZE; ++actual_size) {
            for (auto&& perm_map : avoiders) {
                Permutation perm = perm_map.first;
                unsigned not_avoiders = 0;
#if DEBUG
                std::cout  << "pop" << actual_size<<"\t" << perm << std::endl;
#endif
                perm.up(0, actual_size-1);
#if DEBUG
                std::cout  << "up\t" << perm << std::endl;
#endif
                // before all patterns are significant in avoiders
                if (actual_size < patterns.getBound()+1) {
                    for (int j=1; j < actual_size; ++j) {
                        Permutation down = perm; down.down(j);
                        unsigned not_avoider = 0;
                        for (int i = 0; i < actual_size; ++i) {
                            if (i!=j && i > 0) {
                                down.swapNext( i-1 - (i>j) );
                            }
                            if (avoiders.find(down) == avoiders.end()) {
                                setBit(not_avoider, i);
                            }
                        }
                        not_avoiders |= not_avoider;
                    }
                    Permutation p = perm;
                    for (int i = 0; i < actual_size; ++i) {
                        if (i > 0) {
                            p.swapNext(i-1);
                        }
                        if (patterns.lookup(p)) {
                            setBit(not_avoiders, i);
                        }
                    }
                // if permutations are big enought
                } else {
                    for (int j=1; j < patterns.getBound()+1; ++j) {
                        Permutation down = perm; down.down(j);
                        unsigned not_avoider = (1u<<actual_size)-1;
                        for (int i = 0; i < actual_size; ++i) {
                            if (i!=j && i > 0) down.swapNext( i-1 - (i>j) );
                            auto it = avoiders.find(down);
                            if (it != avoiders.end()) {
                                not_avoider = copyIthBit(it->second, j-1);
                                break;
                            }
                        }
#if DEBUG
                        std::cout << "\t@\t\t";
                        for (int i=0; i < actual_size; ++i) {
                            bool res = getBit(not_avoider, i);
                            std::cout << res << " ";
                        }
                        std::cout << std::endl;
#endif
                        not_avoiders |= not_avoider;
                    }
                }

                if ( actual_size+1 < Permutation::MAX_SIZE ) {
                    for (int i = 0; i < actual_size; ++i) {
                        if (i > 0) perm.swapNext(i-1);
                        if ( !getBit(not_avoiders, i) ) {
                            sizes_cnt[actual_size]++;
                            auto ins = next_avoiders.insert({perm, not_avoiders});
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
                avoiders.clear();
                double coeff = (double)sizes_cnt[actual_size] / (double)sizes_cnt[actual_size-1];
//                double coeff = 1.0;
                avoiders.reserve(sizes_cnt[actual_size] * 4 * std::ceil(coeff) );
                std::swap( avoiders, next_avoiders);
            }
        }
        return sizes_cnt;
    }
    
};

#endif /* avoiders_hpp */

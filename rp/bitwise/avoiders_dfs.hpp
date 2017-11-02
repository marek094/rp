//
//  avoiders.hpp
//  rp
//
//  Created by Marek Černý on 01/07/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#ifndef avoiders_dfs_hpp
#define avoiders_dfs_hpp

#include "permutation.hpp"
#include "permutation_set.hpp"
#include "avoiders.hpp"
#include <cassert>
#include <queue>
#include <array>
#include <utility>
#include <algorithm>
#include <unordered_map>

#define DEBUG 0

namespace rp {

    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation,  unsigned, typename PermutationSet::Hash>
    >
    std::tuple< std::array<int, Permutation::MAX_SIZE>, Map>
    buildAvoidersOfPatternsBoundSize(const PermutationSet& patterns) {

        Map avoiders, next_avoiders;
        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
        sizes_cnt.fill(0);

        if (Permutation::MAX_SIZE == 0) return {sizes_cnt, avoiders};

        sizes_cnt[1] = 1;
        if ( !patterns.lookup(Permutation{}) ) {
            avoiders.insert({Permutation{}, 0u});
        }

        if (Permutation::MAX_SIZE == 1) return {sizes_cnt, avoiders};

        for (unsigned actual_size = 2; actual_size < patterns.getBound()+1; ++actual_size) {
            for (auto&& perm_map : avoiders) {
                Permutation perm = perm_map.first;
                unsigned not_avoiders = 0;
                perm.up(0, actual_size-1);
                // before all patterns are significant in avoiders
                for (int j=1; j < actual_size; ++j) {
                    Permutation down = perm; down.down(j);
                    unsigned not_avoider = 0;
                    for (int i = 0; i < actual_size; ++i) {
                        if (i!=j && i > 0) {
                            down.swapNext( i-1 - (i>j) );
                        }
                        auto it = avoiders.find(down);
                        if (it == avoiders.end()) {
                            setBit(not_avoider, i);
                        }
                    }
                    not_avoiders |= not_avoider;
                }
                for (int i = 0; i < actual_size; ++i) {
                    if (i > 0) perm.swapNext(i-1);
                    if ( !getBit(not_avoiders, i) && !patterns.lookup(perm)) {
                        sizes_cnt[actual_size]++;
                        auto ins = next_avoiders.insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    }
                }
            }

            avoiders.clear();
            // magic experimental constant based on growth rate of function
            double coeff = (double)sizes_cnt[actual_size] / (double)sizes_cnt[actual_size-1];
            avoiders.reserve( sizes_cnt[actual_size] * 5 * std::ceil(coeff) );
            std::swap( avoiders, next_avoiders);
        }
        return {sizes_cnt, avoiders};
    }

    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation, unsigned, typename PermutationSet::Hash>,
        class Sizes = std::array<int, Permutation::MAX_SIZE>
    >
    void buildAvoidersDfsRecursive(const PermutationSet& patterns, Sizes& sizes_cnt,
                                   Map& avoiders, Permutation p, unsigned size) {
//        Map next_avoiders;
        for (int j=0; j < patterns.getBound()+1; ++j) {
            Permutation down = p; down.down(j);
            if (avoiders.find(down) != avoiders.end()) {
                return;
            }
        }

        for (int i=0; i < size; ++i) {
            
        }

    }
    
    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation, unsigned, typename PermutationSet::Hash>
    >
    std::array<int, Permutation::MAX_SIZE>
    buildAvoidersDfs(const PermutationSet& patterns) {

        auto small_avoiders = buildAvoidersOfPatternsBoundSize(patterns);
        Map& avoiders = std::get<Map>(small_avoiders);
        auto&& sizes_cnt = std::get<std::array<int, Permutation::MAX_SIZE>>(small_avoiders);
//        std::cout << sizes_cnt[0] << std::endl;
        
        
        for (auto&& avoider : avoiders) {
            auto p = std::get<0>(avoider);
            buildAvoidersDfsRecursive(patterns, sizes_cnt, avoiders, p, patterns.getBound());
        }
        return sizes_cnt;
    }

};

#endif /* avoiders_hpp */

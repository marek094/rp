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

namespace rp {
    
    template <
        class Avoiders,
        class Permutation
    >
    unsigned getParcExtMap(const Avoiders& avoiders, Permutation p, unsigned j) {
        unsigned extmap = 0u;
        auto size = p.size();
        p.up(0, size);
        for (unsigned i=0; i < size+1; ++i) {
            if (i>0) p.swapNext(i-1);
            Permutation downed = p; downed.down(j+1);
            if (avoiders.find(p) != avoiders.end()) {
                extmap |= 1u << i;
            }
        }
        return extmap;
    }
    
    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation,  unsigned, typename PermutationSet::Hash>
    >
    std::array<Map, Permutation::MAX_SIZE>
    buildAvoidersOfPatternsBoundSize(const PermutationSet& patterns) {
        std::array<Map, Permutation::MAX_SIZE> avoiders;
        if (Permutation::MAX_SIZE == 0) return avoiders;

        if ( !patterns.lookup(Permutation{}) ) {
            avoiders[1].insert({Permutation{}, 0u});
        }

        if (Permutation::MAX_SIZE == 1) return avoiders;

        for (unsigned actual_size = 2; actual_size < Permutation::MAX_SIZE-1; ++actual_size) {
            for (auto&& perm_map : avoiders[actual_size-1]) {
                Permutation perm = perm_map.first;
                unsigned not_avoiders = 0;
                
                perm.up(0, actual_size-1);
//                std::cout << "# " << perm << std::endl;
                for (unsigned j=1; j < actual_size; ++j) {
                    Permutation down = perm; down.down(j);
                    unsigned not_avoider = 0;
                    for (unsigned i = 0; i < actual_size; ++i) {
                        if (i!=j && i > 0) {
                            down.swapNext( i-1 - (i>j) );
                        }
                        auto it = avoiders[actual_size-1].find(down);
                        if (it == avoiders[actual_size-1].end()) {
                            setBit(not_avoider, i);
                        }
                    }
                    not_avoiders |= not_avoider;
                }
                for (unsigned i = 0; i < actual_size; ++i) {
                    if (i > 0) perm.swapNext(i-1);
                    if ( !getBit(not_avoiders, i) && !patterns.lookup(perm)) {
                        auto ins = avoiders[actual_size].insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    }
                }
            }
        }
        return avoiders;
    }

    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation, unsigned, typename PermutationSet::Hash>
    >
    std::array<unsigned, Permutation::MAX_SIZE>
    buildAvoidersDfs(const PermutationSet& patterns) {
        
        auto avoiders = buildAvoidersOfPatternsBoundSize(patterns);

        std::array<unsigned, Permutation::MAX_SIZE> result;
        for (unsigned i=0; i < Permutation::MAX_SIZE; ++i) {
            result[i] = avoiders[i].size();
        }
        
        for (auto&& pmap : avoiders[ Permutation::MAX_SIZE-2 ]) {
            result.back() += __builtin_popcount(pmap.second);
        }
        
        return result;
    }

};

#endif /* avoiders_hpp */

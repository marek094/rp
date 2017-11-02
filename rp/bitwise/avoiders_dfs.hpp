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

#define DEBUG 1

namespace rp {

    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation,  unsigned, typename PermutationSet::Hash>
    >
    std::tuple< std::array<int, Permutation::MAX_SIZE>, std::array<Map, Permutation::MAX_SIZE>>
    buildAvoidersOfPatternsBoundSize(const PermutationSet& patterns) {

        std::array<Map, Permutation::MAX_SIZE> avoiders;
        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
        sizes_cnt.fill(0);

        if (Permutation::MAX_SIZE == 0) return {sizes_cnt, avoiders};

        sizes_cnt[1] = 1;
        if ( !patterns.lookup(Permutation{}) ) {
            avoiders[1].insert({Permutation{}, 0u});
        }

        if (Permutation::MAX_SIZE == 1) return {sizes_cnt, avoiders};

        for (unsigned actual_size = 2; actual_size < patterns.getBound()+1; ++actual_size) {
            for (auto&& perm_map : avoiders[actual_size-1]) {
                Permutation perm = perm_map.first;
                unsigned not_avoiders = 0;
//                std::cout << perm << " - " << actual_size-1 << " - k";
                perm.up(0, actual_size-1);
//                std::cout << " " << perm;
//                std::cout << std::endl;
                // before all patterns are significant in avoiders
                for (int j=1; j < actual_size; ++j) {
                    Permutation down = perm; down.down(j);
                    unsigned not_avoider = 0;
                    for (int i = 0; i < actual_size; ++i) {
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
                for (int i = 0; i < actual_size; ++i) {
                    if (i > 0) perm.swapNext(i-1);
                    if ( !getBit(not_avoiders, i) && !patterns.lookup(perm)) {
                        sizes_cnt[actual_size]++;
                        auto ins = avoiders[actual_size].insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    }
                }
            }

//            avoiders[actual_size].clear();
            // magic experimental constant based on growth rate of function
//            double coeff = (double)sizes_cnt[actual_size] / (double)sizes_cnt[actual_size-1];
//            avoiders[actual_size].reserve( sizes_cnt[actual_size] * 5 * std::ceil(coeff) );
//            std::swap( avoiders, avoiders[actual_size]);
        }
        return {sizes_cnt, avoiders};
    }
    
    template <
        class Permutation,
        class Func
    >
    void walkChildren(const Permutation &perm, unsigned size, Func&& func) {
        for (int i=0; i<size+1; ++i) {
            Permutation child = perm; child.up(i, size);
            func(child);
        }
    }

    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Maps,
        class Sizes = std::array<int, Permutation::MAX_SIZE>
    >
    void buildAvoidersDfsRecursive(const PermutationSet& patterns,
                                   Maps& avoiders, Permutation p, unsigned size) {
        assert(size == p.size());
//        Map next_avoiders;
        for (int j=0; j < patterns.getBound()+1; ++j) {
            Permutation parent = p; parent.down(j);
            auto it = avoiders[size-1].find(parent);
            if (it != avoiders[size-1].end()) {
                
            } else { // not avoider
                return;
            }
        }
#if DEBUG
//        std::cout << "# " << p << " found" << std::endl;
#endif
        // Perm is ok, 0 as only place holder
        avoiders[size].insert({p, 0});
        
        if (size+1 < Permutation::MAX_SIZE) {
            walkChildren(p, size, [&patterns, &avoiders, size](auto&& child) {
                buildAvoidersDfsRecursive(patterns, avoiders, child, size+1);
            });
        }
        
    }

    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation, unsigned, typename PermutationSet::Hash>
    >
    std::array<int, Permutation::MAX_SIZE>
    buildAvoidersDfs(const PermutationSet& patterns) {
        
        auto small_result = buildAvoidersOfPatternsBoundSize(patterns);
        auto&& avoiders = std::get<std::array<Map, Permutation::MAX_SIZE>>(small_result );
        auto&& sizes_cnt = std::get<std::array<int, Permutation::MAX_SIZE>>(small_result );
//        std::cout << sizes_cnt[0] << std::endl;
        
//        std::cout << "--" << std::endl;
        for (auto&& avoider : avoiders[ patterns.getBound() ]) {
            Permutation p = std::get<0>(avoider);
//            std::cout << p << std::endl;
//            std::cout << patterns.getBound() << std::endl;
//            std::cout << p.size() << std::endl;
            walkChildren(p, patterns.getBound(), [&patterns, &avoiders](auto&& child) {
                buildAvoidersDfsRecursive(patterns, avoiders, child, patterns.getBound()+1);
            });
        }

        for (int i=0; i < Permutation::MAX_SIZE; ++i) {
            sizes_cnt[i] = avoiders[i].size();
        }
        
        return sizes_cnt;
    }

};

#endif /* avoiders_hpp */

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

namespace rp {
    
    template <class PermutationSet, class Permutation = typename PermutationSet::Permutation>
    bool isAvoider(const PermutationSet& avoiders, const PermutationSet& patterns,
                   const Permutation& permutation, unsigned size) {
        if ( patterns.lookup(permutation) )
            return false;
        
        for (int i=0; i < std::min(size, patterns.getBound()+1); ++i) {
            Permutation down = permutation; down.down(i);
            if ( !avoiders.lookup( down ) ) {
                return false;
            }
        }
        return true;
    }
    
    template <class PermutationSet, class Permutation = typename PermutationSet::Permutation>
    std::array<int, Permutation::MAX_SIZE>
    buildAvoiders(const PermutationSet& patterns) {
        PermutationSet avoiders, next_avoiders;
        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
        for (int& size_cnt : sizes_cnt) size_cnt = 0;
        
        if (Permutation::MAX_SIZE == 0) return sizes_cnt;
        
        std::queue<Permutation> unprocessed;
        if ( !patterns.lookup(Permutation{}) ) {
            unprocessed.push(Permutation{});
            avoiders.insert(Permutation{});
        }
        
        if (Permutation::MAX_SIZE == 1) return sizes_cnt;
        
        int actual_size = 1;
        int actual_size_cnt = 1;
        int next_size_cnt = 0;
        
        while ( !unprocessed.empty() ) {
            Permutation& perm = unprocessed.front();
            perm.up(0, actual_size);
            
            for (int i = 0; i < actual_size+1; ++i) {
                if (i > 0) perm.swapNext(i-1);
                
                
                if (!isAvoider(avoiders, patterns, perm, actual_size))
                    continue;
                
                auto ins = next_avoiders.insert(perm);
                assert(ins.second == true); // inserted new
                
                if (actual_size+1 < Permutation::MAX_SIZE) {
                    unprocessed.push(perm);
                }
                next_size_cnt++;
            }
                
            unprocessed.pop();
            actual_size_cnt--;
            if (actual_size_cnt <= 0) {
                assert(actual_size_cnt == 0);
                avoiders.getTable().clear();
                std::swap( avoiders, next_avoiders);
                
                sizes_cnt[actual_size] = actual_size_cnt = next_size_cnt;
                actual_size++;
                next_size_cnt = 0;
            }
        }
        
        sizes_cnt[actual_size] = next_size_cnt;
        return sizes_cnt;
    }
    
};

#endif /* avoiders_hpp */

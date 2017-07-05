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
        sizes_cnt[1] = 1;
        
        if (Permutation::MAX_SIZE == 0) return sizes_cnt;
        
        std::queue<Permutation> unprocessed;
        if ( !patterns.lookup(Permutation{}) ) {
            unprocessed.push(Permutation{});
            avoiders.insert(Permutation{});
        }
        
        if (Permutation::MAX_SIZE == 1) return sizes_cnt;
        
        int actual_size = 2;
        int actual_size_cnt = 1;
        int next_size_cnt = 0;
        
        while ( !unprocessed.empty() ) {
            Permutation& perm = unprocessed.front();
#if DEBUG
            std::cout  << "pop" << actual_size<<"\t" << perm << std::endl;
#endif
            perm.up(0, actual_size-1);
#if DEBUG
            std::cout  << "up\t" << perm << std::endl;
#endif
            std::array<bool, Permutation::MAX_SIZE> is_avoider;
            is_avoider.fill(true);
            
            for (int j=1; j < std::min((unsigned)actual_size, patterns.getBound()+1); ++j) {
                Permutation down = perm; down.down(j);
#if DEBUG
                std::cout << "down\t" << down << std::endl;
#endif
                int k = -1;
                for (int i = 0; i < actual_size; ++i) {
                    if (i!=j) {
                        if (k >= 0) down.swapNext(k);
                        k++;
                    }
#if DEBUG
                    std::cout << "down_\t" << down <<"["<<i<< "," <<j<<"]";
                    if ( is_avoider[i] == false )
                        std::cout << " _";
#endif
                    if ( is_avoider[i] == false ) {
                        continue;
                    }
                    
                    if ( !avoiders.lookup( down ) ) {
                        is_avoider[i] = false;
#if DEBUG
                        std::cout << " no" << std::endl;
#endif
                    } else {
#if DEBUG
                        std::cout << " ok" << std::endl;
#endif
                    }
  
                }
            }
            
            for (int i = 0; i < actual_size; ++i) {
                if (i > 0) perm.swapNext(i-1);
                if ( actual_size < patterns.getBound()+1 &&  patterns.lookup(perm) )
                    is_avoider[i] = false;
                if (is_avoider[i]) {
#if DEBUG
                    std::cout << "in\t" << perm << std::endl;
#endif
                    next_size_cnt++;
                    auto ins = next_avoiders.insert(perm);
                    assert(ins.second == true); // inserted new
                    
                    if (actual_size+1 < Permutation::MAX_SIZE) {
                        unprocessed.push(perm);
                    }
                } else {
#if DEBUG
                    std::cout << "out\t" << perm << std::endl;
#endif
                }
            }
                
            unprocessed.pop();
            actual_size_cnt--;
            if (actual_size_cnt <= 0) {
                assert(actual_size_cnt == 0);
                
//                if (actual_size < 6) {
//                    std::cout << "Avoides " << actual_size << std::endl;
//                    for (auto&& p : avoiders.getTable()) {
//                        std::cout << p << std::endl;
//                    }
//                }
//                
//                std::cout << "\t" << next_size_cnt << " " << next_avoiders.size() << std::endl;
                
                avoiders.getTable().clear();
                std::swap( avoiders, next_avoiders);
//                for (auto&&p:avoiders.getTable()) {
//                    std::cout << "Av\t" << p << std::endl;
//                }
                
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

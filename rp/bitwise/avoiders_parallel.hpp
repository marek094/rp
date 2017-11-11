//
//  avoiders.hpp
//  rp
//
//  Created by Marek Černý on 01/07/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#ifndef avoiders_parallel_hpp
#define avoiders_parallel_hpp

#include "permutation.hpp"
#include "permutation_set.hpp"
#include "avoiders.hpp"
#include <cassert>
#include <queue>
#include <array>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <future>

#define DEBUG 0

namespace rp {
    
    
    template <unsigned Threads = 0>
    std::pair<std::size_t, std::size_t> getGranulationByThreads(std::size_t work_size) {
        std::size_t cnt = std::max(Threads, std::thread::hardware_concurrency());
        std::size_t size = (work_size+cnt) / cnt; //(% \in {0..cnt})
        return {cnt, size};
    }
    
    template <unsigned SignifSize = 10000>
    std::pair<std::size_t, std::size_t> getGranulationByWork(size_t work_size) {
        std::size_t size = SignifSize;
        std::size_t cnt = (work_size + SignifSize) / SignifSize;
        return {cnt, size};
    }
    
    // given permutation and create extension maps
    template <class Permutation, class Map>
    unsigned inline getNotAvoiders(const Map& avoiders, const Map& others,
                            unsigned bound, unsigned size,
                            Permutation perm) {
        unsigned not_avoiders = 0;
        perm.up(0, size-1);
        for (int j=1; j < bound+1; ++j) {
            Permutation down = perm; down.down(j);
            unsigned not_avoider = (1u<<size)-1;
            auto it = avoiders.find(down);
            if (it == avoiders.end()) {
                it = others.find(down);
            }
            not_avoider = copyIthBit(it->second, j-1);
            not_avoiders |= not_avoider;
        }
        return not_avoiders;
    };
    
    
    template <class PermutationSet, class Permutation = typename PermutationSet::Permutation>
    std::array<int, Permutation::MAX_SIZE>
    buildAvoidersParallel(const PermutationSet& patterns) {
        
        using Map = std::unordered_map<Permutation,  unsigned, typename PermutationSet::Hash>;
        Map avoiders, next_avoiders, others, next_others;
        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
        unsigned bound = patterns.getBound();
        sizes_cnt.fill(0);
        sizes_cnt[1] = 1;
        
        if (Permutation::MAX_SIZE < 2) return sizes_cnt;
        
        if ( !patterns.lookup(Permutation{}) ) {
            avoiders.insert({Permutation{}, 0u});
        }
        
        unsigned actual_size = 2;
        

        for (; actual_size < patterns.getBound()+1; ++actual_size) {
            for (auto&& p : avoiders) {
                Permutation perm = p.first;
                unsigned not_avoiders = 0;
                perm.up(0, actual_size-1);
                // before all patterns take shape in avoiders
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
                Permutation perm2 = perm;
                for (int i = 0; i < actual_size; ++i) {
                    if (i > 0) {
                        perm2.swapNext(i-1);
                    }
                    if (patterns.lookup(perm2)) {
                        setBit(not_avoiders, i);
                    }
                }
                
                for (int i = 0; i < actual_size; ++i) {
                    if (i > 0) perm.swapNext(i-1);
                    if ( !getBit(not_avoiders, i) ) {
                        auto ins = next_avoiders.insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    } else {
                        auto ins = next_others.insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    }
                }
            }
            sizes_cnt[actual_size] = next_avoiders.size();
            
            avoiders.clear();
            double coeff = (double)sizes_cnt[actual_size] / (double)sizes_cnt[actual_size-1];
            double coeff2 = (double)next_others.size() / std::max(1.0,(double)others.size());
            //                double coeff2 = 1.0; double coeff = 1.0;
            avoiders.reserve( sizes_cnt[actual_size] * 5 * std::ceil(coeff) );
            std::swap( avoiders, next_avoiders);
            others.clear();
            others.reserve(   sizes_cnt[actual_size] * 5 * std::ceil(coeff2) );
            std::swap( others, next_others);
        }
        
        
        
        for (; actual_size < Permutation::MAX_SIZE-1; ++actual_size) {
            for (auto&& p : avoiders) {
                Permutation perm = p.first;
                unsigned not_avoiders = getNotAvoiders(avoiders, others, bound, actual_size, perm);
                for (int i = 0; i < actual_size; ++i) {
                    if (i == 0) perm.up(0, actual_size-1);
                    else perm.swapNext(i-1);
                    if ( !getBit(not_avoiders, i) ) {
                        auto ins = next_avoiders.insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    } else {
                        auto ins = next_others.insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    }
                    
                }
            }
            sizes_cnt[actual_size] = next_avoiders.size();
            
            avoiders.clear();
            double coeff = (double)sizes_cnt[actual_size] / (double)sizes_cnt[actual_size-1];
            double coeff2 = (double)next_others.size() / std::max(1.0,(double)others.size());
            //                double coeff2 = 1.0; double coeff = 1.0;
            avoiders.reserve( sizes_cnt[actual_size] * 5 * std::ceil(coeff) );
            std::swap( avoiders, next_avoiders);
            others.clear();
            others.reserve(   sizes_cnt[actual_size] * 5 * std::ceil(coeff2) );
            std::swap( others, next_others);
        }
        
        
        auto runTask = [&avoiders, &others, bound, actual_size] (auto begin, auto end) {
            unsigned cnt = 0;
            for (auto it = begin; it!=end; ++it) {
                Permutation perm = *it;
                unsigned not_avoiders = getNotAvoiders(avoiders, others, bound, actual_size, perm);
                cnt += actual_size - __builtin_popcount(not_avoiders);
            }
            return cnt;
        };
        
        {   // Parallel part
            using namespace std;
            vector<Permutation> work;
            work.reserve(avoiders.size());
            for (auto&& a: avoiders) work.push_back(a.first);
            
            auto [chunk_cnt, chunk_size] = getGranulationByWork<10000>(work.size());
            
            vector<future<unsigned>> fresults;
            for (int t = 0; t < work.size(); t += chunk_size) {
                future<unsigned> fr = async(launch::async,
                                            runTask,
                                            work.begin()+t,
                                            min(work.begin()+t+chunk_size, work.end()));
                fresults.emplace_back(move(fr));
            }
            
            sizes_cnt[actual_size] = 0;
            for (future<unsigned>& fr : fresults) {
                sizes_cnt[actual_size] += fr.get();
            }
        }
        
        return sizes_cnt;
    }
    
};

#endif /* avoiders_parallel_hpp */

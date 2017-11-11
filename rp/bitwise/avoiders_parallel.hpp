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
    
    
    template <unsigned Threads = 0> // 0 means std::thread::hardware_concurrency()
    std::pair<std::size_t, std::size_t> getGranulationByThreads(std::size_t work_size) {
        std::size_t cnt = Threads > 0 ? Threads : std::thread::hardware_concurrency());
        std::size_t size = (work_size+cnt) / cnt; //(% \in {0..cnt})
        return {cnt, size};
    }
    
    template <unsigned SignifSize = 10000>
    std::pair<std::size_t, std::size_t> getGranulationByWork(size_t work_size) {
        std::size_t size = SignifSize;
        std::size_t cnt = (work_size + SignifSize) / SignifSize;
        return {cnt, size};
    }
    
//    template <class PermutationSet>
//    class buildAvoidersParallelCl {
//        using Permutation = typename PermutationSet::Permutation;
//        using Map = std::unordered_map<Permutation, unsigned, typename PermutationSet::Hash>;
//    public:
//        buildAvoidersParallel(PermutationSet&& patterns) : patterns{std::move(patterns)} {}
//
//        std::array<int, Permutation::MAX_SIZE> operator()() {
//            return {}
//        }
//
//
//    private:
//        Map avoiders, next_avoiders, others, next_others;
//        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
//        unsigned bound, actual_size;
//        // input
//        PermutationSet patterns;
//        // result
//        std::array<int, Permutation::MAX_SIZE> result;
//    };
//
    
    // given permutation and create extension maps
    template <class Permutation, class Map>
    inline unsigned getNotAvoiders(const Map& avoiders, const Map& others,
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
    }
    
    template <class Permutation, class Func>
    inline void walkSizeUpDown(Permutation perm, unsigned size, Func&& func) {
        perm.up(0, size-1);
        func((const Permutation)perm, 0);
        for (int i = 1; i < size; ++i) {
            perm.swapNext(i-1);
            func((const Permutation)perm, i);
        }
    }
    
    template <class Permutation, class Map>
    inline void saveNext(Map& next_avoiders, Map& next_others,
                         unsigned not_avoiders, unsigned size,
                         Permutation perm) {
        walkSizeUpDown(perm, size, [&](auto&& perm, int i) mutable {
            if ( !getBit(not_avoiders, i) ) {
                auto ins = next_avoiders.insert({perm, not_avoiders});
                assert(ins.second == true); // inserted new
            } else {
                auto ins = next_others.insert({perm, not_avoiders});
                assert(ins.second == true); // inserted new
            }
        });
    }
    
    template <
        class PermutationSet,
        class Permutation = typename PermutationSet::Permutation,
        class Map = std::unordered_map<Permutation, unsigned, typename PermutationSet::Hash>
    >
    std::array<int, Permutation::MAX_SIZE>
    buildAvoidersParallel(const PermutationSet& patterns) {
        
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
                walkSizeUpDown(p.first, actual_size, [&](auto&& perm, int i) mutable {
                    if (patterns.lookup(perm)) {
                        setBit(not_avoiders, i);
                    }
                });
                saveNext(next_avoiders, next_others, not_avoiders, actual_size, p.first);
            }
            sizes_cnt[actual_size] = next_avoiders.size();
            
            avoiders.clear();
            others.clear();
            std::swap( avoiders, next_avoiders);
            std::swap( others, next_others);
        }
        
        for (; actual_size < Permutation::MAX_SIZE-1; ++actual_size) {
            
            for (auto&& p : avoiders) {
                Permutation perm = p.first;
                unsigned not_avoiders = getNotAvoiders(avoiders, others, bound, actual_size, perm);
                saveNext(next_avoiders, next_others, not_avoiders, actual_size, p.first);
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
        
        {   // Parallel part
            auto runTask = [&avoiders, &others, bound, actual_size] (auto begin, auto end) {
                unsigned cnt = 0;
                for (auto it = begin; it!=end; ++it) {
                    Permutation perm = *it;
                    unsigned not_avoiders = getNotAvoiders(avoiders, others, bound, actual_size, perm);
                    cnt += actual_size - __builtin_popcount(not_avoiders);
                }
                return cnt;
            };
            
            using namespace std;
            vector<Permutation> work;
            work.reserve(avoiders.size());
            for (auto&& a: avoiders) work.push_back(a.first);
            
            auto [chunk_cnt, chunk_size] = getGranulationByThreads(work.size());
            
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

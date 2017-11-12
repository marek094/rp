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
#include <vector>

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
        
        using namespace std;
        using Map = std::unordered_map<Permutation,  unsigned, typename PermutationSet::Hash>;
        Map avoiders, next_avoiders, others, next_others;
        std::array<int, Permutation::MAX_SIZE> sizes_cnt;
        sizes_cnt.fill(0);
        sizes_cnt[1] = 1;
        
        if (Permutation::MAX_SIZE < 2) return sizes_cnt;
        
        if ( !patterns.lookup(Permutation{}) ) {
            avoiders.insert({Permutation{}, 0u});
        }
        
        unsigned actual_size = 2;
        
        for (; actual_size < patterns.getBound()+1; ++actual_size) {
            for (auto&& perm_map : avoiders) {
                Permutation perm = perm_map.first;
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
                Permutation p = perm;
                for (int i = 0; i < actual_size; ++i) {
                    if (i > 0) {
                        p.swapNext(i-1);
                    }
                    if (patterns.lookup(p)) {
                        setBit(not_avoiders, i);
                    }
                }
                for (int i = 0; i < actual_size; ++i) {
                    if (i > 0) perm.swapNext(i-1);
                    if ( !getBit(not_avoiders, i) ) {
                        sizes_cnt[actual_size]++;
                        auto ins = next_avoiders.insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    } else {
                        auto ins = next_others.insert({perm, not_avoiders});
                        assert(ins.second == true); // inserted new
                    }
                }
            }
            avoiders.clear();
            others.clear();
            std::swap( avoiders, next_avoiders);
            std::swap( others, next_others);
        }
        
        class Record {
        public:
            enum Tag : char { AVOIDER, OTHER, FIND, NA };
            
            Record() = default;
            Record(Permutation perm, unsigned na)
                : perm(perm), tag(Record::NA), not_avoiders(na) {}
            Record(pair<Permutation, unsigned> p, Tag tag)
                : perm(p.first), tag(tag), not_avoiders(p.second) {}
            Record(Permutation perm, unsigned na, Tag tag)
                : perm(perm), tag(tag), not_avoiders(na) {}
            inline auto get() const { return make_pair(perm, not_avoiders); }
            inline auto getReason() const {
//                assert(tag == FIND);
//                Permutation for_perm = perm.up(j, perm.size()-1);
                return make_pair(for_perm, j);
            }
            inline void addReason(Permutation for_perm, unsigned j) {
                assert(tag == FIND);
                this->for_perm = for_perm;
                this->j = j;
            }
        
            // ordered to be well-packer & and fast accesible in std::sort
            Permutation perm;
            Tag tag;
            char j;
            unsigned not_avoiders;
            Permutation for_perm;
        };
        
        vector<Record> w, nextW;
        w.reserve(avoiders.size() + others.size());
        for (auto&& p : avoiders) w.emplace_back(p, Record::AVOIDER);
        for (auto&& p : others)   w.emplace_back(p, Record::OTHER);
        
        for (; actual_size < Permutation::MAX_SIZE; ++actual_size) {
            
            nextW.clear();
            // reserve because of iterator invalidation
            w.reserve(w.size() + sizes_cnt[actual_size-1]*patterns.getBound());
            for (auto&& record : w) {
                if (record.tag == Record::AVOIDER) {
                    Permutation perm = record.perm;
                    perm.up(0, actual_size-1);
                    // if permutations are big enought
                    for (int j=1; j < patterns.getBound()+1; ++j) {
                        Permutation down = perm; down.down(j);
                        w.emplace_back(down, 0, Record::FIND);
                        w.back().addReason(perm, j);
                    }
                }
            }
            
            std::sort(w.begin(), w.end(), [](auto&& r1, auto&& r2) {
                return (r1.perm < r2.perm) || (r1.perm == r2.perm && r1.tag < r2.tag);
            });
            
            for (auto it = w.begin(), found_it = it; it != w.end(); ++it) {
                if (it->tag == Record::FIND) {
                    it->not_avoiders = copyIthBit(found_it->not_avoiders, it->j - 1);
                } else {
                    found_it = it;
                }
            }
            
//            std::sort(w.begin(), w.end(), [](auto&& r1, auto&& r2) {
//                // Record::FIND first, then Reason
//                return (r1.tag > r2.tag) || (r1.tag == r2.tag && r1.for_perm < r2.for_perm);
//            });
        
            auto it_border = std::remove_if(w.begin(), w.end(), [](auto&& r1) {
                return r1.tag != Record::FIND;
            });
            std::sort(w.begin(), it_border, [](auto&& r1, auto&& r2) {
                return r1.for_perm < r2.for_perm;
            });

            unsigned not_avoiders = 0;
            // 'Elephant in Cairo':
            Permutation prev_for_perm = w.front().for_perm;
            *it_border = Record{};
            for (auto it=w.begin(); it!=next(it_border); ++it) {
                auto&& p = *it;
                if (prev_for_perm == p.for_perm) {
                    not_avoiders |= p.not_avoiders;
                    continue;
                }
                // !=
                if (actual_size+1 < Permutation::MAX_SIZE) {
                    for (int i = 0; i < actual_size; ++i) {
                        if (i > 0) prev_for_perm.swapNext(i-1);
                        if ( !getBit(not_avoiders, i) ) {
                            sizes_cnt[actual_size]++;
                            nextW.emplace_back(prev_for_perm, not_avoiders, Record::AVOIDER);
                        } else {
                            nextW.emplace_back(prev_for_perm, not_avoiders, Record::OTHER);
                        }
                    }
                } else {
                    sizes_cnt[actual_size] += actual_size - __builtin_popcount(not_avoiders);
                }
    
                prev_for_perm = p.for_perm;
                not_avoiders = p.not_avoiders;
            }
            
            std::swap(w, nextW);
        }
        
        return sizes_cnt;
    }
    
};

#endif /* avoiders_hpp */

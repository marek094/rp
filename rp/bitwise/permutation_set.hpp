//
//  permutation_set.hpp
//  rp
//
//  Created by Marek Černý on 01/07/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#ifndef permutation_set_hpp
#define permutation_set_hpp

#include "permutation.hpp"
#include <unordered_set>
#include <functional>
#include <algorithm>

namespace rp {
    
    
    template <unsigned LETTER, unsigned MAX_SIZE = defaultMAXSIZE(LETTER)>
    class PermutationSet {
    public:
        using Permutation = rp::Permutation<LETTER, MAX_SIZE>;

        PermutationSet() {}

        template <class P>
        auto insert(P&& p, unsigned size) {
            bound = std::max(bound, size);
            return table.insert(std::forward<P>(p));
        }
        
        template <class P>
        auto insert(P&& p) {
            return table.insert(std::forward<P>(p));
        }

        bool lookup(const Permutation& p) const {
            return table.find(p) != table.end();
        }

        auto&& getTable() {
            return table;
        }
        
        auto&& getTable() const {
            return table;
        }

        auto size() const {
            return table.size();
        }
        
        auto empty() const {
            return table.empty();
        }
        
        auto getBound() const {
            return bound;
        }
        
        PermutationSet& operator=(const PermutationSet& ps) {
            bound = ps.bound;
            table = ps.table;
            return *this;
        }
        
        struct Hash {
            
            ull operator ()(const Permutation& p) const {
                ull hash = 0;
                for (unsigned i=0; i<Permutation::WORDS; ++i) {
                    hash ^= hash_function(p.getWord(i)) << i;
                }
                return hash;
            }
            
            std::hash<ull> hash_function{};
        };
        
    private:
        unsigned bound = 0;
        std::unordered_set<Permutation, Hash> table;
    };

}

#endif /* permutation_set_hpp */

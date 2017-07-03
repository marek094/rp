//
//  simple_avoiders.cpp
//  rp
//

#include "simple_avoiders.hpp"
#include <cassert>
#include <queue>
#include <algorithm>

namespace rp_simple {
    
    bool is_avoider(const_set avoiders,
                    const_bounded_set patterns,
                    const_perm permutation)
    {
        if (patterns.find(permutation) != patterns.end())
            return false;
        
        for (int i=0; i < patterns.bound()+1 && i < permutation.size(); i++) {
            if (avoiders.find( perm_down(permutation, i) ) == avoiders.end()) {
                return false;
            }
        }
        
        return true;
    }
    
    set build_avoiders(const_bounded_set patterns, size_t n) {
        set avoiders;

        if (n == 0) return avoiders;
        
        std::queue<perm> unprocessed;
        if (patterns.find(IDENTITY) == patterns.end()) {
            unprocessed.push(IDENTITY);
            avoiders.insert(IDENTITY);
        }
        
        if (n == 1) return avoiders;
        
        while (!unprocessed.empty()) {
            perm permutation = std::move(unprocessed.front());
            unprocessed.pop();
            
            for (int i=0; i<permutation.size()+1; i++) {
                perm up_permutation = perm_up(permutation, i);
                if (!is_avoider(avoiders, patterns, up_permutation))
                    continue;
                
                auto ins = avoiders.insert(up_permutation);
                assert(ins.second == true); // inserted new
                
                if (up_permutation.size() < n) {
                    unprocessed.push(up_permutation);
                }
            
            }
        }
        
        return avoiders;
    }
    
    perm perm_down(const_perm p, size_t position) {
        position = (int)p.size() - position-1;
        assert(0 <= position);
        assert(position < p.size());
        
        std::string result;
        result.resize(p.size()-1);
        
        char del = p[position];
        auto rit = result.begin();
        
        for (int i=0; i<p.size(); i++) {
            if (i == position) continue;
            // normalize
            *rit++ = (p[i] > del ? p[i]-1 : p[i]);
        }
        
        return result;
    }
    
    
    perm perm_up(const_perm p, size_t position) {
        assert(0 <= position);
        assert(position < p.size()+1);
        
        std::string result;
        result.resize(p.size()+1);
        auto rit = result.begin();
        
        int i = 0;
        for (; i < position; i++) {
            *rit++ = p[i];
        }
        
        *rit++ = '0' + p.size()+1;
        for (; i < p.size(); i++) {
            *rit++ = p[i];
        }
        
        return result;
    }
    
};

namespace rp_simple {
    
    size_t count_hits(const_perm permutation, const_bounded_set patterns) {
        hits_table table;
        auto&& hits = count_hits(permutation, patterns, table);
        assert( hits.size() > 0);
        return hits[0];
    }
    
    std::vector< size_t>& count_hits(const_perm permutation,
                                  const_bounded_set patterns,
                                  hits_table& table)
    {
        std::vector< size_t>& hits = table[permutation];
        
        // already computed
        if (! hits.empty()) return hits;
        hits.resize(patterns.bound()+2, 0);
       
        for (int i=(int)patterns.bound(); i>=0; --i) {
            if (i == permutation.size() &&
                patterns.find(permutation) != patterns.end()) {
                hits.at(i) = 1;
            } else if (i < permutation.size()) {
                perm down_permutation = perm_down(permutation, i);
                auto&& down_hits = count_hits(down_permutation, patterns, table);
                
                hits.at(i) = down_hits.at(i) + hits.at(i+1);
            }
        }
        
        return hits;
    }
    

};

namespace rp_simple {
    
    constexpr auto UNDEFINED = -1;
    
    bool count_hits_bounded(const_set bound_avoiders,
                            const_perm permutation, const_bounded_set patterns, size_t bound) {
        hits_table table;
        auto&& hits = count_hits_bounded(bound_avoiders, permutation, patterns, bound, table);
        assert( hits.size() > 0 );
        assert( hits[0] != rp_simple::UNDEFINED);
        return hits[0] > bound;
    }
    
    
    std::vector< size_t>& count_hits_bounded(const_set bound_avoiders,
                                             const_perm permutation,
                                             const_bounded_set patterns,
                                             size_t bound,
                                             hits_table& table)
    {
        
        std::vector< size_t>& hits = table[permutation];
        
        if (!hits.empty()) return hits;
        hits.resize(patterns.bound()+2, 0);
        
        for (int i=(int)patterns.bound(); i>=0; --i) {
            if (i == permutation.size() &&
                patterns.find(permutation) != patterns.end()) {
                hits.at(i) = 1;
            } else if (i < permutation.size()) {
                perm down_permutation = perm_down(permutation, i);
                
                auto it = bound_avoiders.find(down_permutation);
                if (it == bound_avoiders.end()) {
                    for (int r = (int)patterns.bound()+1; r >= i+1; --r) {
                        hits.at(i) = UNDEFINED;
                    }
                    return hits;
                }
                
                auto&& down_hits = count_hits(down_permutation, patterns, table);
                hits.at(i) = down_hits.at(i) + hits.at(i+1);
            }
        }
        
        assert(hits[0] != UNDEFINED);
        if (hits.at(0) > bound) {
            for (int i = (int)patterns.bound()+1; i>=0; --i) {
                hits.at(i) = UNDEFINED;
            }
        }
        
        return hits;
    }
  
    set build_perms_with_bounded_hits(const_bounded_set patterns, size_t n, size_t bound) {
        set bound_avoiders;
        
        if (n == 0) return bound_avoiders;
        
        std::queue<perm> unprocessed;
        if (patterns.find(IDENTITY) == patterns.end() && bound > 0) {
            unprocessed.push(IDENTITY);
            bound_avoiders.insert(IDENTITY);
        }
        
        if (n == 1) return bound_avoiders;
        
        while (! unprocessed.empty()) {
            perm permutation = std::move(unprocessed.front());
            unprocessed.pop();
            
            for (int i=0; i < permutation.size()+1; ++i) {
                perm up_permutation = perm_up(permutation, i);
                if (count_hits_bounded(bound_avoiders, permutation, patterns, bound)) {
                    if (up_permutation.size() < n) {
                        unprocessed.push(up_permutation);
                    }
                    bound_avoiders.insert(std::move(up_permutation));
                }
            }
            
        }
        
        return bound_avoiders;
    }
    
};

/*
 while (!unprocessed.empty()) {
 perm permutation = std::move(unprocessed.front());
 unprocessed.pop();
 
 for (int i=0; i<permutation.size()+1; i++) {
 perm up_permutation = perm_up(permutation, i);
 if (!is_avoider(avoiders, patterns, up_permutation))
 continue;
 
 auto ins = avoiders.insert(up_permutation);
 assert(ins.second == true); // inserted new
 
 if (up_permutation.size() < n) {
 unprocessed.push(up_permutation);
 }
 
 }
 }
 */
 
 
 
 
 
 
 


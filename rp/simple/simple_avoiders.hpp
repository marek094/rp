//
//  simple_avoiders.hpp
//  rp
//

#ifndef simple_avoiders_hpp
#define simple_avoiders_hpp

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <cassert>

namespace rp {
namespace simple {
  
    // simple types
    using perm = std::string;
    using const_perm = const perm&;
    using set = std::unordered_set< perm>;
    using const_set = const set&;
    using hits_table = std::unordered_map< perm, std::vector< size_t>>;
    class non_consistent_bounded_set;
    using bounded_set = const non_consistent_bounded_set;
    using const_bounded_set = const bounded_set&;
    
    constexpr auto IDENTITY = "1";
    
    // operations on permutations
    perm perm_down(const_perm p, size_t position);
    perm perm_up(const_perm p, size_t position);
    
    
    // "Algorithm 2"
    set build_avoiders(const_bounded_set patterns, size_t n);
    
    // "Algorithm 3"
    size_t count_hits(const_perm, const_bounded_set patterns);
    std::vector< size_t>& count_hits(const_perm,
                                     const_bounded_set, hits_table&);
    
    // "Algorithm 5"
    bool count_hits_bounded(const_set bound_avoiders,
                            const_perm, const_bounded_set patterns, size_t bound);
    std::vector< size_t>& count_hits_bounded(const_set bound_avoiders,
                                            const_perm,
                                            const_bounded_set,
                                            size_t, hits_table&);
    
    set build_perms_with_bounded_hits(const_bounded_set patterns,
                                      size_t n, size_t bound);
    
    
    // set extended with bound
    class non_consistent_bounded_set : public set {
    public:
        
        non_consistent_bounded_set() : set() {}
        
        non_consistent_bounded_set(std::initializer_list<value_type> arg)
            : set(arg) { update_bound(); }
        
        template<class X>
        non_consistent_bounded_set(X && arg)
            : set(std::forward< X>(arg)) { update_bound(); }
        
        template<class X>
        non_consistent_bounded_set(X && arg, size_type bound)
            : set(std::forward< X>(arg)), bound_(bound) {}
        
        template<class Iter>
        non_consistent_bounded_set(Iter first, Iter last)
            : set(first, last) { update_bound(); }
    
        size_type bound() const { return bound_; }
        
        void update_bound() {
            //auto old_bound = bound_;
            bound_ = 0;
            for (const perm &p : *this) {
                if (p.size() > bound_) {
                    bound_ = p.size();
                }
            }
            //return old_bound == bound_;
        }
        
        void update_bound(const_iterator it) {
            if (it->size() > bound_) bound_ = it->size();
        }
        
    private:
        size_type bound_;
    };
    
    
}
}

#include <cassert>
#include <queue>
#include <algorithm>

namespace rp {
namespace simple {
    
    bool is_avoider(const_set avoiders,
                    const_bounded_set patterns,
                    const_perm permutation)
    {
        if (patterns.find(permutation) != patterns.end())
            return false;
        
        for (unsigned i=0; i < patterns.bound()+1 && i < permutation.size(); i++) {
            if (avoiders.find( perm_down(permutation, i) ) == avoiders.end()) {
                return false;
            }
        }
        
        return true;
    }
    
    set build_avoiders(const_bounded_set patterns, size_t n) {
        set avoiders;
        
        std::queue<perm> unprocessed;
        if (patterns.find(IDENTITY) == patterns.end()) {
            unprocessed.push(IDENTITY);
            avoiders.insert(IDENTITY);
        }
        
        if (n == 0) return avoiders;
        if (n == 1) return avoiders;
        
        while (!unprocessed.empty()) {
            perm permutation = std::move(unprocessed.front());
            unprocessed.pop();
            
            for (int i=0; i<(int)permutation.size()+1; i++) {
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
        
        for (unsigned i=0; i<p.size(); i++) {
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
        
        unsigned i = 0;
        for (; i < position; i++) {
            *rit++ = p[i];
        }
        
        *rit++ = '0' + p.size()+1;
        for (; i < p.size(); i++) {
            *rit++ = p[i];
        }
        
        return result;
    }
    
}
}

namespace rp {
namespace simple {
    
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
            if (i == (int)permutation.size() &&
                patterns.find(permutation) != patterns.end()) {
                hits.at(i) = 1;
            } else if (i < (int)permutation.size()) {
                perm down_permutation = perm_down(permutation, i);
                auto&& down_hits = count_hits(down_permutation, patterns, table);
                
                hits.at(i) = down_hits.at(i) + hits.at(i+1);
            }
        }
        
        return hits;
    }
    
    
}
}

namespace rp {
namespace simple {
    
    constexpr auto UNDEFINED = ~0u;
    
    bool count_hits_bounded(const_set bound_avoiders,
                            const_perm permutation, const_bounded_set patterns, size_t bound) {
        hits_table table;
        auto&& hits = count_hits_bounded(bound_avoiders, permutation, patterns, bound, table);
        assert( hits.size() > 0 );
        assert( hits[0] != rp::simple::UNDEFINED);
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
            if (i == (int)permutation.size() &&
                patterns.find(permutation) != patterns.end()) {
                hits.at(i) = 1;
            } else if (i < (int)permutation.size()) {
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
            
            for (int i=0; i < (int)permutation.size()+1; ++i) {
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
    
}
};

#endif /* simple_avoiders_hpp */

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

namespace rp_simple {
  
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
    
    
};


#endif /* simple_avoiders_hpp */

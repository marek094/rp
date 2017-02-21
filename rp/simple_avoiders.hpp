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
    size_t count_hits(const_perm, const_bounded_set);
    std::vector< size_t>& count_hits(const_perm,
                                     const_bounded_set, hits_table&);
    
    
    // set extended with bound
    class non_consistent_bounded_set : public rp::set {
    public:
        
        non_consistent_bounded_set() : rp::set() {}
        
        non_consistent_bounded_set(std::initializer_list<value_type> arg)
            : rp::set(arg) { update_bound(); }
        
        template<class X>
        non_consistent_bounded_set(X && arg)
            : rp::set(std::forward< X>(arg)) { update_bound(); }
        
        template<class X>
        non_consistent_bounded_set(X && arg, size_type bound)
            : rp::set(std::forward< X>(arg)), bound_(bound) {}
        
        template<class Iter>
        non_consistent_bounded_set(Iter first, Iter last)
            : rp::set(first, last) { update_bound(); }
    
        size_type bound() const { return bound_; }
        
        void update_bound() {
            //auto old_bound = bound_;
            bound_ = 0;
            for (const rp::perm &p : *this) {
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

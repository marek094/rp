//
//  invariant.hpp
//  rp
//
//  Created by Marek Černý on 01/07/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#ifndef invariant_hpp
#define invariant_hpp

template<class Test>
class Invariant {
public:
    
    Invariant(std::string name, Test&& f) : f(std::move(f)), name(name) {}
    explicit Invariant(Test&& f) : f(std::move(f)), name("unnamed") {}
    
    template<class... Args>
    bool test(Args&&...args) {
        bool satisfied = f(std::forward<Args>(args)...);
        wasAlwaysSatisfied = satisfied && wasAlwaysSatisfied;
        return satisfied;
    }
    
    bool satisfied() const {
        return wasAlwaysSatisfied;
    }
    
    std::string result() const {
        if (this->satisfied()) {
            return "Invariant " + name + " passed";
        } else {
            return "Invariant " + name + " not passed";
        }
    }
    
private:
    Test f;
    std::string name;
    bool wasAlwaysSatisfied = true;
};

template<class Test>
auto make_invariant(Test&& f) {
    return Invariant<Test>(std::forward<Test>(f));
}

template<class Test>
auto make_invariant(std::string&& name, Test&& f) {
    return Invariant<Test>(std::move(name), std::forward<Test>(f));
}

#endif /* avoiders_hpp */

//
//  altrepr.cpp
//  rp
//
//  Created by Marek Černý on 06/11/2017.
//  Copyright © 2017 Marek Cerny. All rights reserved.
//

#include "bitwise/permutation.hpp"
#include <iostream>
#include <vector>
#include <functional>
#include <iomanip>

using namespace rp;

using Perm = Permutation<4>;


auto convert(Perm p) {
    auto result = std::vector<int>{};
//    result.push_back(0);
    for (int x = 0; x < p.size(); ++x) {
        int ri = 0;
        for (int i = 0; i < p.size(); ++i) {
            if (p[i] >  x) continue;
            if (p[i] == x) break;
            ++ri;
        }
        result.push_back(ri);
    }
    return result;
}

Perm inverse(Perm p) {
    auto res = std::vector<int>(p.size());
    for (int i=0; i<p.size(); ++i) {
        res[ p[i] ] = i;
    }
    return {res.begin(), res.end()};
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T> &v) {
    for (auto&& p : v) os << p;
    return os;
}

template<class Func>
class Invariant {
public:
    
    Invariant(std::string name, Func&& f) : f(std::move(f)), name(name) {}
    explicit Invariant(Func&& f) : f(std::move(f)), name("unnamed") {}

    template<class... Args>
    bool test(Args&&...args) {
        bool satisfied = f(std::forward<Args>(args)...);
//         (res != Result::FAIL);
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
    Func f;
    std::string name;
    bool wasAlwaysSatisfied = true;
};

template<class Func>
auto make_invariant(Func&& f) {
    return Invariant<Func>(std::forward<Func>(f));
}

template<class Func>
auto make_invariant(std::string&& name, Func&& f) {
    return Invariant<Func>(std::move(name), std::forward<Func>(f));
}

auto invA = make_invariant( "A", [](std::vector<int> p, std::vector<int> c, int i) {
    auto it = std::find(p.begin(), p.end(), p.size()-1);
    if (it != p.end()) return true;
    if (std::distance(p.begin(), it) != i) return true;
    p.erase(it);
    return (p == c);
});

auto invB = make_invariant( "B", [](std::vector<int> p, std::vector<int> c, int i) {
    if (i > 0) return true;
    
    
    return false;
});

auto invC = make_invariant( "Same", [](auto&& a, auto&& b, auto&& p) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    if (a!=b) {
//        std::cout << "\t ";
//        for (auto x:a) std::cout << x << ' ';
//        std::cout << std::endl << "\t ";
//        for (auto x:b) std::cout << x << ' ';
//        int cc = 0;
//        for (int i=0; i<a.size(); ++i) cc += a[i]!=b[i];
//        std::cout << "  " << cc << " " << p << std::endl;
//        std::cout << std::endl;
        return false;
    }
    return true;
});

template <class Func>
void downsi(std::vector<int> v, Func&& f) {
    for (int i=0; i<v.size(); ++i) {
        auto w = v;
        auto x = w[i];
        w.erase(w.begin()+i);
        for (int j=i; j<w.size(); ++j) {
            if (w[j] > x) w[j]--;
            else x++;
        }
        f(w);
    }
}

void dfs(unsigned msz, const Perm& p = Perm{}) {
    
    if (p.size() > msz) return;
//    std::cout << p << " --> " << convert(p) << std::endl;
    if (p.size() == msz) {
        std::cout << inverse(p) << "-> " << convert(inverse(p))<< std::endl;
        std::cout << p << "-> " <<  convert(p) << "  ";
        std::vector<std::vector<int>> a,b;
        p.walkDownsi( [&p,&a](auto&& c, int i) {
            a.push_back(convert(c));
            std::cout << convert(c) << ' ';
//            std::cout << "\t\t" << c << " --> " << convert(c);
//            std::cout << (invA.test(convert(p), convert(c), i) ? " k" : " -");
//            std::cout << (invB.test(convert(p), convert(c), i) ? " k" : " -");
//
//            std::cout << std::endl;
        });
        std::cout << "  ";
        downsi(convert(p), [&b](auto&& v){
            b.push_back(v);
        });
//        std::sort(b.begin(), b.end());
        for (int i=0;i<b.size();++i) {
            std::cout << b[i] << ' ';
        }
//        std::cout << "  ";
//        for (int i=0;i<b.size();++i) {
//            if (i>0 && b[i]==b[i-1])
//                std::cout << std::setw(3) << i << "  ";
//            else
//                std::cout << b[i] << ' ';
//        }
        std::cout << std::endl;
        invC.test(a,b,p);
        
    }
    p.walkChildren( std::bind( dfs, msz, std::placeholders::_1) );
//    std::cout << std::endl;
}

void dfs2(unsigned msz, const Perm& p = Perm{}) {
    if (p.size() == msz-1) {
//        std::cout <<  p << " -> " <<  convert(p) << std::endl;
//        std::vector<std::vector<Perm>> vvp;
//        p.walkChildren([&](auto&& c){
//            c.walkDowns([])
//        });
//
    } else {
        p.walkChildren( std::bind( dfs2, msz, std::placeholders::_1) );
    }
}

Perm hexToPerm(unsigned hex) {
    auto result = std::vector<int>{};
    for (int zerocnt = 0;;) {
        unsigned v = hex % 16;
        if (v == 0) zerocnt++;
        if (zerocnt > 1) break;
        result.push_back(v);
        hex /= 16;
    }
    // backwards
    return Perm{result.rbegin(), result.rend()};
}


int main() {
    
    dfs(5);
    std::cout << invA.result() << std::endl;
    std::cout << invB.result() << std::endl;
    std::cout << invC.result() << std::endl;

    for (auto hv : {0x0,0x01, 0x10, 0x120, 0x41320, 0x0123}) {
        Perm p = hexToPerm(hv);
        std::cout << p << " ——> ";
        auto r = convert(p);
        std::cout << r << std::endl;
    }
    std::vector<Perm> vp {
        hexToPerm(0x42301),
        hexToPerm(0x24301),
        hexToPerm(0x23401),
        hexToPerm(0x23041),
        hexToPerm(0x23014),
    };

    for (auto&& p : vp) {
        std::cout << p << "->" << convert(p) << ' ';
        downsi(convert(p), [](auto&& c){
            std::cout << ' ' << c;
        });
        std::cout << std::endl;
    }

    
//    dfs2(5);
    
    return 0;
}

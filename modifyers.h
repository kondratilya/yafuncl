#pragma once
#include <set>

enum class Modifyer {
    Inner, Outer, 
    Mutable, Immutable,
};

typedef std::set<Modifyer> Modifyers;


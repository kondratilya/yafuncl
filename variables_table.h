#pragma once
#include <vector>
#include <string>
#include <algorithm>

typedef size_t VariableId;

class VariablesLookup {
    std::vector<std::string> names_lookup;
    public:
    VariableId insert(std::string name) {
        auto it = std::find(names_lookup.begin(), names_lookup.end(), name);
        int name_index;
        if (it == names_lookup.end()) {
            names_lookup.push_back(name);
            return names_lookup.size()-1;
        }
        return std::distance(names_lookup.begin(), it);
    }
    std::string get(VariableId id) const { 
        return names_lookup[id];
    }
};

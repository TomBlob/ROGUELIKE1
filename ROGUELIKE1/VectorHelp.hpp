#pragma once

#include <vector>

template<typename T>
void erase_at(std::vector<T>& v, typename std::vector<T>::size_type i) {
    if (i < v.size()) {
        if (i < v.size() - 1)
            v[i] = std::move(v.back());
        v.pop_back();
    }
}
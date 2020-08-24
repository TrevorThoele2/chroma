#pragma once

#include <vector>

template <class T>
std::vector<T> operator+(const std::vector<T>& left, const std::vector<T>& right)
{
    auto result = left;
    result.reserve(left.size() + right.size());
    result.insert(result.end(), right.begin(), right.end());
    return result;
}
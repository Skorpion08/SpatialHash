#pragma once

#include <vector>
#include <algorithm>

class sorted_vector
{
public:
    sorted_vector() = default;

    sorted_vector(std::initializer_list<size_t>&& initList) : m_vector(std::move(initList)) { std::sort(m_vector.begin(), m_vector.end()); }

    bool operator==(const sorted_vector& other) const {
        return m_vector == other.m_vector;
    }

    inline size_t operator[](size_t index) const { return m_vector[index]; }

    inline size_t Size() const { return m_vector.size(); }

    void Insert(size_t newElement);

    int FindIndexFor(int x);

private:

	std::vector<size_t> m_vector;
};

struct TypeHash 
{
    size_t operator()(const sorted_vector& type) const {
        // Your custom hash implementation here
        size_t hashValue = 0;
        for (size_t i = 0; i < type.Size(); ++i) {
            // Combine the hash value with each element in the vector.
            hashValue ^= std::hash<size_t>{}(type[i]);
        }
        return hashValue;
    }
};
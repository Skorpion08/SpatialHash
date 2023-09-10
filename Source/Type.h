#pragma once

#include <vector>

class Type
{
public:

    inline size_t operator[](size_t index) const { return m_vector[index]; }

    inline size_t Size() const { return m_vector.size(); }

    void Insert(size_t newElement);

private:

	std::vector<size_t> m_vector;
};
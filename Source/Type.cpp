#include "Type.h"

void Type::Insert(size_t newElement)
{
	auto it = std::lower_bound(m_vector.begin(), m_vector.end(), newElement);
	m_vector.insert(it, newElement);
}
#include "sorted_vector.h"

void sorted_vector::Insert(size_t newElement)
{
	auto it = std::lower_bound(m_vector.begin(), m_vector.end(), newElement);
	m_vector.insert(it, newElement);
}

int sorted_vector::FindIndexFor(int x)
{
	int l = 0, r = m_vector.size(), m;

	while (l <= r)
	{
		m = l + (r - l) / 2;

		if (m_vector[m] == x)
			return m;

		if (m_vector[m] <= x)
			l = m + 1;
		else
			r = m - 1;

	}
	return -1;
}

#include "Column.h"
#include <iostream>
template<typename T, typename ...Args>
inline T* Column::Insert(Args && ...args)
{
	if (typeid(T) == type)
	{
		size_t newSize = m_count * element_size + element_size;
		if (newSize > elements.capacity())
		{
			elements.reserve(elements.capacity() * 2);
		}
		elements.resize(newSize);
		T* elementPtr = reinterpret_cast<T*>(elements.data() + m_count++ * element_size);
		*elementPtr = T(std::forward<Args>(args)...);
		return elementPtr;
	}
	return nullptr;
}

template<typename T>
inline T* Column::Get(size_t row)
{
	if (typeid(T) == type)
	{
		return reinterpret_cast<T*>(elements.data() + row * element_size);
	}
	return nullptr;
}
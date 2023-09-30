#pragma once

#include <vector>
#include <typeinfo>

struct Column
{
	Column(const std::type_info& t, size_t size) : type(t), element_size(size), m_count(0) {}

	std::vector<uint8_t> elements = {}; // We use a uint8_t as a buffer for memory
	const std::type_info& type;
	size_t element_size;
	size_t m_count = 0;

	template <typename T, typename... Args>
	T* Insert(Args&&... args);

	template <typename T>
	T* Get(size_t row);

	inline void* GetAddress(size_t row) { return elements.data() + row * element_size; }

	inline void PreallocFor(size_t numOfElements) { elements.reserve(elements.capacity() + numOfElements * element_size); }

	inline void ResizeFor(size_t numOfElements) { elements.resize(m_count *element_size + numOfElements * element_size); }
};

// We need to add some safe checks like bound checks etc.

template<typename T, typename ...Args>
T* Column::Insert(Args && ...args)
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
T* Column::Get(size_t row)
{
	if (typeid(T) == type)
	{
		return reinterpret_cast<T*>(elements.data() + row * element_size);
	}
	return nullptr;
}
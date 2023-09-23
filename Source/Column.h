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

	inline void ResizeFor(size_t numOfElements) { elements.resize(m_count*element_size + numOfElements * element_size); }
};
#pragma once

#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <memory>

namespace New {
struct ColumnBase
{
public:
	virtual void Destroy(size_t index) = 0;
};

template<typename T>
struct Column : ColumnBase
{
public:
	std::vector<T> elements = {};
	size_t m_count = 0;

	template <typename... Args>
	T* PushBack(Args&&... args);

	T* ConstructAt(size_t index, T&& value);

	void Destroy(size_t index) override;

	T* Get(size_t index);

	inline void PreallocFor(size_t numOfElements) { elements.reserve(elements.capacity() + numOfElements); }

	inline void ResizeFor(size_t numOfElements) { elements.resize(m_count + numOfElements); }
};

template<typename T>
template<typename... Args>
T* Column<T>::PushBack(Args&&... args)
{
	size_t newSize = m_count + 1;
	if (newSize > elements.capacity())
	{
		elements.reserve(elements.capacity() * 2);
	}
	elements.resize(newSize);

	new (&elements[m_count]) T(args...);

	return &elements[m_count++];
}

template<typename T>
void Column<T>::Destroy(size_t index)
{
	if (index >= m_count) throw std::out_of_range("Column index out of range!");

	// Call the destructor
	elements[index].~T();

	// If the index is the last one in the column we do not need to swap
	if (index != m_count - 1)
	{
		elements[index] = elements[m_count - 1];
	}
	--m_count;
}

template<typename T>
T* Column<T>::Get(size_t index)
{
	if (index >= m_count) throw std::out_of_range("Column index out of range!");

	return &elements[index];
}

};
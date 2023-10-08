#pragma once

#include <vector>
#include <typeinfo>

struct Column
{
	Column(const std::type_info& t, size_t size) : type(t), element_size(size), m_count(0) {}

	//void (*constructor)();

	std::vector<uint8_t> elements = {}; // We use a uint8_t as a buffer for memory
	const std::type_info& type;
	size_t element_size;
	size_t m_count = 0;

	template <typename T, typename... Args>
	T* PushBack(Args&&... args);

	template <typename T, typename... Args>
	T* ConstructAt(size_t index, Args&&... args);

	template <typename T>
	void Destroy(size_t index);

	template <typename T>
	T* Get(size_t row);

	inline void* GetAddress(size_t row) { return elements.data() + row * element_size; }

	inline void PreallocFor(size_t numOfElements) { elements.reserve(elements.capacity() + numOfElements * element_size); }

	inline void ResizeFor(size_t numOfElements) { elements.resize(m_count *element_size + numOfElements * element_size); }
};

// We need to add some safe checks like bound checks etc.

template<typename T, typename ...Args>
T* Column::PushBack(Args && ...args)
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

		// We use placement new to construct the object in a pre allocated memory
		// The pros: we don't copy, we don't call the destructor, Cons: we have to call the dtor manually

		return new (elementPtr) T(std::forward<Args>(args)...);
		//*elementPtr = T(std::forward<Args>(args)...);
	}
	return nullptr;
}

template<typename T, typename ...Args>
inline T* Column::ConstructAt(size_t index, Args && ...args)
{
	if (typeid(T) == type)
	{
		T* elementPtr = reinterpret_cast<T*>(elements.data() + index * element_size);

		return new (elementPtr) T(std::forward<Args>(args)...);
	}
	return nullptr;
}

template<typename T>
inline void Column::Destroy(size_t index)
{
	static_assert(index < m_count, "Index out of range!");
	static_assert(typeid(T) == type, "Different type!");
	if (typeid(T) == type)
	{
		T* elementPtr = reinterpret_cast<T*>(elements.data() + index * element_size);
		elementPtr->~T();

		// If the index is the last one in the column we do not need to swap
		if (index != m_count - 1)
		{
			void* lastElement = elements.data() + --m_count * element_size;
			memmove(elementPtr, lastElement, element_size);
		}
	}
}


template<typename T>
T* Column::Get(size_t row)
{
	static_assert(row < m_count, "Index out of range!");
	static_assert(typeid(T) == type, "Different type!");
	if (typeid(T) == type)
	{
		return reinterpret_cast<T*>(elements.data() + row * element_size);
	}
	return nullptr;
}
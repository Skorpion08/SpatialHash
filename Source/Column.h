#pragma once

#include "Log.h"

#include <vector>
#include <stdexcept>

struct ColumnBase
{
public:
	virtual ColumnBase* CloneType() = 0;

	virtual void* GetAddress(size_t index) = 0;

	virtual void Destroy(size_t index) = 0;

	virtual void MoveFrom(ColumnBase* other, size_t srcIndex) = 0;

protected:
	size_t m_count = 0;
};

template<typename T>
struct Column : ColumnBase
{
public:

	template <typename... Args>
	void PushBack(Args&&... args);

	template <typename... Args>
	void ConstructAt(size_t index, Args&&... args);

	ColumnBase* CloneType() override;

	void* GetAddress(size_t index) override { return elements.data() + index; }

	void MoveFrom(ColumnBase* other, size_t srcIndex) override;

	void Destroy(size_t index) override;

	T* Get(size_t index);

	inline void PreallocFor(size_t numOfElements) { elements.reserve(elements.capacity() + numOfElements); }

	inline void ResizeFor(size_t numOfElements) { elements.resize(m_count + numOfElements); }

	inline size_t Size() { return m_count; }

private:
	std::vector<T> elements = {};
};

template <typename T>
template <typename... Args>
void Column<T>::PushBack(Args&&... args)
{
	size_t newSize = m_count + 1;
	if (newSize > elements.capacity())
	{
		elements.reserve(elements.capacity() * 2);
	}
	elements.resize(newSize);

	new (&elements[m_count++]) T(args...);
}

template <typename T>
template <typename... Args>
void Column<T>::ConstructAt(size_t index, Args&&... args)
{
	if (index >= m_count) throw std::out_of_range("Column index out of range!");

	new (&elements[index]) T(args...);
}

template<typename T>
inline ColumnBase* Column<T>::CloneType()
{
	return new Column<T>;
}

template<typename T>
inline void Column<T>::MoveFrom(ColumnBase* other, size_t srcIndex)
{
	Column<T>* column = static_cast<Column<T>*>(other);

	this->PushBack(*column->Get(srcIndex));

	column->Destroy(column->m_count - 1);
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
		elements[index] = std::move(elements[m_count - 1]);
	}
	--m_count;
}

template<typename T>
T* Column<T>::Get(size_t index)
{
	if (index >= m_count) return nullptr;

	return &elements[index];
}
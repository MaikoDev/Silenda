#include "rbuffer.h"

namespace render
{
	// rBufferElem_ptr //

	template<class T>
	rBufferElem_ptr<T>& rBufferElem_ptr<T>::operator=(T* ptr)
	{
		m_ptr = ptr;

		return *this;
	}

	template<class T>
	rBufferElem_ptr<T>& rBufferElem_ptr<T>::operator=(const rBufferElem_ptr& other)
	{
		m_ptr = other.m_ptr;

		return *this;
	}

	template<class T>
	rBufferElem_ptr<T>& rBufferElem_ptr<T>::operator=(rBufferElem_ptr&& other) noexcept
	{
		m_ptr = other.m_ptr;
		other.m_ptr = nullptr;

		return *this;
	}

	// rBuffer //

	template<class T>
	rBuffer<T>::rBuffer(const uint size) : m_Size(size)
	{
		m_Buffer = std::unique_ptr<T[]>(new T[m_Size]);
	}

	template<class T>
	rBuffer<T>::rBuffer(const T def, const uint size) : m_Size(size)
	{
		m_Buffer = std::unique_ptr<T[]>(new T[m_Size]);
		for (uint i = 0; i < size; i++)
			m_Buffer[i] = def;
	}

	template<class T>
	rBuffer<T>::rBuffer(const rBuffer& other)
	{
		m_Size = other.m_Size;

		m_Buffer = std::unique_ptr<T[]>(new T[m_Size]);

		for (uint i = 0; i < m_Size; i++)
			m_Buffer[i] = other.m_Buffer[i];
	}

	template<class T>
	rBuffer<T>::rBuffer(rBuffer&& other) noexcept
	{
		m_Size = other.m_Size;
		m_Buffer = std::move(other.m_Buffer);

		other.m_Size = NULL;
	}

	template<class T>
	rBuffer<T>& rBuffer<T>::operator=(const rBuffer& other)
	{
		m_Size = other.m_Size;

		for (uint i = 0; i < m_Size; i++)
			m_Buffer[i] = other.m_Buffer[i];

		return *this;
	}

	template<class T>
	rBuffer<T>& rBuffer<T>::operator=(rBuffer&& other) noexcept
	{
		m_Size = other.m_Size;
		m_Buffer = std::move(other.m_Buffer);

		other.m_Size = NULL;

		return *this;
	}

	template<class T>
	bool rBuffer<T>::operator==(const rBuffer& other)
	{
		if (m_Size == other.m_Size)
		{
			bool compState = true;

			// Compare each element in buffer
			for (uint i = 0; i < m_Size; i++)
				compState &= (m_Buffer[i] == other.m_Buffer[i]);

			return compState;
		}

		return false;
	}

	template<class T>
	bool rBuffer<T>::operator!=(const rBuffer& other)
	{
		return !(*this == other);
	}

	template<class T>
	rBufferElem_ptr<T> rBuffer<T>::operator[](const uint index)
	{
		if (index < m_Size)
			return &m_Buffer[index];

		return nullptr;
	}

	template<class T>
	const rBufferElem_ptr<T> rBuffer<T>::operator[](const uint index) const
	{
		if (index < m_Size)
			return &m_Buffer[index];

		return nullptr;
	}
}
#pragma once

#include "types.h"

namespace render
{
	template<typename T>
	// To be used in tandom with rBuffer operator[], element ptr wrapper which does not delete upon deconstruction.
	class rBufferElem_ptr
	{
	public:
		rBufferElem_ptr() : m_ptr(nullptr) {}
		rBufferElem_ptr(T* ptr) : m_ptr(ptr) {}

		rBufferElem_ptr(const rBufferElem_ptr& other) : m_ptr(other.m_ptr) {}         // copy constructor
		rBufferElem_ptr(rBufferElem_ptr&& other) noexcept { other.m_ptr = nullptr; }  // move constructor

		~rBufferElem_ptr() {} // Does not delete ptr.

		rBufferElem_ptr& operator=(T* ptr);                                // ptr assignment
		rBufferElem_ptr& operator=(const rBufferElem_ptr& other);          // copy assignment
		rBufferElem_ptr& operator=(rBufferElem_ptr&& other) noexcept;      // move assignemnt

		// Checks if other ptr is point to the same address.
		bool operator==(const rBufferElem_ptr& other) { return (m_ptr == other.m_ptr); }
		bool operator!=(const rBufferElem_ptr& other) { return !(*this == other); }

		T& operator*() { return *m_ptr; }
		T* operator->() { return m_ptr; };
	private:
		T* m_ptr;
	};

	template<typename T>
	// Type passed to buffer must have a default constructor
	class rBuffer
	{
	public:
		rBuffer() : m_Size(NULL), m_Buffer(nullptr) {}
		rBuffer(const uint size);
		rBuffer(const T def, const uint size);
		rBuffer(const rBuffer& other);
		rBuffer(rBuffer&& other) noexcept;
		~rBuffer() {};

		rBuffer& operator=(const rBuffer& other);
		rBuffer& operator=(rBuffer&& other) noexcept;

		bool operator==(const rBuffer& other);
		bool operator!=(const rBuffer& other);
		
		rBufferElem_ptr<T> operator[](const uint index);
		const rBufferElem_ptr<T> operator[](const uint index) const;

		inline const uint& size() const& { return m_Size; };

		inline bool empty() {
			if (m_Buffer == nullptr)
				return true;
			return false;
		}
	protected:
		uint m_Size;
		std::unique_ptr<T[]> m_Buffer;
	};
}
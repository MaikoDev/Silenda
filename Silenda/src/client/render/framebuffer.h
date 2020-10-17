#pragma once

#include "rbuffer.h"
#include "fragment.h"

namespace render
{
	class FragBuffer : public rBuffer<Fragment>
	{
	public:
		FragBuffer() : rBuffer() {};
		FragBuffer(const uint size) : rBuffer(size) {};
		FragBuffer(const Fragment& def, const uint size) : rBuffer(def, size) {};

		inline void reset() {
			for (unsigned int i = 0; i < m_Size; i++)
			{
				m_Buffer[i].visual = ' ';
				m_Buffer[i].color = 0xA;
			}
		};

	};
}
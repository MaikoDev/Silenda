#pragma once

#include "meshpoint.h"
#include "..\rbuffer.h"
#include "..\rbuffer.cpp"

#include "..\..\utils\row_major.h"

namespace render
{
	class MeshFrame : public rBuffer<MeshPoint>
	{
	public:
		MeshFrame(const ushort& length, const ushort& width, const FragPos globalPos = { 0, 0, 0 });
		void reset();

		void DrawPoint(const FragColor point, const FragColor bg, const FragPos& pos);
		// Restricted to horizontal and vertical lines, cannot draw diagonal lines.
		void DrawLine(const FragPos& p1, const FragPos& p2, const FragColor point, const FragColor bg);
		void DrawRect(const short& length, const short& width, const FragColor point, const FragColor bg, const FragPos& pos);
		void DrawText(const std::wstring& txt, const FragColor point, const FragColor bg, const FragPos& pos);

		inline const void SetGlobalPos(const FragPos& pos) { m_Pos = pos; };
		
		inline const ushort& GetLength() const& { return m_Length; };
		inline const ushort& GetWidth() const& { return m_Width; };
		inline const FragPos& GetGlobalPos() const& { return m_Pos; };
	private:
		inline void DrawFrag(const Fragment& frag, const FragPos& pos)
		{
			uint i = row_major(pos.x, pos.y, m_Length);

			if (m_Buffer[i].frag != frag)
			{
				m_Buffer[i].frag = frag;
				m_Buffer[i].pos = pos;
			}
		}
	private:
		ushort m_Length;
		ushort m_Width;
		FragPos m_Pos;
	};
}
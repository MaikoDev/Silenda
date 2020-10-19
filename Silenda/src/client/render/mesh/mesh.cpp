#include "mesh.h"

namespace render
{
	MeshFrame::MeshFrame(const ushort length, const ushort width, const FragPos globalPos) : rBuffer(length * width), m_Length(length), m_Width(width), m_Pos(globalPos)
	{
		for (ushort y = 0; y < m_Width; y++)
		{
			for (ushort x = 0; x < m_Length; x++)
				m_Buffer[row_major(x, y, m_Length)].pos = { (short)x, (short)y, 0 }; // setup coordinates of fragments
		}
	}

	void MeshFrame::reset()
	{
		Fragment fragInitial;
		FragPos posInitial;

		for (ushort y = 0; y < m_Width; y++)
		{
			for (ushort x = 0; x < m_Length; x++)
			{
				uint i = row_major(x, y, m_Length);

				m_Buffer[i].frag = fragInitial;
				m_Buffer[i].pos = { (short)x, (short)y, 0 };
			}
		}
	};

	void MeshFrame::DrawPoint(const FragColor point, const FragColor bg, const FragPos& pos)
	{
		DrawFrag({ L'\u2022', point, bg }, pos);
	}

	void MeshFrame::DrawLine(const FragPos& p1, const FragPos& p2, const FragColor point, const FragColor bg)
	{
		float slope = ((float)p2.y - (float)p1.y) / ((float)p2.x - (float)p1.x);

		// Check for diagonals and determine p1 and p2 do not form a diagonal
		if (slope == 0 || isinf(slope))
		{
			if (slope == 0)
			{
				// Horizontal line
				int length = p2.x - p1.x;
				byte dir = (length) / abs(length);

				FragPos active = p1;

				for (int i = 0; i < length; i++, active.x += dir)
					DrawFrag({ L'\u2500', point, bg }, active);
			}
			else
			{
				// Vertical line
				int length = p2.y - p1.y;
				byte dir = (length) / abs(length);

				FragPos active = p1;

				for (int i = 0; i < length; i++, active.y += dir)
					DrawFrag({ L'\u2502', point, bg }, active);
			}
		}
	}

	void MeshFrame::DrawRect(const short& length, const short& width, const FragColor point, const FragColor bg, const FragPos& pos)
	{
		if (length != 0 && width != 0)
		{
			// Construct corners
			DrawFrag({ L'\u250C', point, bg }, pos);
			DrawFrag({ L'\u2510', point, bg }, { pos.x + length, pos.y, pos.z });
			DrawFrag({ L'\u2518', point, bg }, { pos.x + length, pos.y + width, pos.z });
			DrawFrag({ L'\u2514', point, bg }, { pos.x, pos.y + width, pos.z });

			// Constuct top & bottom
			for (short i = pos.x + 1; i < length; i++)
			{
				DrawFrag({ L'\u2500', point, bg }, { i, 0, 0 });
				DrawFrag({ L'\u2500', point, bg }, { i, width, 0 });
			}

			// Construct left & right
			for (short i = pos.y + 1; i < width; i++)
			{
				DrawFrag({ L'\u2502', point, bg }, { 0, i, 0 });
				DrawFrag({ L'\u2502', point, bg }, { length, i, 0 });
			}
		}
	}

	void MeshFrame::DrawText(const std::wstring& txt, const FragColor point, const FragColor bg, const FragPos& pos)
	{
		if (txt.size() < SHRT_MAX)
		{
			short p = pos.x;
			for (short i = 0; i < txt.size() && p < m_Length; i++, p++)
				DrawFrag({ txt[i], point, bg }, { p, pos.y, pos.z });
		}
	}
}
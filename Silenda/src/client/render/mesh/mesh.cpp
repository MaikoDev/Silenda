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
		DrawFrag({ (uchar)'*', point, bg }, pos);
	}

	void MeshFrame::DrawLine(const FragPos& p1, const FragPos& p2)
	{

	}
}
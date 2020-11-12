#include "mesh.h"

namespace render
{
	MeshFrame::MeshFrame(const ushort& length, const ushort& width, const FragPos globalPos) : rBuffer(length * width), m_Length(length), m_Width(width), m_Pos(globalPos)
	{
		for (ushort y = 0; y < m_Width; y++)
		{
			for (ushort x = 0; x < m_Length; x++)
				m_Buffer[row_major(x, y, m_Length)].pos = { (short)x, (short)y, BACKGROUND_DEPTH }; // setup coordinates of fragments
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
				m_Buffer[i].pos = { (short)x, (short)y, BACKGROUND_DEPTH };
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

	void MeshFrame::DrawRect(const short& length, const short& width, const FragColor point, const FragColor bg, const FragPos& pos, int lineRenderFlags, bool shouldFill)
	{
		if (length != 0 && width != 0)
		{
			//Fill MeshPoints within area with position of rect
			if (shouldFill)
			{
				short widthLimit = width + pos.y - 1, lengthLimit = length + pos.x - 1;

				for (short y = pos.y + 1; y < widthLimit; y++)
				{
					for (short x = pos.x + 1; x < lengthLimit; x++)
						m_Buffer[row_major(x, y, m_Length)].pos = { (short)x, (short)y, BACKGROUND_DEPTH + 6 };
				}
			}

			//Corner Positions
			FragPos CornerPos[4] = { pos, { (pos.x + length) - 1, pos.y, pos.z }, { pos.x, (pos.y + width) - 1, pos.z }, { (pos.x + length) - 1, (pos.y + width) - 1, pos.z } };

			//Extract corner flags
			int cornerFlags[4] = { (2 * ((lineRenderFlags & LEFT_SHADOWED) == LEFT_SHADOWED) + ((lineRenderFlags & TOP_SHADOWED) == TOP_SHADOWED)),
								   (2 * ((lineRenderFlags & RIGHT_SHADOWED) == RIGHT_SHADOWED) + ((lineRenderFlags & TOP_SHADOWED) == TOP_SHADOWED)),
								   (2 * ((lineRenderFlags & LEFT_SHADOWED) == LEFT_SHADOWED) + ((lineRenderFlags & BOTTOM_SHADOWED) == BOTTOM_SHADOWED)),
								   (2 * ((lineRenderFlags & RIGHT_SHADOWED) == RIGHT_SHADOWED) + ((lineRenderFlags & BOTTOM_SHADOWED) == BOTTOM_SHADOWED)),
			};

			// Construct corners
			wchar_t cornerCharBase = 9484;
			for (ubyte i = 0, offsetIndex = 70; i < 4; i++, offsetIndex--, cornerCharBase += 4)
			{
				//cornerChar = cornerCharBase + offset + modifier
				wchar_t cornerChar = L'\0';
				ubyte offset, modifier = 0;

				offset = offsetIndex;
				switch (cornerFlags[i])
				{
				case RectCornerShadowType::NONE:
					offset = 0;
					break;
				case RectCornerShadowType::VERTICAL:
					modifier = 1;
					break;
				case RectCornerShadowType::FULL:
					modifier = 2;
					break;
				}

				cornerChar = cornerCharBase + offset + modifier;
				DrawFrag({ cornerChar, point, bg }, CornerPos[i]);
			}

			// Constuct top & bottom
			wchar_t lineChar = 9472;
			for (short i = 0, p = pos.x + 1; i < length - 2; i++, p++)
			{
				DrawFrag({ wchar_t(lineChar + (((lineRenderFlags & TOP_SHADOWED) == TOP_SHADOWED) * 80)), point, bg }, { p, pos.y, pos.z });
				DrawFrag({ wchar_t(lineChar + (((lineRenderFlags & BOTTOM_SHADOWED) == BOTTOM_SHADOWED) * 80)), point, bg }, { p, pos.y + (width - 1), pos.z });
			}

			// Construct left & right
			lineChar = 9474;
			for (short i = 0, p = pos.y + 1; i < width - 2; i++, p++)
			{
				DrawFrag({ wchar_t(lineChar + (((lineRenderFlags & LEFT_SHADOWED) == LEFT_SHADOWED) * 79)), point, bg }, { pos.x, p, pos.z });
				DrawFrag({ wchar_t(lineChar + (((lineRenderFlags & RIGHT_SHADOWED) == RIGHT_SHADOWED) * 79)), point, bg }, { pos.x + (length - 1), p, pos.z });
			}
		}
	}

	void MeshFrame::DrawUText(const std::wstring& txt, const FragColor point, const FragColor bg, const FragPos& pos)
	{
		if (txt.size() < SHRT_MAX)
		{
			short p = pos.x;
			for (short i = 0; i < txt.size() && p < m_Length; i++, p++)
				DrawFrag({ txt[i], point, bg }, { p, pos.y, pos.z });
		}
	}
}
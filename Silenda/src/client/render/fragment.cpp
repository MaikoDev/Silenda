#include <Windows.h>

#include "fragment.h"

namespace render
{
	std::ostream& operator<<(std::ostream& os, const Fragment& frag)
	{
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD oldColorAttribs;
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

		GetConsoleScreenBufferInfo(h, &csbiInfo);
		oldColorAttribs = csbiInfo.wAttributes;

		SetConsoleTextAttribute(h, frag.color);

		os << frag.visual;

		SetConsoleTextAttribute(h, oldColorAttribs);
		return os;
	}
}
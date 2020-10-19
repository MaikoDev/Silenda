#pragma once

#include "types.h"

namespace render
{
	enum FragColor
	{
		COLOR_BLACK = 0x0,
		COLOR_DARK_BLUE = 0x1,
		COLOR_DARK_GREEN = 0x2,
		COLOR_DARK_TEAL = 0x3,
		COLOR_DARK_RED = 0x4,
		COLOR_DARK_PINK = 0x5,
		COLOR_DARK_YELLOW = 0x6,
		COLOR_DARK_WHITE = 0x7,
		COLOR_DARK_GREY = 0x8,
		COLOR_BLUE = 0x9,
		COLOR_GREEN = 0xA,
		COLOR_TEAL = 0xB,
		COLOR_RED = 0xC,
		COLOR_PINK = 0xD,
		COLOR_YELLOW = 0xE,
		COLOR_WHITE = 0xF,
		COUNT
	};

	struct ColorScheme
	{
	public:
		ColorScheme(const FragColor& txt, const FragColor& bg) : scheme_code(FragColor::COUNT * bg + txt) {};
		ColorScheme(const FragColor code) : scheme_code(code) {};
	public:
		ubyte scheme_code;
	};
}
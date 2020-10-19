#pragma once

#include <iostream>

#include "types.h"
#include "fragcolor.h"

#include <wchar.h>

namespace render
{
	struct Fragment
	{
	public:
		Fragment() {};
		Fragment(wchar_t character, const FragColor txtcolor, const FragColor bgcolor) : visual(character), color(FragColor::COUNT * bgcolor + txtcolor) {};
		
		// Unrecommended unless you specifically know the terminal scheme you want
		Fragment(wchar_t character, ubyte themecode) : visual(character), color(themecode) {};

		inline Fragment& operator=(wchar_t& character) { visual = character; return *this; };

		inline bool operator==(const Fragment& other) { return (visual == other.visual) && (color == other.color); };
		inline bool operator!=(const Fragment& other) { return !(*this == other); };
		
		inline operator wchar_t() { return visual; };
		inline operator ubyte() { return color; };

		friend std::wostream& operator<<(std::wostream& os, const Fragment& frag);
	public:
		// Actual text representation of fragment that gets printed to console.
		wchar_t visual = L' '; // default text rep is blank space - ' '

		// Terminal color code which determines the background/text color of the fragment.
		// Code is determined by: FragColor::COUNT * bgcolor + txtcolor.
		ubyte color = 0xA; // default color is Black BG and Green TXT - 0xA
	};
}
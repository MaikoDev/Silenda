#pragma once

#include <iostream>

#include "types.h"
#include "fragcolor.h"

namespace render
{
	struct Fragment
	{
	public:
		Fragment() {};
		Fragment(unsigned char character, const FragColor txtcolor, const FragColor bgcolor) : visual(character), color(FragColor::COUNT * bgcolor + txtcolor) {};
		
		// Unrecommended unless you specifically know the terminal scheme you want
		Fragment(unsigned char character, ubyte themecode) : visual(character), color(themecode) {};

		inline Fragment& operator=(const char& character) { visual = character; return *this; };

		inline bool operator==(const Fragment& other) { return (visual == other.visual) && (color == other.color); };
		inline bool operator!=(const Fragment& other) { return !(*this == other); };
		
		inline operator char() { return visual; };
		inline operator ubyte() { return color; };

		friend std::ostream& operator<<(std::ostream& os, const Fragment& frag);
	public:
		// Actual text representation of fragment that gets printed to console.
		unsigned char visual = ' '; // default text rep is blank space - ' '

		// Terminal color code which determines the background/text color of the fragment.
		// Code is determined by: FragColor::COUNT * bgcolor + txtcolor.
		ubyte color = 0xA; // default color is Black BG and Green TXT - 0xA
	};
}
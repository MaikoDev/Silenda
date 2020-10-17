#pragma once

namespace render
{
	struct FragPos
	{
	public:
		inline bool operator==(const FragPos& other) { return (xpos == other.xpos) && (ypos == other.ypos) && (zpos == other.zpos); };
		inline bool operator!=(const FragPos& other) { return !(*this == other); };
	public:
		short xpos = 0;
		short ypos = 0;
		short zpos = 32767;
	};
}
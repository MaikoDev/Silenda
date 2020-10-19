#pragma once

namespace render
{
	struct FragPos
	{
	public:
		inline bool operator==(const FragPos& other) { return (x == other.x) && (y == other.y) && (z == other.z); };
		inline bool operator!=(const FragPos& other) { return !(*this == other); };
	public:
		short x = 0;
		short y = 0;
		short z = 32767;
	};
}
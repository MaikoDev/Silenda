#pragma once

#include "..\fragment.h"
#include "..\fragpos.h"

namespace render
{
	struct MeshPoint
	{
	public:
		MeshPoint() {};
		MeshPoint(const Fragment& fragment, const FragPos& position) : frag(fragment), pos(position) {};

		~MeshPoint() {};

		inline bool operator==(const MeshPoint& other) { return (frag == other.frag) && (pos == other.pos); };
		inline bool operator!=(const MeshPoint& other) { return !(*this == other); };
	public:
		Fragment frag;
		FragPos pos;
	};
}
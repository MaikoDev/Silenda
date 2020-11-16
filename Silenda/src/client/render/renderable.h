#pragma once

#include "mesh/mesh.h"

namespace render
{
	class Renderable
	{
	public:
		virtual MeshFrame OnRender() = 0;
	};
}
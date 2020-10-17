#pragma once

#include "framebuffer.h"
#include "renderable.h"

namespace render
{
	class Renderer
	{
	public:
		~Renderer() {};

		static Renderer* GetInstance();

		void init(const uint& length, const uint& width);
		bool draw(Renderable* obj);
		void flush();
	private:
		void setCursorPos(const ushort& x, const ushort& y);
	private:
		FragBuffer m_CurrentBuffer;
		FragBuffer m_FutureBuffer;

		rBuffer<short> m_DepthBuffer;

		ushort m_FrameLength = 0;
		ushort m_FrameWidth = 0;
	private:
		Renderer() {};
		static Renderer* m_Instance;
	};
}
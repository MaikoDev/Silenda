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

		void SetWindowSize(const int& width, const int& height);
	private:
		void setCursorPos(const ushort& x, const ushort& y);
		void showCursor(const bool& showFlag);
		void resetDepth();
	private:
		FragBuffer m_CurrentBuffer;
		FragBuffer m_FutureBuffer;

		rBuffer<short> m_DepthBuffer;

		ushort m_FrameLength = 0;
		ushort m_FrameWidth = 0;

		const HWND _hwnd = GetConsoleWindow();
		const HANDLE _hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	private:
		Renderer();
		static Renderer* m_Instance;
	};
}